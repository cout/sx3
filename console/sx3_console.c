/*
** sx3_console.c
**
**   This module is in charge of the input console.
**  
**   It contains a history buffer, a current line buffer, and functions
** to parse the input from the user and call the global module.
** The main purpose of the console is to allow the user to modify global
** variables (such as the wireframe state) from a command line interface.
** This command line console "sits" on top of the current screen, much
** like the input console in quake and other 3d shooters.
**
*/

#ifdef WIN32
#include <windows.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gltext.h>

#include "sx3_console.h"
#include "sx3_registry.h"
#include "sx3_console_commands.h"


/* ERROR CODES ***************************************************************/
static struct Error_Message_Entry console_error_lookup_table[] = {

    {CONSOLE_SUCCESS,
     "Operation successful"},

    {CONSOLE_ERROR_LINE_OVERFLOW,
     "Console line overflow"},

    {CONSOLE_ERROR_CRITICAL,
     "Critical error in console module"},

    {-1, NULL}

}; 


/* FUNCTION DECLARATIONS *****************************************************/
CONSOLE_ERROR_CODE process_input_line			(void);
CONSOLE_ERROR_CODE move_input_line_to_history	(void);
CONSOLE_ERROR_CODE print_console_text			(int x, int y, char *s);
int process_command_line_completion 		(const char in_char);
int process_command_history 				(const char in_char);
void console_get  (unsigned char* p1, unsigned char* p2);
void console_set  (unsigned char* p1, unsigned char* p2);
void console_help (unsigned char* p1, unsigned char* p2);
void console_list_global_variables (unsigned char* p1, unsigned char* p2);
int  call_command_function (const char *command,
							unsigned char* p1,
							unsigned char* p2);
char* get_command_description (const char *command);


/* DEFINES *******************************************************************/
#define MAX_LINES_IN_HISTORY 500 
#define MAX_LINE_LENGTH      160
#define LINE_HEIGHT          15
#define FADE_INCREMENT		 0.2F


/* LOCAL GLOBAL VARIABLES ****************************************************/
/* The history buffer contains all the console history: input and output     */
static char history_data [MAX_LINES_IN_HISTORY][MAX_LINE_LENGTH];
static int  history_start_line  = 0;     /* counters for the history buffer  */
static int  history_num_lines   = 0;
/* The command history buffer contains only a record of the commands issued  
   to the console */
static char command_history_data [MAX_LINES_IN_HISTORY][MAX_LINE_LENGTH];
static int  command_history_start_line = 0;  /* counters for the command ... */
static int  command_history_num_lines  = 1;  /* ... history buffer */
static int  command_history_line_index = 0;
/* Miscellaneous variables and flags: */
static int  console_active      = 0;     /* flag indicating active status */
static char km_activate_console = '`';   /* hot key for the console */
static int  console_lines_per_page = 20; /* used for the command line compl. */
static float console_size = (float)2/3;  /* 1=full screen, .5=half screen */
static char console_prompt[MAX_LINE_LENGTH] = "> ";  /* The command prompt */
static float back_color[] = {0.0, 0.0, 0.0, 0.7}; /* The background and */
static float fore_color[] = {1.0, 1.0, 1.0, 1.0}; /* foreground colors */
static int  scrollback = 0;              /* amount to 'scroll back' */
static float fade;						 /* fade==0: invis. -- fade==1: vis. */
static gltContext *text_context;         /* used by the gltext module */

/* The current_line buffer is assumed to be text only.  Therefore we can 
treat it as a string. */
static char current_line  [MAX_LINE_LENGTH] = "\0";

/* The command array.  This is used to store pointers to the commands
   available on the console prompt.  Other than the standard 'set', 'get',
   and 'quit' commands, these commands will be exported by other modules
   using the sx3_console_commands.h file.  
   Each function declared in sx3_console_commands.h must have two input
   vars (DWORDs - NULL if not used). */
struct Console_Command_Type {
	char *name;        /* command name */
	char *shortcut;	   /* alternative to the 'command name' */
	char *description; /* command description */
	void (*callback) (unsigned char* p1, unsigned char* p2);
};

