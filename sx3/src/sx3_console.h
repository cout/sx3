// File: sx3_console.h
// Author: Marc Bryant
//
// Header file for the input console module.

#ifndef SX3_CONSOLE_H
#define SX3_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

SX3_ERROR_CODE sx3_console_process_input   (const char in_char);
SX3_ERROR_CODE sx3_console_refresh_display (void);
SX3_ERROR_CODE console_print               (char *line);

#ifdef __cplusplus
}
#endif
#endif

