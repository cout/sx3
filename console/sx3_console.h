/*
** sx3_console.h
**
**   Header file for the input console module.
**
*/

#ifndef _SX3_CONSOLE_H_
#define _SX3_CONSOLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sx3_utils.h>

/* ERROR CODES ***************************************************************/
typedef enum {
    CONSOLE_SUCCESS,
    CONSOLE_ERROR_LINE_OVERFLOW,
    CONSOLE_ERROR_CRITICAL,

    CONSOLE_ERROR_COUNT_,
    CONSOLE_ERROR_INVALID_,
} CONSOLE_ERROR_CODE;

    
/* FUNCTION DECLARATIONS *****************************************************/
int                 sx3_console_process_input	(const char in_char);
CONSOLE_ERROR_CODE  sx3_console_refresh_display	(void);
CONSOLE_ERROR_CODE  sx3_console_print			(const char *line);
void                sx3_console_register_vars   (void);
CONSOLE_ERROR_CODE  sx3_console_init            (const char *fontname);

#ifdef __cplusplus
}
#endif
#endif