static struct Console_Command_Type console_commands[] = {
	{
		"set",		
		"s",
		"Changes the value of a variable",	
		&console_set
	},
	{
		"get",		
		"g",
		"Returns the value of a variable",	
		&console_get
	},
	{
		"list_vars",		
		"listv",
		"Displays a list of the state variables accessible using 'get' and 'set'",
		&console_list_global_variables
	},
	{
		"help",	
		"h",
		"Obtain help on a command",			
		&console_help
	},
	{
		"quit",	
		"exit",
		"Exit the program",					
		&console_quit
	},
	/* Insert all commands before this !!!! */
	{0,0,0,NULL}
};


/* FUNCTION DEFINITIONS ******************************************************/


/* 
** sx3_console_error_lookup
**
** Calls the error lookup in the utils module
*/
const char *
sx3_console_error_lookup (
    CONSOLE_ERROR_CODE error
    )
{
    return error_lookup ( console_error_lookup_table, error );
}  /* sx3_console_error_lookup */



/*
** sx3_console_process_input 
**
** This function takes ASCII characters as input and processes them.  If the
** console is not active, the characters are discarded.  If the console is
** active, the characters are placed into the current_line and processed.
**
** If the input character is equal to the global variable 'km_activate_console,'
** then the console is either activated or de-activated, depending on it's 
** current state.
**
** INPUT:   in_char - the character to process
** OUTPUT:  none
**
** RETURN:  1 - character processed and used
**          0 - character not used (the console was not active
*/
int
sx3_console_process_input (
    const char in_char
    )
{
    int  length;               /* Length of current input line */
    int  console_activated;    /* State of the console         */
    char km_activate_console;  /* Key-map for activate_console */

    /* Check the state of the console and the key-map to change the state*/
    console_activated = sx3_get_global_bool ("console.active", NULL);
    km_activate_console = sx3_get_global_char ("keymap.activate_console", NULL);

    /* Did the user press the 'activate_console' key?  If yes, change the 
       current state of the console and return */
    if (in_char ==  km_activate_console)
    {
        if (console_activated)
            console_activated = 0;
        else
            console_activated = 1;
        sx3_force_set_global_bool ("console.active", console_activated);
        return 1;  /* character processed and used */
    }

    /* Is the console active?  If no, return without doing anything */
    if (!console_activated)
        return 0;  /* character not used */

	/* Process scroll-back characters */
	if (in_char == '[')
	{
		scrollback++;
		if (scrollback>(MAX_LINES_IN_HISTORY-100))
			scrollback = MAX_LINES_IN_HISTORY-100;
		return 1;  /* character used */
	}
	else if (in_char == ']')
	{
		scrollback--;
		if (scrollback<0)
			scrollback=0;
		return 1;  /* character used */
	}
	else if (in_char == '{')
	{
		scrollback+=console_lines_per_page;
		if (scrollback>(MAX_LINES_IN_HISTORY-100))
			scrollback = MAX_LINES_IN_HISTORY-100;
		return 1;  /* character used */
	}
	else if (in_char == '}')
	{
		scrollback-=console_lines_per_page;
		if (scrollback<0)
			scrollback=0;
		return 1;  /* character used */
	}
	scrollback = 0;  /* Any other key should reset the scrollback */

	/* Clear the line ? */
	if ((unsigned char)27 == in_char)
	{
	  	current_line[0] = '\0';
		command_history_line_index = command_history_num_lines-1;
		return 1;  /* character used */
	}
	  
	/* Process command history */
	if (process_command_history (in_char))
	  	return 1;  /* character used */
	
	/* Process command-line completion */
	if (process_command_line_completion (in_char))
		return 1;  /* character used */

	/* Is the input character a carriage return? */
    if (in_char == 13)
    {   /* Yes: process this line */
        process_input_line();
        /* move_input_line_to_history(); */
        return 1;  /* character used */
    }

    /* Get length of current input line */
    length = (int) strlen (current_line);

    /* Process the backspace character */
    if (in_char == 8)
    {
        current_line [length-1] = '\0';
        return 1;  /* character used */
    }

	/* Return an error if the line is currently full */
    if (length+1 >= MAX_LINE_LENGTH)
        return 1;  /* character used */

    /* Copy the new character to the current line */
    current_line [length]     = in_char;
    current_line [length + 1] = '\0';   /* terminating NULL */
    return 1;  /* character used */
}  /* sx3_console_process_input */


