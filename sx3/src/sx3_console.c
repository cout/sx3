// File: sx3_console.c
// Author: Marc Bryant
//
// This module is in charge of the input console.
//  
// It contains a history buffer, a current line buffer, and functions
// to parse the input from the user and call the global module.
// The main purpose of the console is to allow the user to modify global
// variables (such as the wireframe state) from a command line interface.
// This command line console "sits" on top of the current screen, much
// like the input console in quake and other 3d shooters.

#ifdef __MINGW32__
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sx3.h"
#include "sx3_console.h"
#include "sx3_errors.h"
#include "sx3_global.h"
#include "sx3_gui.h"


// ===========================================================================
// Function declarations
// ===========================================================================

SX3_ERROR_CODE process_input_line            (void);
SX3_ERROR_CODE move_input_line_to_history    (void);
SX3_ERROR_CODE console_print                (char *line);
SX3_ERROR_CODE print_console_text            (int x, int y, char *s);
SX3_ERROR_CODE process_command_line_completion (const char in_char);


// ===========================================================================
// Macros
// ===========================================================================

// FIX ME!! These should be static const variables
#define MAX_LINES_IN_HISTORY 200 
#define MAX_LINE_LENGTH      160
#define LINE_HEIGHT          18


// ===========================================================================
// Global variables
// ===========================================================================

// FIX ME!! These should be put into a Console structure of some sort.
static char history_data [MAX_LINES_IN_HISTORY][MAX_LINE_LENGTH];
static int  history_start_line = 0;
static int  history_num_lines  = 0;

// The current_line buffer is assumed to be text only.  Therefore we can 
// treat it as a string.
static char current_line  [MAX_LINE_LENGTH] = "\0";


// ===========================================================================
// Function definitions
// ===========================================================================

// sx3_console_process_input 
//
// This function takes ASCII characters as input and processes them.  If the
// console is not active, the characters are discarded.  If the console is
// active, the characters are placed into the current_line and processed.
//
// If the input character is equal to the global variable 'km_activate_console,'
// then the console is either activated or de-activated, depending on it's 
// current state.
//
// INPUT:   in_char - the character to process
// OUTPUT:  none
//
// RETURN:  SX3_ERROR_SUCCESS
//          SX3_ERROR_CONSOLE_LINE_OVERFLOW

SX3_ERROR_CODE sx3_console_process_input(const char in_char)
{
    int  length;               // Length of current input line
    int  console_activated;    // State of the console
    char km_activate_console;  // Key-map for activate_console

    // Check the state of the console and the key-map to change the state
    sx3_get_global_bool ("console_active",      &console_activated);
    sx3_get_global_char ("km_activate_console", &km_activate_console);

    // Did the user press the 'activate_console' key?  If yes, change the 
    //  current state of the console and return
    if (in_char ==  km_activate_console)
    {
        if (console_activated)
            console_activated = 0;
        else
            console_activated = 1;
        sx3_force_set_global_bool("console_active", console_activated);
        return SX3_ERROR_SUCCESS;
    }

    // Is the console active?  If no, return without doing anything
    if (!console_activated)
        return SX3_ERROR_SUCCESS;

    // Process command-line completion
    if (process_command_line_completion (in_char))
        return SX3_ERROR_SUCCESS;

    // Is the input character a carriage return?
    if (in_char == 13)
    {   // Yes: process this line
        process_input_line();
        move_input_line_to_history();
        return SX3_ERROR_SUCCESS;
    }

    // Get length of current input line
    length = (int) strlen (current_line);

    // Process the backspace character
    if (in_char == 8)
    {
        current_line [length-1] = '\0';
        return SX3_ERROR_SUCCESS;
    }

    // Return an error if the line is currently full
    if(length+1 >= MAX_LINE_LENGTH)
        return SX3_ERROR_CONSOLE_LINE_OVERFLOW;

    // Copy the new character to the current line
    current_line [length]     = in_char;
    current_line [length + 1] = '\0';   // terminating NULL

    return SX3_ERROR_SUCCESS;
} // sx3_console_process_input()


