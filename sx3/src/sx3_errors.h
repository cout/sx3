// File: sx3_errors.h
// Author: Marc Bryant
//
// Global sx3 error codes.
// All project modules should use these standardized error codes.
//
// Errors are divided into 4 classes:
//   SX3_ERROR_CLASS_INFO
//   SX3_ERROR_CLASS_WARNING   (most errors fall under this category)
//   SX3_ERROR_CLASS_CRITICAL
//   SX3_ERROR_CLASS_FATAL
//
// When adding new errors, please try to conform to the standard set in place
// in this file.  Refer to the errors already declared to see how to declare
// new errors. 
//
// When adding new errors, do not forget to add the error MESSAGE to 
// the sx3_errors.c file.

#ifndef SX3_ERRORS_H
#define SX3_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif


// ===========================================================================
// Error type definitions
// ===========================================================================
typedef unsigned long int SX3_ERROR_CODE;


// ===========================================================================
// Function declarations
// ===========================================================================
void sx3_translate_error_code (SX3_ERROR_CODE code, char *message, int *length);


// ===========================================================================
// Error macros
// ===========================================================================

// The four classes of errors ------------------------------------------------
// FIX ME!! These should ALL be static const variables!
#define SX3_ERROR_CLASS_INFO                0x00000000
#define SX3_ERROR_CLASS_WARNING             0x40000000
#define SX3_ERROR_CLASS_CRITICAL            0x80000000
#define SX3_ERROR_CLASS_FATAL               0xC0000000


// The obligatory success error code -----------------------------------------
#define SX3_ERROR_SUCCESS               (0x00000000 | SX3_ERROR_CLASS_INFO)


// General error codes -------------------------------------------------------
#define SX3_ERROR_BUFFER_TOO_SMALL      (0x00000020 | SX3_ERROR_CLASS_WARNING)
#define SX3_ERROR_CANNOT_OPEN_FILE      (0x00000021 | SX3_ERROR_CLASS_WARNING)
#define SX3_ERROR_MEM_ALLOC             (0x00000022 | SX3_ERROR_CLASS_CRITICAL)
#define SX3_ERROR_BAD_PARAMS		(0x00000023 | SX3_ERROR_CLASS_WARNING)


// Global variable error codes -----------------------------------------------
#define SX3_ERROR_G_VAR_NOT_FOUND       (0x00000080 | SX3_ERROR_CLASS_WARNING)
#define SX3_ERROR_G_VAR_WRONG_TYPE      (0x00000081 | SX3_ERROR_CLASS_WARNING)
#define SX3_ERROR_G_VAR_READ_ONLY       (0x00000082 | SX3_ERROR_CLASS_WARNING)
#define SX3_ERROR_G_VAR_STRING_TOO_SHORT (0x00000083 | SX3_ERROR_CLASS_WARNING)
#define SX3_ERROR_G_VAR_BAD_DATA_TYPE   (0x00000084 | SX3_ERROR_CLASS_WARNING)


// Console error codes -------------------------------------------------------
#define SX3_ERROR_CONSOLE_LINE_OVERFLOW (0x000000A0 | SX3_ERROR_CLASS_WARNING)


// SX3_ERROR_INVALID_ERROR_CODE ----------------------------------------------
// THIS IS RESERVED FOR SYSTEM USE!!!!!!!!!!!!
// DO NOT USE THIS ERROR CODE!!!!
#define SX3_ERROR_INVALID_ERROR_CODE    (0x00FFFFFF | SX3_ERROR_CLASS_INFO)


#ifdef __cplusplus
}
#endif
#endif