/*
** sx3_console_refresh_display
**
** This function refreshes the console display.  If the console is not
** currently active, the function returns immediately.  If the console
** is active, the console (including the history and the current line
** of text) is displayed.
**
** INPUT:   none
** OUTPUT:  none
**
** RETURN:  CONSOLE_SUCCESS
*/
CONSOLE_ERROR_CODE
sx3_console_refresh_display (
    void 
    )
{
    int i;
    int line_index;
    int print_y;
    GV_ERROR_CODE success;
    int window_size_x = sx3_get_global_int (
        "video.size.x", &success);
    int window_size_y = sx3_get_global_int (
        "video.size.y", &success);
	char temp_line [MAX_LINE_LENGTH*2/*+strlen(console_prompt)*/+1];

    /* Is the console active? */
    if (!console_active && fade<=0.0)
        return CONSOLE_SUCCESS;

	/* Verify that the size of the console is valid */
	if (console_size>1.0) console_size=1.0;
	if (console_size<0.1) console_size=0.1;

	/* The console need to fade in/out */
	if (console_active && fade<1.0)
	{
		fade += FADE_INCREMENT;
		if (fade > 1.0) fade = 1.0;
	}
	else if (!console_active && fade>0.0)
	{
		fade -= FADE_INCREMENT;
		if (fade < 0.0) fade = 0.0;
	}

    /* Save various opengl settings */
    glPushAttrib (GL_CURRENT_BIT |
                  GL_LIGHTING |
                  GL_POLYGON_MODE |
                  GL_POLYGON_BIT |
                  GL_LINE_STIPPLE |
                  GL_BLEND);

    /* Make sure that lighting is disabled */
    glDisable(GL_LIGHTING);

    /* Set the PROJECTION matrix to orthogonal */
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho (0, window_size_x, window_size_y, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    /* Draw the console background (the window) */
    /* The position of the console is hard-coded relative
       to the size fo the window.  In the future, we might want
       to use g_console_size and g_console_pos */
    glLineWidth (3.0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND); 
    
    glBegin (GL_QUADS);
		glColor4f (	back_color[0]-
					(back_color[0]/5), 
					back_color[1]-
					(back_color[1]/5),
					back_color[2]-
					(back_color[2]/5),
					(back_color[3]-
					 (back_color[3]/5)) * fade);
        glVertex2f (window_size_x, window_size_y*console_size);
		glColor4f (	back_color[0]-
					(back_color[0]/2), 
					back_color[1]-
					(back_color[1]/2),
					back_color[2]-
					(back_color[2]/2),
					(back_color[3]-
					 (back_color[3]/2)) * fade);
        glVertex2f (window_size_x, 5);
		glColor4f (	back_color[0],
					back_color[1],
					back_color[2],
					back_color[3] * fade);
        glVertex2f (0, 5);
        glVertex2f (0, window_size_y*console_size);
    glEnd ();

    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBegin (GL_LINES);
		glColor4f (	back_color[0]-
					(back_color[0]/1.5), 
					back_color[1]-
					(back_color[1]/1.5),
					back_color[2]-
					(back_color[2]/1.5),
					fade);
        glVertex2f (0, window_size_y*console_size);
        glVertex2f (window_size_x, window_size_y*console_size);
        glVertex2f (window_size_x, 5);
        glVertex2f (0, 5);
    glEnd ();
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    
    glDisable(GL_BLEND);

    /* Print the current line */
	if (!scrollback)
	{
	  	strcpy (temp_line, console_prompt);
		strcat (temp_line, current_line);
		strcat (temp_line, "<");
		print_console_text (10, (window_size_y*console_size-
                text_context->font_y - 8), temp_line);
	}
	else
	{
		print_console_text (25, (window_size_y*console_size-
                text_context->font_y - 8), "...");
	}

    /* Print the history, starting with the last line */
    for (i=history_num_lines-1; i>=0; i--)
    {
        if ((line_index = (history_start_line+i-scrollback) % MAX_LINES_IN_HISTORY) < 0)
			line_index += MAX_LINES_IN_HISTORY;
        print_y = (window_size_y*console_size-
            text_context->font_y - 8) - (history_num_lines-i)*LINE_HEIGHT;
        if (print_y < 20) break;

        print_console_text (10, print_y, history_data[line_index]);
    }

    /* Restore the PROJECTION matrix */
    glDisable (GL_BLEND);
    glColor3ub(255, 255, 255);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);

    glPopAttrib ();

    return CONSOLE_SUCCESS;
}  /* sx3_console_refresh_display */