// sx3_console_refresh_display
//
// This function refreshes the console display.  If the console is not
// currently active, the function returns immediately.  If the console
// is active, the console (including the history and the current line
// of text) is displayed.
//
// INPUT:   none
// OUTPUT:  none
//
// RETURN:  SX3_ERROR_SUCCESS
SX3_ERROR_CODE sx3_console_refresh_display(void)
{
    struct IPoint window_size;
    int i;
    int line_index;
    int print_y;

    // Is the console active?
    if (!g_console_active)
        return SX3_ERROR_SUCCESS;

    // Save various opengl settings
    glPushAttrib (GL_CURRENT_BIT |
                  GL_LIGHTING |
                  GL_POLYGON_MODE |
                  GL_POLYGON_BIT |
                  GL_LINE_STIPPLE |
                  GL_BLEND);

    // Make sure that lighting is disabled
    glDisable(GL_LIGHTING);

    // Query the global variables for the position of the console
    sx3_get_global_int ("window_size_x",&window_size.x);
    sx3_get_global_int ("window_size_y",&window_size.y);
    window_size.x = g_window_size.x;
    window_size.y = g_window_size.y;

    // Set the PROJECTION matrix to orthogonal
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, window_size.x, window_size.y, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw the console background (the window)
    // The position of the console is hard-coded relative
    // to the size fo the window.  In the future, we might want
    // to use g_console_size and g_console_pos
    glLineWidth (3.0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glBegin (GL_QUADS);
        glColor4ub (64, 64, 64, 128);
        glVertex2f (0, window_size.y/2);
        glVertex2f (window_size.x, window_size.y/2);
        glVertex2f (window_size.x, 5);
        glVertex2f (0, 5);
    glEnd ();
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDisable (GL_BLEND);
    glBegin (GL_LINES);
        glColor3ub (0, 0, 0);
        glVertex2f (0, window_size.y/2);
        glVertex2f (window_size.x, window_size.y/2);
        glVertex2f (window_size.x, 5);
        glVertex2f (0, 5);
    glEnd ();
    
    // Print the current line
    glColor3ub (255, 255, 255);
    glLineWidth (1.0);
    print_console_text (10, (window_size.y/2-8), ">");
    print_console_text (30, (window_size.y/2-8), current_line);
    print_console_text (strlen(current_line)*10+30, (window_size.y/2-8), "_");

    // Print the history, starting with the last line
    for (i = history_num_lines - 1; i >= 0; i--)
    {
        line_index = (history_start_line+i) % MAX_LINES_IN_HISTORY;
        print_y = (window_size.y/2-8) - (history_num_lines-i)*LINE_HEIGHT;
        if (print_y < 20) break;

        print_console_text (10, print_y, history_data[line_index]);
    }

    // Restore the PROJECTION matrix
    glColor3ub(255, 255, 255);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    glPopAttrib ();

    return SX3_ERROR_SUCCESS;
} // sx3_console_refresh_display()


// process_input_line
//
// Parses the current input line and calls the appropriate functions.
//
// INPUT:   none
// OUTPUT:  none
//
// RETURN:  SX3_ERROR_SUCCESS
SX3_ERROR_CODE process_input_line(void)
{
    int num_tokens;
    char command [MAX_LINE_LENGTH];
    char var     [MAX_LINE_LENGTH];
    char value   [MAX_LINE_LENGTH];

    num_tokens = sscanf (current_line, "%s %s %s", command, var, value);

    // Just return if the line is empty
    if (num_tokens < 1)
        return SX3_ERROR_SUCCESS;

    // Quit command ---------------------------------------------------------
    if (0==strcasecmp (command, "quit"))
    {
        exit (0);
        return SX3_ERROR_SUCCESS;
    }

    // Set command ----------------------------------------------------------
    else if (0==strcasecmp (command, "set"))
    {
        move_input_line_to_history ();

        // Check for valid number of args
        if (num_tokens!=3)
        {
            strcpy (current_line, "USAGE: set <var name> <var value>");
            return SX3_ERROR_SUCCESS;
        }

        switch (sx3_set_global_value (var, value))
        {
            case SX3_ERROR_SUCCESS:
                sprintf (current_line, "%s set successfully.", var);
                break;
            case SX3_ERROR_G_VAR_NOT_FOUND:
                sprintf (current_line, "%s does not exist.", var);
                break;
            case SX3_ERROR_G_VAR_READ_ONLY:
                sprintf (current_line, "%s is read-only.", var);
                break;
            case SX3_ERROR_G_VAR_STRING_TOO_SHORT:
                sprintf (current_line, "The '%s' string is too short to accept the new value.", var);
                break;
            default:
                sprintf (current_line, "%s(%d): Unrecognized error returned "
                    "by sx3_set_global_value.",__FILE__,__LINE__);
                break;
        }
    }

    // Get command ----------------------------------------------------------
    else if (0==strcasecmp (command, "get"))
    {
        int length = MAX_LINE_LENGTH;

        move_input_line_to_history ();

        // Check for valid number of args
        if (num_tokens!=2)
        {
            strcpy (current_line, "USAGE: get <var name>");
            return SX3_ERROR_SUCCESS;
        }

        switch (sx3_print_global_value (var, current_line, &length))
        {
            case SX3_ERROR_SUCCESS:
                break;
            case SX3_ERROR_G_VAR_NOT_FOUND:
                sprintf (current_line, "%s does not exist.", var);
                break;
            case SX3_ERROR_BUFFER_TOO_SMALL:
                sprintf (current_line, "The console line is too small to print '%s.'",var);
                break;
            default:
                sprintf (current_line, "%s(%d): Unrecognized error returned "
                    "by sx3_set_global_value.",__FILE__,__LINE__);
                break;
        }
    }

    // Cheats ---------------------------------------------------------------
    else if (0==strcasecmp (command, "god"))
    {
        move_input_line_to_history ();
        sprintf (current_line, "Cheater!");
    }

    // Unrecognized command -------------------------------------------------
    else 
    {
        move_input_line_to_history ();
        sprintf (current_line, "Unrecognized command.");
    }

    return SX3_ERROR_SUCCESS;
} // process_input_line()


// move_input_line_to_history
//
// Copies the current input line to the history buffer.
//
// INPUT:   none
// OUTPUT:  none
//
// RETURN:  SX3_ERROR_SUCCESS
SX3_ERROR_CODE move_input_line_to_history(void)
{
    // Calculate the index of the last line in the history
    int last_line = (history_start_line+history_num_lines) % MAX_LINES_IN_HISTORY;

    // Copy the current line onto the last line in the history
    strcpy (history_data[last_line], current_line);

    // If the history fills up, overwrite the first line
    if ((++history_num_lines) > MAX_LINES_IN_HISTORY)
    {
        history_start_line = (history_start_line+1) % MAX_LINES_IN_HISTORY;
        history_num_lines = MAX_LINES_IN_HISTORY;
    }

    // Clear the current line
    *current_line = '\0';

    return SX3_ERROR_SUCCESS;
} // move_input_line_to_history()


// print_console_text
//
// Prints the specified line of text at the specified location.  
// This function assumes that the PROJECTION matrix has been set up
// orthogonally to the window size (ie: upper left=0 x 0, 
// lower right=window_size.x x window_size.y).
//
// The current gl color is not saved.
//
// INPUT:   x - x coordinate at which to print
//          y - y corrdinate at which to print
//          s - string to print
// OUTPUT:  none
//
// RETURN:  SX3_ERROR_SUCCESS
SX3_ERROR_CODE print_console_text(int x, int y, char *s)
{
    // FIX ME!! I forget what the correct font parameter is.  We aren't
    // using it, though.
    sx3_draw_text(x, y-15, s, 0);

    return SX3_ERROR_SUCCESS;    
}  // print_console_text()


// console_print
//
// Copies the input text to the console history.
//
// INPUT:   line - pointer to string to copy to history
// OUTPUT:  none
//
// RETURN:  SX3_ERROR_CONSOLE_LINE_OVERFLOW
//            SX3_ERROR_SUCCESS
SX3_ERROR_CODE console_print(char *line)
{
    int last_line;  // index of last line in the history buffer
    
    if (strlen (line)>=MAX_LINE_LENGTH)
        return SX3_ERROR_CONSOLE_LINE_OVERFLOW;

    // Calculate the index of the last line in the history
    last_line = (history_start_line+history_num_lines) % MAX_LINES_IN_HISTORY;

    // Copy the input line onto the last line in the history
    strcpy (history_data[last_line], line);

    // If the history fills up, overwrite the first line
    if ((++history_num_lines) > MAX_LINES_IN_HISTORY)
    {
        history_start_line = (history_start_line+1) % MAX_LINES_IN_HISTORY;
        history_num_lines = MAX_LINES_IN_HISTORY;
    }

    return SX3_ERROR_SUCCESS;
} // console_print()


// process_command_line_completion
//
// This function takes care of the command line completion.
//
// INPUT:   in_char - the character typed to the console
// OUTPUT:  none
//
// RETURN:  0 - Function did nothing
//          1 - Function processed a clc
SX3_ERROR_CODE process_command_line_completion(const char in_char)
{
    char temp_string[MAX_LINE_LENGTH]   = "";
    char search_string[MAX_LINE_LENGTH] = "";
    char value_string[MAX_LINE_LENGTH]  = "";
    char first_match[MAX_LINE_LENGTH]   = "";
    char other_match[MAX_LINE_LENGTH]   = "";
    int  lines;                // Line counter
    int  page_size;            // # of lines per page
    static int clc_active = 0; // Command line comp. is active
    
    // Get global variables
    sx3_get_global_int ("console_lines_per_page", &page_size);
    
    // Is there already a clc in progress?
    // If yes, display the next page of matches 
    if (clc_active)
    {
        // Did the user hit 'q' to cancel this clc?
        if (in_char == 'q')
        {
            clc_active = 0;
            console_print ("<CANCELED>");
            return 1;
        }
        // Dislay the next page of matches
        lines = 0;
        sx3_find_global_var (NULL, other_match);
        while (0 != other_match[0])
        {
            // Print current match
            sprintf (temp_string, "   %s", other_match);
            console_print (temp_string);
            // Break up the output into pages
            if ((++lines)>=page_size)
            {
                console_print ("Hit any key to continue ('q' to cancel)");
                clc_active = 1;  // Wait for user input before
                return 1;        // Displaying next page
            }
            // Find next match
            sx3_find_global_var (NULL, other_match);        
        }
        clc_active = 0;  // This clc is now complete
        return 1;
    } // End clc already in progress

    // Start new command line completion
    if (in_char == 9)
    {
        temp_string[0]   = '\0';
        search_string[0] = '\0';
        value_string[0]  = '\0';
        first_match[0]   = '\0';
        other_match[0]   = '\0';

        sscanf (current_line, "%s %s %s", 
            temp_string, 
            search_string,
            value_string);

        // Only do clc if the command is get or set
        if (strcasecmp (temp_string, "set") != 0 &&
            strcasecmp (temp_string, "get") != 0)
            return 0;
        sx3_find_global_var (search_string, first_match);
        
        if (first_match[0] != 0)
        {
            sx3_find_global_var (NULL, other_match);
            if (other_match[0] == 0)
            {  // Found only one match
                sprintf (current_line, "%s %s %s",
                    temp_string, first_match, value_string);
            }
            else
            {  // Found several matches: print them all out
                console_print (current_line);
                sprintf (temp_string, "   %s", first_match);
                console_print (temp_string);
                lines = 2;
                while (0 != other_match[0])
                {
                    // Print current match
                    sprintf (temp_string, "   %s", other_match);
                    console_print (temp_string);
                    // Break up the output into pages
                    if ((++lines)>=page_size)
                    {
                        console_print ("Hit any key to continue ('q' to cancel)");
                        clc_active = 1;  // Wait for user input before
                        break;           // we display the remaining lines
                    }
                    // Find next match
                    sx3_find_global_var (NULL, other_match);        
                }
            }
        }
        return 1;
    }  // End new command line completion

    return 0; // No clc processed
} // process_command_line_completion()
