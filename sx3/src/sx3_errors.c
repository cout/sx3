// File: sx3_errors.c
// Author: Marc Bryant
//
// Functions used to interpret the error codes found in sx3_errors.h.
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

#include <string.h>
#include "sx3_errors.h"


// ===========================================================================
// Error code/message table
// ===========================================================================

struct sx3_error_message {
    SX3_ERROR_CODE error_code;
    char *         error_message;
};

struct sx3_error_message sx3_error_message_array[] = {

    {SX3_ERROR_SUCCESS,
    "Operation successful."},

    {SX3_ERROR_BUFFER_TOO_SMALL,
    "The buffer is too small for the requested operation."},

    {SX3_ERROR_G_VAR_NOT_FOUND,
    "The global variable does not exist."},

    {SX3_ERROR_G_VAR_WRONG_TYPE,
    "The global variable is of the wrong type."},

    {SX3_ERROR_G_VAR_READ_ONLY,
    "The global variable is read-only."},

    {SX3_ERROR_G_VAR_STRING_TOO_SHORT,
    "The global variable string is too short."},

    {SX3_ERROR_G_VAR_BAD_DATA_TYPE,
    "The global variable has a bad data type."},

    {SX3_ERROR_CONSOLE_LINE_OVERFLOW,
    "Input console line overflow."},

    {SX3_ERROR_CANNOT_OPEN_FILE,
    "Unable to open file."},
    
    {SX3_ERROR_MEM_ALLOC,
    "Unable to allocate memory."},

    // Input new error codes and messages here.
    // Remember to also update sx3_errors.h.

    {SX3_ERROR_INVALID_ERROR_CODE,
    "The error code is invalid."}
};
    

// ===========================================================================
// Function definitions
// ===========================================================================

// sx3_translate_error_code
//
// Looks up the error code and returns the error message for that error code.
// If the code does not exist, it returns an invalid error code statement.
// If the buffer is too small to accept all of the error message, this function
// will copy as much of the message as will fit into the buffer.
//
// INPUT:  code - the error code to look up
//         message - the buffer in which to copy the error message
//         length - the length of the 'message' buffer
// OUTPUT: length - number of bytes copied to buffer (excluding terminating NULL)
//
// RETURN: void
void sx3_translate_error_code(SX3_ERROR_CODE code, char *message, int *length)
{
    int i = -1;

    // If the 'message' buffer is WAY too short, return immediately
    if (*length < 2)
    {
        *length = 0;  // We did not copy any characters
        return;
    }

    // Search for error code
    while (sx3_error_message_array[++i].error_code != SX3_ERROR_INVALID_ERROR_CODE)
        if (sx3_error_message_array[i].error_code == code)
            break;

    // is the 'message' buffer long enough?
    if ( ((int)strlen(sx3_error_message_array[i].error_message)+1) > (*length) )
    {
        // No: we will copy as much of the message as will fit in the buffer
        // we reserve the last byte of message for the NULL
        *length = (*length) - 1;
    }
    else
    {
        // Yes: we will copy all of the message into the buffer
        *length = strlen(sx3_error_message_array[i].error_message);
    }
        
    strncpy (message, sx3_error_message_array[i].error_message, *length);
    message [*length] = '\0';  // Add the terminating NULL character

    return;
} // sx3_translate_error_code()