/*
** process_input_line
**
** Parses the current input line and calls the appropriate functions.
**
** INPUT:   none
** OUTPUT:  none
**
** RETURN:  CONSOLE_SUCCESS
*/
CONSOLE_ERROR_CODE
process_input_line (
    void
    )
{
    int num_tokens;
    char command [MAX_LINE_LENGTH];
    char var     [MAX_LINE_LENGTH];
    char value   [MAX_LINE_LENGTH];

    num_tokens = sscanf (current_line, "%s %s %s", command, var, value);

    move_input_line_to_history ();

    /* Just return if the line is empty */
    if (num_tokens < 1)
        return CONSOLE_SUCCESS;
		
	/* Clear the unused parameters */
	if (num_tokens<3)
		*value = '\0';
	if (num_tokens<2)
		*var = '\0';

	/* Execute the command */
	if (call_command_function(command, 
			(unsigned char*)var,
			(unsigned char*)value))
	{
        sx3_console_print ("Invalid command.");
	}

    return CONSOLE_SUCCESS;
}  /* process_input_line */


/* 
** move_input_line_to_history
**
** Copies the current input line to the history buffer.
**
** INPUT:   none
** OUTPUT:  none
**
** RETURN:  CONSOLE_SUCCESS
*/
CONSOLE_ERROR_CODE
move_input_line_to_history (
    void
    )
{
    /* Calculate the index of the last line in the history buffer */
    int last_line = (history_start_line+history_num_lines) % MAX_LINES_IN_HISTORY;
    /* Copy the current line onto the last line in the history */
	strcpy  (history_data[last_line], console_prompt);
    strncat (history_data[last_line], current_line,
			 MAX_LINE_LENGTH-strlen(console_prompt));
	
    /* If the history fills up, overwrite the first line */
    if ((++history_num_lines) > MAX_LINES_IN_HISTORY)
    {
        history_start_line = (history_start_line+1) % MAX_LINES_IN_HISTORY;
        history_num_lines = MAX_LINES_IN_HISTORY;
    }

	/* Calculate the index of the last line in the command history buffer */
    last_line = (command_history_start_line+command_history_num_lines-1) % 
		MAX_LINES_IN_HISTORY;
	/* Copy the entered command to the current spot in the history array */
	strcpy (command_history_data[last_line], current_line);
	/* The last command is always going to be empty: it is the command
	   that the user is still working on at the time that he starts 
	   browsing the command history */
    last_line = (command_history_start_line+command_history_num_lines) % 
		MAX_LINES_IN_HISTORY;
	strcpy (command_history_data[last_line], "");
	
    /* If the command history fills up, overwrite the first line */
    if ((++command_history_num_lines) > MAX_LINES_IN_HISTORY)
    {
        command_history_start_line = (command_history_start_line+1) %
			MAX_LINES_IN_HISTORY;
        command_history_num_lines = MAX_LINES_IN_HISTORY;
    }

	/* Make sure that the current command index (into the command history) points
	   to the current command (which is an empty string) */
	command_history_line_index = command_history_num_lines-1;
	
    /* Clear the current line */
    *current_line = '\0';

    return CONSOLE_SUCCESS;
}  /* move_input_line_to_history */


/*
** print_console_text
**
** Prints the specified line of text at the specified location.  
** This function assumes that the PROJECTION matrix has been set up
** orthogonally to the window size (ie: upper left=0 x 0, 
** lower right=window_size.x x window_size.y).
**
** The current gl color is not saved.
**
** INPUT:   x - x coordinate at which to print
**          y - y corrdinate at which to print
**          s - string to print
** OUTPUT:  none
**
** RETURN:  CONSOLE_SUCCESS
*/
CONSOLE_ERROR_CODE
print_console_text (
    int x,
    int y,
    char *s
    )
{
    glColor4f ( fore_color[0], 
				fore_color[1],
				fore_color[2],
				fore_color[3] * fade);
    text_context->x = x;
    text_context->y = y;
    // glAlphaFunc(GL_GEQUAL, 0.0625);
    // glEnable(GL_ALPHA_TEST);
    gltTextureString(text_context, s);
    // gltWireString(text_context, s);

    return CONSOLE_SUCCESS;    
}  /* print_console_text */



