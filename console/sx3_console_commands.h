/*
** sx3_console_commands.h
**
** Prototypes for the callbacks used by the console.
** The callbacks are associated to console commands via
** the console_commands[] array in the sx3_console.c file.
**
** For example, if a module wants to export a function called
** find_smurf, called when the user enters 'find_smurf' on
** the console prompt, the following has to be done:
** 1. Declare the function find_smurf in this file
** 2. Define the function find_smurf in the smurf module
** 3. Associate the find_smurf function with the 'find_smurf'
**    command by adding an entry to the console_commands[]
**    array in the sx3_console.c file.
**
** The callback functions must use the following template:
**   void function_name (unsigned char* p1, unsigned char* p2);
*/

void console_quit    (unsigned char* p1, unsigned char* p2);