/*
** sx3_console_print
**
** Copies the input text to the console history.
**
** INPUT:   line - pointer to string to copy to history
** OUTPUT:  none
**
** RETURN:  CONSOLE_ERROR_LINE_OVERFLOW
**			CONSOLE_SUCCESS
*/
CONSOLE_ERROR_CODE
sx3_console_print (
	const char *line
	)
{
	int last_line;  /* index of last line in the history buffer */
	
	if (strlen (line)>=MAX_LINE_LENGTH)
		return CONSOLE_ERROR_LINE_OVERFLOW;

    /* Calculate the index of the last line in the history */
    last_line = (history_start_line+history_num_lines) % MAX_LINES_IN_HISTORY;

    /* Copy the input line onto the last line in the history */
    strcpy (history_data[last_line], line);

    /* If the history fills up, overwrite the first line */
    if ((++history_num_lines) > MAX_LINES_IN_HISTORY)
    {
        history_start_line = (history_start_line+1) % MAX_LINES_IN_HISTORY;
        history_num_lines = MAX_LINES_IN_HISTORY;
    }

    return CONSOLE_SUCCESS;
}  /* sx3_console_print */



/*
** process_command_history
**
** This function takes care of the command history.  It is actually
** a fairly simple function: check to see if the user just scrolled
** back or fore in the command history.  If yes, copy the current 
** command to the current_line and update the command_history_index
** variable.
**
** INPUT:   in_char - the character typed to the console
** OUTPUT:  none
**
** RETURN:  0 - Function did nothing
**          1 - Function processed a command history function.
*/
int
process_command_history (
	const char in_char
	)
{
	int ret_value;
	int line_num;
  
  	/* Idealy, we would be using the up and down arrows to navigate
	   the command history (like every other command line interface
	   in the world).  But GLUT does not process those keys, so we
	   are using '<' and '>' for the time being. */
  	if ('<' == in_char)
	{  /* user is browsing back in the command history */
		if (command_history_num_lines)
		{
			if ((--command_history_line_index)<0)
				command_history_line_index = command_history_num_lines-1;
			line_num = command_history_start_line + command_history_line_index %
						MAX_LINES_IN_HISTORY;
			strcpy (current_line, command_history_data [line_num]);
		}
		ret_value = 1;  /* in_char was used */
	}
	else if ('>' == in_char)
	{  /* user is browsing foreward in the command history */
		if (command_history_num_lines)
		{
			command_history_line_index = (command_history_line_index + 1) %
										command_history_num_lines;
			line_num = command_history_start_line + command_history_line_index %
						MAX_LINES_IN_HISTORY;
			strcpy (current_line, command_history_data [line_num]);
		}
		ret_value = 1;  /* in_char was used */
	}
	else
	{  /* user is not browsing the command history */
		ret_value = 0;  /* in_char was not used */
	}

	return ret_value;
}  /* process_command_history */

	
	
/*
** process_command_line_completion
**
** This function takes care of the command line completion.
**
** INPUT:   in_char - the character typed to the console
** OUTPUT:  none
**
** RETURN:  0 - Function did nothing
**          1 - Function processed a clc
*/
int
process_command_line_completion (
	const char in_char
	)
{
	char temp_string[MAX_LINE_LENGTH]   = "";
	char search_string[MAX_LINE_LENGTH] = "";
	char value_string[MAX_LINE_LENGTH]  = "";
	char first_match[MAX_LINE_LENGTH]   = "";
	char other_match[MAX_LINE_LENGTH]   = "";
	int  lines;                /* Line counter                 */
    int  page_size;            /* # of lines per page          */
	static int clc_active = 0; /* Command line comp. is active */	
	
	/* Get global variables */
	page_size = sx3_get_global_int ("console.lines_per_page", NULL);
    
	/* Is there already a clc in progress?      */
    /* If yes, display the next page of matches */
    if (clc_active)
	{
		/* Did the user hit 'q' to cancel this clc? */
		if (in_char == 'q')
		{
			clc_active = 0;
			sx3_console_print ("<CANCELED>");
			return 1;
		}
		/* Dislay the next page of matches */
		lines = 0;
		sx3_find_global_var (NULL, other_match);
		while (0 != other_match[0])
		{
			/* Print current match */
			sprintf (temp_string, "   %s", other_match);
			sx3_console_print (temp_string);
			/* Break up the output into pages */
			if ((++lines)>=page_size)
			{
				sx3_console_print ("Hit any key to continue ('q' to cancel)");
				clc_active = 1;  /* Wait for user input before     */
				return 1;        /* Displaying next page           */
			}
			/* Find next match */
			sx3_find_global_var (NULL, other_match);		
		}
		clc_active = 0;  /* This clc is now complete */
		return 1;
	}  /* End clc already in progress */

	/* Start new command line completion */
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

		/* Only do clc if the command is get or set */
		if (strcasecmp (temp_string, "set") != 0 &&
			strcasecmp (temp_string, "get") != 0 &&
			strcasecmp (temp_string, "s")   != 0 &&
			strcasecmp (temp_string, "g")   != 0)
			return 0;
		sx3_find_global_var (search_string, first_match);
		
		if (first_match[0] != 0)
		{
			sx3_find_global_var (NULL, other_match);
			if (other_match[0] == 0)
			{  /* Found only one match */
				if ('.'==first_match[strlen(first_match)-1])
				  	sprintf (current_line, "%s %s%s",
						temp_string, first_match, value_string);
				else
					sprintf (current_line, "%s %s %s",
						temp_string, first_match, value_string);
			}
			else
			{  /* Found several matches: print them all out */
				sx3_console_print (current_line);
				sprintf (temp_string, "   %s", first_match);
				sx3_console_print (temp_string);
				lines = 2;
				while (0 != other_match[0])
				{
					/* Print current match */
					sprintf (temp_string, "   %s", other_match);
					sx3_console_print (temp_string);
					/* Break up the output into pages */
					if ((++lines)>=page_size)
					{
						sx3_console_print ("Hit any key to continue ('q' to cancel)");
						clc_active = 1;  /* Wait for user input before     */
						break;           /* we display the remaining lines */
					}
					/* Find next match */
					sx3_find_global_var (NULL, other_match);		
				}
			}
		}
		return 1;
	}  /* End new command line completion */

	return 0;  /* No clc processed */
}  /* process_command_line_completion */


void sx3_console_register_vars(void)
{
    /* Initialize the global vars that will be seen on the command line */
	sx3_add_global_var ("keymap.activate_console",
						SX3_GLOBAL_CHAR,
						0,
						(void*)&km_activate_console,
						0,
						NULL);
	sx3_add_global_var ("console.active",
						SX3_GLOBAL_BOOL,
						0,
						(void*)&console_active,
						0,
						NULL);
	sx3_add_global_var ("console.lines_per_page",
						SX3_GLOBAL_INT,
						0,
						(void*)&console_lines_per_page,
						0,
						NULL);

	sx3_add_global_var ("console.size",
						SX3_GLOBAL_FLOAT,
						0,
						(void*)&console_size,
						0,
						NULL);
	sx3_add_global_var ("console.back_color.r",
						SX3_GLOBAL_FLOAT,
						0,
						(void*)&back_color[0],
						0,
						NULL);
	sx3_add_global_var ("console.back_color.g",
						SX3_GLOBAL_FLOAT,
						0,
						(void*)&back_color[1],
						0,
						NULL);
	sx3_add_global_var ("console.back_color.b",
						SX3_GLOBAL_FLOAT,
						0,
						(void*)&back_color[2],
						0,
						NULL);
	sx3_add_global_var ("console.back_color.a",
						SX3_GLOBAL_FLOAT,
						0,
						(void*)&back_color[3],
						0,
						NULL);
	sx3_add_global_var ("console.fore_color.r",
						SX3_GLOBAL_FLOAT,
						0,
						(void*)&fore_color[0],
						0,
						NULL);
	sx3_add_global_var ("console.fore_color.g",
						SX3_GLOBAL_FLOAT,
						0,
						(void*)&fore_color[1],
						0,
						NULL);
	sx3_add_global_var ("console.fore_color.b",
						SX3_GLOBAL_FLOAT,
						0,
						(void*)&fore_color[2],
						0,
						NULL);
	sx3_add_global_var ("console.fore_color.a",
						SX3_GLOBAL_FLOAT,
						0,
						(void*)&fore_color[3],
						0,
						NULL);
	sx3_add_global_var ("console.prompt",
						SX3_GLOBAL_STRING,
						0,
						(void*)console_prompt,
						MAX_LINE_LENGTH,
						NULL);
}

/*
** sx3_console_init
**
** This function initializes the console.  This will need 
** to be called before the console will work correctly.
*/
CONSOLE_ERROR_CODE
sx3_console_init (
    const char * fontname
	)
{
	/* Clear the history data! */
	memset (history_data, '\0', MAX_LINES_IN_HISTORY*MAX_LINE_LENGTH);
	memset (command_history_data, '\0', MAX_LINES_IN_HISTORY*MAX_LINE_LENGTH);

    /* Initialize the gltext context */
    if ((text_context = gltNewContext ()) == 0)
        return CONSOLE_ERROR_CRITICAL;
    gltFontSize(text_context, 8, 16);
    gltLoadTextureFont(text_context, fontname); // TODO: Use the GUI

	if (sx3_get_global_bool ("console_active", NULL))
		fade = 1.0;
	else
		fade = 0.0;

	return CONSOLE_SUCCESS;
}  /* sx3_console_init */



/*
** get_command_function
** 
** calls the function callback for a specific command.
**
** INPUT:	command - name of the function
** OUTPUT:  callback - pointer to the callback function
** RETURN:	0 - success;
**			1 - command does not exist
*/
int  
call_command_function (
	const char *command,
	unsigned char* p1,
	unsigned char* p2
	)
{
	int index = 0;

	/* Search for the command, and return the callback */
	while (console_commands[index].name)
	{
		if (!strcasecmp ((char*)command, console_commands[index].name) ||
		    !strcasecmp ((char*)command, console_commands[index].shortcut))
		{
			console_commands[index].callback (p1, p2);
			return 0;
		}
		index++;
	}

	return 1;  /* command does not exist */
}  /* get_command_function */


/* 
** get_command_description
** 
** Returns the description for a specific command.
**
** INPUT:	command - name of the function
** RETURN:	pointer to the description for the command
**			(NULL if the command does not exist)
*/
char*
get_command_description (
	const char *command
	)
{
	int index = 0;

	/* Search for the command, and return the description */
	while (console_commands[index].name)
	{
		if (!strcasecmp (command, console_commands[index].name) ||
		    !strcasecmp (command, console_commands[index].shortcut))
			return console_commands[index].description;
		index++;
	}

	return NULL;  /* command does not exist */
}  /* get_command_description */

   
   
/*
** console_set
**
** Callback for the console 'set' command.
**
** INPUT:	p1 - ptr to the name of the variable to set
**			p2 - ptr to the new value of the variable
*/
void
console_set (
	unsigned char* p1,
	unsigned char* p2
	)
{
	char temp_string[128];
    
	/* Check for valid number of args */
    /*if (num_tokens!=3)*/
	if (!(*(char*)p1) || !(*(char*)p2))
    {
        sx3_console_print ("USAGE: set <var name> <var value>");
        return;
    }

    switch (sx3_set_global_value ((char*)p1, (char*)p2))
    {
    case GV_SUCCESS:
        sprintf (temp_string, "%s set successfully.", (char*)p1);
		sx3_console_print (temp_string);
        break;
    case GV_ERROR_G_VAR_NOT_FOUND:
        sprintf (temp_string, "%s does not exist.", (char*)p1);
		sx3_console_print (temp_string);
        break;
    case GV_ERROR_G_VAR_READ_ONLY:
        sprintf (temp_string, "%s is read-only.", (char*)p1);
		sx3_console_print (temp_string);
        break;
    case GV_ERROR_G_VAR_STRING_TOO_SHORT:
        sprintf (temp_string, "The '%s' string is too short to accept the new value.", (char*)p1);
		sx3_console_print (temp_string);
        break;
    default:
        sprintf (temp_string, "%s(%d): Unrecognized error returned "
            "by sx3_set_global_value.",__FILE__,__LINE__);
		sx3_console_print (temp_string);
        break;
    }  /* Switch on success of set */

	return;
}  /* console_set */



/*
** console_get
**
** Callback for the console 'get' command.
**
** INPUT:	p1 - ptr to the name of the variable to read
**			p2 - ignored
*/
void
console_get (
	unsigned char* p1,
	unsigned char* p2
	)
{
    int length = MAX_LINE_LENGTH;
	char var_name [MAX_LINE_LENGTH];
	char temp_string [MAX_LINE_LENGTH];
	char temp2_string [MAX_LINE_LENGTH];

    /* Check for valid number of args */
    /*if (num_tokens!=2)*/
	if (!(*(char*)p1) || (*(char*)p2))
    {
        sx3_console_print ("USAGE: get <var name>");
        return;
    }

	/* Print out all the variables (and values) that match the 
	   input search string */
	sx3_find_global_var ((char*)p1, var_name);
	if (var_name[0])
	{	
		while (var_name[0])
		{
    		switch (sx3_print_global_value ((char*)var_name, temp_string, &length))
    		{
    		case GV_SUCCESS:
				sprintf (temp2_string, "%s = %s", var_name, temp_string);
				sx3_console_print (temp2_string);
       	 		break;
	    	case GV_ERROR_G_VAR_NOT_FOUND:
				/* This should mean that the variable is a higher order var (ie: contains other vars) */
        		sprintf (temp_string, "%s (...)", (char*)var_name);
				sx3_console_print (temp_string);
        		break;
	    	case GV_ERROR_BUFFER_TOO_SMALL:
    	    	sprintf (temp_string, "%s = (console line too small to print value)",(char*)var_name);
				sx3_console_print (temp_string);
        		break;
	    	default:
    	   		sprintf (temp_string, "%s = %s(%d): Unrecognized error returned "
        	    	"by sx3_print_global_value.",var_name,__FILE__,__LINE__);
				sx3_console_print (temp_string);
    	    	break;
    		}  /* Switch on success of get */

			sx3_find_global_var (NULL, var_name);
			length = MAX_LINE_LENGTH;
		}
	}
	else
	{
	  	sx3_console_print ("No matching variables.");
	}
	
	return;
}  /* console_get */



/*
** console_quit
**
** Callback for the console 'quit' command.
**
** INPUT:	p1 - ignored
**			p2 - ignored
*/
/*void
console_quit (
	unsigned char* p1,
	unsigned char* p2
	)
{
	exit (0);
} */ /* console_quit */



/*
** console_help
**
** Callback for the console 'help' command.
**
** INPUT:	p1 - pointer to the name of the command
**			p2 - ignored
*/
void
console_help (
	unsigned char* p1,
	unsigned char* p2
	)
{
	char *temp_sptr;
	char temp_string[128];

	if (!(*(char*)p1))
	{
		int index = -1;
		sx3_console_print ("Console commands available:");
		while (console_commands[++index].name)
		{
			sprintf (temp_string, "  %s [%s]: %s",
				console_commands[index].name,
				console_commands[index].shortcut,
				console_commands[index].description);
			sx3_console_print (temp_string);
		}
	}
	else
	{
		if (NULL!=(temp_sptr=get_command_description ((char*)p1)))
			sx3_console_print (temp_sptr);
		else
			sx3_console_print ("Command does not exist.");
	}
	return;
}  /* console_help */



/* 
** console_list_global_variables
**
** Displays a list of the state variables accessable using 'get' and
** 'set'.  This function will be called 
** by the console module when a user enters the 
** list_vars command.  If the user also enters a variable name,
** the members of that variable will be listed (ex: 'listv view.').
**
** INPUT:	p1 - var name
**			p2 - ignored
*/
void console_list_global_variables (unsigned char* p1, unsigned char* p2)
{
	char var_name [MAX_LINE_LENGTH];
	char temp_string [MAX_LINE_LENGTH+2];

	if (p1)
	  	strcpy (temp_string, (const char*)p1);
	else
	  	strcpy (temp_string, "");	
	
	sx3_console_print ("State variables:");
	sx3_find_global_var (temp_string, var_name);
	while (var_name[0])
	{
		sprintf (temp_string, "  %s", var_name);
		sx3_console_print (temp_string);
		
		sx3_find_global_var (NULL, var_name);
	}

	return;
}  /* console_list_global_variables */

