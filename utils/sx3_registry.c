/*
** sx3_global.c
**
**   Global variables.  Most of these can be accesed through the sx3 global
** variable functions.  But for the sake of speed, most of these can also be
** accessed directly.  The global variable database only keeps pointers to 
** these variables.  
**
** Note: Please use the gloabl variable access functions to access the global
** variables whenever speed is not a critical issue.
**
** Author: Marc Bryant
** Copyright (c) 2000
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sx3_registry.h"
#include "sx3_utils.h"
#include <ini.h>


static struct Error_Message_Entry gv_error_lookup_table[] = {

    {GV_SUCCESS,
    "Operation successful"},

    {GV_ERROR_BUFFER_TOO_SMALL,
    "The buffer is too small for the requested operation"},

    {GV_ERROR_CANNOT_OPEN_FILE,
    "Unable to open file"},

    {GV_ERROR_MEM_ALLOC,
    "Unable to allocate memory"},

    {GV_ERROR_BAD_PARAMS,
    "Bad parameters"},

    {GV_ERROR_G_VAR_NOT_FOUND,
    "The global variable does not exist"},

    {GV_ERROR_G_VAR_WRONG_TYPE,
    "The global variable is of the wrong type"},

    {GV_ERROR_G_VAR_READ_ONLY,
    "The global variable is read-only"},

    {GV_ERROR_G_VAR_STRING_TOO_SHORT,
    "The global variable string is too short"},

    {GV_ERROR_G_VAR_BAD_DATA_TYPE,
    "The global variable has a bad data type"},

    {GV_ERROR_G_VAR_ALREADY_EXISTS,
    "The global variable already exists"},

    {-1, NULL}
};


/* DEFINES and PROTOTYPES ****************************************************/
GV_ERROR_CODE sx3_init_global_vars        (void);

#define MAX_SEARCH_STRING_LENGTH 80
#define MAKE_SURE_DATABASE_IS_INITIALIZED if (!sx3_global_varlist) sx3_init_global_vars();

/* GLOBAL VAR ACCESS DATABASE: TYPES, ENUMS, VARS *****************************
** Structs, datatypes, and vars used by the global variable database **********
******************************************************************************/

/* generic global variable structure */
struct sx3_global_variable {
    char                         *name;
    enum sx3_global_data_types   data_type;
    void                         *data;
    int                          max_size;       /* Only for strings */
    int                          read_only;
    void                         (*update_function) (void);
};

/* 
** The global variable database itself:
**   The database keeps track of pointers to global variables.
** any caller who adds a new pointer (ie: variable) to this 
** database needs to make sure that the variable has global scope
** (static global).  
**
**   At this point, the database is being implemented as a simple array,
** realloced as necessary.  This is by no means an efficient way of doing
** this, but it works and is relatively simple.  In the future we might
** want to implement it using a faster data structure.
*/
static struct sx3_global_variable *sx3_global_varlist = NULL;
                                                                                    

/* GLOBAL VARIABLE ACCESS FUNCTIONS *******************************************
** Note: use these functions to access the global variables when speed is    **
** not a crucial issue (ie: most of the time).                               **
******************************************************************************/


/*
** sx3_gv_error_lookup
**
** Calls the error lookup in the utils module 
*/
const char *
sx3_gv_error_lookup (
	GV_ERROR_CODE error
	)
{
	return error_lookup ( gv_error_lookup_table, error );
}  /* sx3_gv_error_lookup */


/*
** sx3_read_config_file
**
** Uses the ini module to read in original config info 
*/
GV_ERROR_CODE sx3_read_config_file(const char *filename)
{
    INI_Context *ini = ini_new_context();
    const struct sx3_global_variable *var;
    const char *val;

    if(ini_load_config_file(ini, filename) != INI_OK)
    {
        ini_free_context(ini);
        return GV_ERROR_CANNOT_OPEN_FILE;
    }

    for(var = sx3_global_varlist; var->data; ++var)
    {
        if((val = ini_get_value(ini, "Global", var->name)) != 0)
        {
            printf("Setting %s to %s\n", var->name, val);
            sx3_set_global_value(var->name, val);
        }
    }

    ini_free_context(ini);
    return GV_SUCCESS;
}  /* sx3_read_config_file */


/*
** sx3_add_global_var
**
** This function adds a global variable to the database.  The database only
** stores a pointer to the variable (ie: not the value itself), so the caller
** must make sure that the pointer passed to this function refers
** to a variable that will never go out of scope.
** 
** Note: This function inserst the variables in ascending alphabetic order
** (by variable name).  This function also checks to make sure that the 
** variable does not already exist in the database.
**
** INPUT:  name - name of the new variable
**         type - type of new variable
**         read_only - TRUE==ReadOnly, FALSE==Read/Write
**         var - pointer to the new variable.
**         str_len - length of the buffer (only for strings; includes \0)
**         update_function - pointer to the function to be called when the 
**                           variable is modified
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND     (name==NULL) 
**         GV_ERROR_G_VAR_OUT_OF_MEM
**         GV_ERROR_G_VAR_BAD_DATA_TYPE 
**		   GV_ERROR_G_VAR_ALREADY_EXISTS
*/
GV_ERROR_CODE
sx3_add_global_var (
    char *name,
    enum sx3_global_data_types type,
    int read_only,
    void *var,
    int str_len,
    void (*update_function) (void)
    )
{
	int length = -1;  /* The last element in the database */
    int ii = -1;       /* the insertion index (ins. before this index) */
	struct sx3_global_variable *temp_ptr;
    int i, result;

	MAKE_SURE_DATABASE_IS_INITIALIZED
    
    /* Make sure that the name or data is not NULL */
    if (name==NULL || var==NULL)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Find the length of the database */
    while (sx3_global_varlist[++length].data);

	/* Find the insertion point (alpha by var name) */
	while ((++ii)<length)
	{
	  	result = strcasecmp (name, sx3_global_varlist[ii].name);
		/* Is this variable already in the database? */
		if (!result)
		  	return GV_ERROR_G_VAR_ALREADY_EXISTS;
		/* Have we found our insertion point? */	
		if (result<0)
		  	break;
	}
	
    /* Reallocate space for the database */
    temp_ptr = realloc (sx3_global_varlist, 
                (length+2) * sizeof (struct sx3_global_variable));

    /* return error if unable to allocate memory */
    if (NULL==temp_ptr)
        return GV_ERROR_MEM_ALLOC;
    else
        sx3_global_varlist = temp_ptr;

	/* Move all of the later elements to make room for the new 
	   inserted element */
	for (i=(length); i>=ii; i--)
	 	sx3_global_varlist[i+1] = sx3_global_varlist[i]; 

    /* Initialize the tail element */
    /*sx3_global_varlist[length+1].name = NULL;
    sx3_global_varlist[length+1].data_type = SX3_GLOBAL_INT;
    sx3_global_varlist[length+1].data = NULL;
    sx3_global_varlist[length+1].max_size = 0;
    sx3_global_varlist[length+1].read_only = 1;
    sx3_global_varlist[length+1].update_function = NULL;*/

    /* Initialize the new element */
    sx3_global_varlist[ii].name = malloc ((strlen(name)+1) * sizeof(char));
    if (NULL==sx3_global_varlist[ii].name)
        return GV_ERROR_MEM_ALLOC;       
    strcpy (sx3_global_varlist[ii].name, name);
    sx3_global_varlist[ii].read_only = read_only;
    sx3_global_varlist[ii].update_function = update_function;
    sx3_global_varlist[ii].data_type = type;
	sx3_global_varlist[ii].max_size = str_len;
	sx3_global_varlist[ii].data = var;
    /* Each var type is initialized differently */
/*    switch (type)
    {
    case SX3_GLOBAL_STRING:
		sx3_global_varlist[length].max_size = (str_len+1) * sizeof (char);
        sx3_global_varlist[length].data = (void*) malloc ((str_len+1) * sizeof (char));
        if (NULL==sx3_global_varlist[length].data)
            return GV_ERROR_MEM_ALLOC;       
        strcpy (sx3_global_varlist[length].data, value);
        break;
    case SX3_GLOBAL_INT:
        sx3_global_varlist[length].data = (void*) malloc (sizeof (int));
        if (NULL==sx3_global_varlist[length].data)
            return GV_ERROR_MEM_ALLOC;
        *((int*)(sx3_global_varlist[length].data)) = *((int*)value);
        break;
    case SX3_GLOBAL_BOOL:
        sx3_global_varlist[length].data = (void*) malloc (sizeof (int));
        if (NULL==sx3_global_varlist[length].data)
            return GV_ERROR_MEM_ALLOC;
        *((int*)(sx3_global_varlist[length].data)) = *((int*)value);
        break;
    case SX3_GLOBAL_FLOAT:
        sx3_global_varlist[length].data = (void*) malloc (sizeof (float));
        if (NULL==sx3_global_varlist[length].data)
            return GV_ERROR_MEM_ALLOC;
        *((float*)(sx3_global_varlist[length].data)) = *((float*)value);
        break;
    case SX3_GLOBAL_CHAR:
        sx3_global_varlist[length].data = (void*) malloc (sizeof (char));
        if (NULL==sx3_global_varlist[length].data)
            return GV_ERROR_MEM_ALLOC;
        *((char*)(sx3_global_varlist[length].data)) = *((char*)value);
        break;
    case SX3_GLOBAL_PTR:
        sx3_global_varlist[length].data = (void*) malloc (sizeof (void*));
        if (NULL==sx3_global_varlist[length].data)
            return GV_ERROR_MEM_ALLOC;
        *((void**)(sx3_global_varlist[length].data)) = *((void**)value);
        break;
    case SX3_GLOBAL_VECTOR:
        sx3_global_varlist[length].data = (void*) malloc (sizeof (float[4]));
        if (NULL==sx3_global_varlist[length].data)
            return GV_ERROR_MEM_ALLOC;
        for (i=0; i<4; i++)
            ((float*)sx3_global_varlist[length].data)[i] = ((float*)value)[i];
        break;

    default:   uh-oh: bad data type! */
/*        return GV_ERROR_G_VAR_BAD_DATA_TYPE;
    }   end switch on type */

    return GV_SUCCESS;    
}  /* sx3_add_global_var */



/* 
** sx3_release_global_vars
**
** This funtion releases all the dynamic memory allocated
** by the global variable database
**
** RETURN: GV_SUCCESS
*/
GV_ERROR_CODE
sx3_release_global_vars (
	void 
	)
{
    int i=-1;		

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Free the name and data fields for each var */
    while (sx3_global_varlist[++i].data)
    {
        if (sx3_global_varlist[i].name)
            free (sx3_global_varlist[i].name);
    }

    /* free the database itself */
    free (sx3_global_varlist);
	sx3_global_varlist = NULL;  /* So that we can re-initialize it */

    return GV_SUCCESS;
}  /* sx3_release_global_vars */



/* 
** sx3_init_global_vars
** 
** Allocate space for an empty variable database.
** At this time, the only element is the tail element.
** If space has already been alocated for the database,
** return immediately.  (use sx3_release_global_vars
** to deallocate the database, then use sx3_init_global_vars
** to reallocate it).
**
** RETURN: GV_SUCCESS
*/
GV_ERROR_CODE
sx3_init_global_vars (
   void
   )
{
	/* If space has allready been allocated for the database, return success */
	if (sx3_global_varlist)
		return GV_SUCCESS;

    /* Allocate space for an empty database */
    sx3_global_varlist = (struct sx3_global_variable*) malloc (
                            sizeof (struct sx3_global_variable));
    /* Return an error if we were unable to allocate the space */
    if (NULL == sx3_global_varlist)
        return GV_ERROR_MEM_ALLOC;

    /* Initialize the tail element */
    sx3_global_varlist->name = NULL;
    sx3_global_varlist->data_type = SX3_GLOBAL_INT;
    sx3_global_varlist->data = NULL;
    sx3_global_varlist->max_size = 0;
    sx3_global_varlist->read_only = 1;
    sx3_global_varlist->update_function = NULL;

    return GV_SUCCESS;  /* Success! */
}  /* sx3_init_global_vars */


/* 
** find_sx3_global_variable 
**
** finds the desired variable and returns the index for that var.
**
** INPUT:  var - the name of the desired variable
** RETURN: <0 - variable not found
**         >=0 - index of variable
*/
int
find_sx3_global_variable (
    const char *var
    )
{
    int i = -1;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    while (sx3_global_varlist[++i].data)
        if (0 == strcasecmp(var, sx3_global_varlist[i].name))
            return i;

    return -1;
}  /* find_sx3_global_variable */


/*
** sx3_print_global_value 
**
** Outputs the desired value to buffer (in ASCII format).
**
** INPUT:  var - The name of the variable to print
**         buffer - pointer to the buffer that will accept the value
**         length - size in bytes of buffer
** OUTPUT: length - number of bytes copied to buffer (excluding terminating NULL)
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_BUFFER_TOO_SMALL 
*/         
GV_ERROR_CODE
sx3_print_global_value (
    const char *var, 
    char *buffer, 
    int *length
    )
{
    int var_index;
    char temp_string[32];

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* This function behaves differently depending on the variable's data type */
    switch (sx3_global_varlist[var_index].data_type)
    {
    case SX3_GLOBAL_STRING:
        if ( (int)(strlen(sx3_global_varlist[var_index].data) + 1) > *length )
            return GV_ERROR_BUFFER_TOO_SMALL;  /* Buffer size insufficient */
        strcpy (buffer, sx3_global_varlist[var_index].data);
        *length = strlen (buffer);
        break;

    case SX3_GLOBAL_INT:
        /* don't forget the terminating null character! */
        if ( (sprintf (temp_string,"%d",*(int*)(sx3_global_varlist[var_index].data)) + 1) > 
             (*length) )
            return GV_ERROR_BUFFER_TOO_SMALL;  /* Buffer size insufficient */
        strcpy (buffer, temp_string);
        *length = strlen (buffer);
        break;

    case SX3_GLOBAL_BOOL:  /* This is actually an int */
        /* don't forget the terminating null character! */
        if ( (int)(strlen ("false") + 1) > *length )
            return GV_ERROR_BUFFER_TOO_SMALL;  /* Buffer size insufficient */
        if (*(int*)(sx3_global_varlist[var_index].data))
            strcpy (buffer, "True");
        else
            strcpy (buffer, "False");
        break;

    case SX3_GLOBAL_FLOAT:
        /* don't forget the terminating null character! */
        if ( (sprintf (temp_string,"%f",*(float*)(sx3_global_varlist[var_index].data)) + 1) >
             *length )
            return GV_ERROR_BUFFER_TOO_SMALL;  /* Buffer size insufficient */
        strcpy (buffer, temp_string);
        *length = strlen (buffer);
        break;

    case SX3_GLOBAL_DOUBLE:
        /* don't forget the terminating null character! */
        if ( (sprintf (temp_string,"%f",*(double*)(sx3_global_varlist[var_index].data)) + 1) >
             *length )
            return GV_ERROR_BUFFER_TOO_SMALL;  /* Buffer size insufficient */
        strcpy (buffer, temp_string);
        *length = strlen (buffer);
        break;

    case SX3_GLOBAL_CHAR:
        if (*length < 1)
            return GV_ERROR_BUFFER_TOO_SMALL;  /* Buffer size insufficient */
        *buffer = *(char*)(sx3_global_varlist[var_index].data);
        *(buffer+1) = '\0';
        *length = 1;
        break;

    default:
        break;
        /* We should never reach this point!!!! */
    }  /* switch on data_type */

    return GV_SUCCESS;  /* success */
}  /* sx3_print_global_value */


/*
** sx3_set_global_int
** 
** Sets the desired global variable to the value specified 
**
** INPUT:  var - The name of the variable to set
**         value - the new value for this variable
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
**         GV_ERROR_G_VAR_READ_ONLY
*/
GV_ERROR_CODE
sx3_set_global_int (
    const char *var, 
    const int value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_INT)
        return GV_ERROR_G_VAR_WRONG_TYPE;

    /* Is the variable read only ? */
    if (sx3_global_varlist[var_index].read_only)
        return GV_ERROR_G_VAR_READ_ONLY;

    /* set the value */
	*(int*)(sx3_global_varlist[var_index].data) = value;

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_set_global_int */


/*
** sx3_set_global_float
** 
** Sets the desired global variable to the value specified 
**
** INPUT:  var - The name of the variable to set
**         value - the new value for this variable
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
**         GV_ERROR_G_VAR_READ_ONLY
*/
GV_ERROR_CODE
sx3_set_global_float (
    const char *var, 
    const float value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_FLOAT)
        return GV_ERROR_G_VAR_WRONG_TYPE;

    /* Is the variable read only ? */
    if (sx3_global_varlist[var_index].read_only)
        return GV_ERROR_G_VAR_READ_ONLY;

    /* set the value */
	*(float*)(sx3_global_varlist[var_index].data) = value;

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_set_global_float */


/*
** sx3_set_global_double
** 
** Sets the desired global variable to the value specified 
**
** INPUT:  var - The name of the variable to set
**         value - the new value for this variable
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
**         GV_ERROR_G_VAR_READ_ONLY
*/
GV_ERROR_CODE
sx3_set_global_double (
    const char *var, 
    const double value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_DOUBLE)
        return GV_ERROR_G_VAR_WRONG_TYPE;

    /* Is the variable read only ? */
    if (sx3_global_varlist[var_index].read_only)
        return GV_ERROR_G_VAR_READ_ONLY;

    /* set the value */
	*(double*)(sx3_global_varlist[var_index].data) = value;

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_set_global_double */

   
/*
** sx3_set_global_string
** 
** Sets the desired global variable to the value specified 
**
** INPUT:  var - The name of the variable to set
**         value - pointer to the new string buffer (NULL terminated)
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
**         GV_ERROR_G_VAR_STRING_TOO_SHORT
**         GV_ERROR_G_VAR_READ_ONLY
*/
GV_ERROR_CODE
sx3_set_global_string (
    const char *var, 
    const char *value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_STRING)
        return GV_ERROR_G_VAR_WRONG_TYPE;

    /* Is the global variable string long enough? (remember the terminating NULL) */
    if ( (int)(strlen(value) + 1) > 
         sx3_global_varlist[var_index].max_size )
         return GV_ERROR_G_VAR_STRING_TOO_SHORT;

    /* Is the variable read only ? */
    if (sx3_global_varlist[var_index].read_only)
        return GV_ERROR_G_VAR_READ_ONLY;

	/* set the value */
    strcpy (sx3_global_varlist[var_index].data, value);

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_set_global_string */


/*
** sx3_set_global_bool
** 
** Sets the desired global variable to the value specified 
**
** INPUT:  var - The name of the variable to set
**         value - The new value for this variable
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
**         GV_ERROR_G_VAR_READ_ONLY
*/
GV_ERROR_CODE
sx3_set_global_bool (
    const char *var, 
    const int value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_BOOL)
        return GV_ERROR_G_VAR_WRONG_TYPE;

    /* Is the variable read only ? */
    if (sx3_global_varlist[var_index].read_only)
        return GV_ERROR_G_VAR_READ_ONLY;

    /* set the value */
	*(int*)(sx3_global_varlist[var_index].data) = value;

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_set_global_bool */


/*
** sx3_set_global_char
** 
** Sets the desired global variable to the value specified 
**
** INPUT:  var - The name of the variable to set
**         value - the new value for this variable
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
**         GV_ERROR_G_VAR_READ_ONLY
*/
GV_ERROR_CODE
sx3_set_global_char (
    const char *var, 
    const char value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_CHAR)
        return GV_ERROR_G_VAR_WRONG_TYPE;

    /* Is the variable read only ? */
    if (sx3_global_varlist[var_index].read_only)
        return GV_ERROR_G_VAR_READ_ONLY;

	/* set the value */
	*(char*)(sx3_global_varlist[var_index].data) = value;

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_set_global_char */

   
/*
** sx3_get_global_int
** 
** Returns the value of the specified global variable.  If an 
** error occurs, the function returns 0, and the error code is
** returned in error (if the error pointer passed in was not NULL)
**
** INPUT:  var - The name of the variable to retrieve
** OUTPUT: error - error value (ignored if input as NULL)
**                   GV_SUCCESS
**                   GV_ERROR_G_VAR_NOT_FOUND
**                   GV_ERROR_G_VAR_WRONG_TYPE
**
** RETURN: value of the desired variable
*/
int
sx3_get_global_int (
    const char *var, 
    GV_ERROR_CODE *error
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
	{
		if (error)
			*error = GV_ERROR_G_VAR_NOT_FOUND;
		return 0;
	}

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_INT)
	{
		if (error)
			*error = GV_ERROR_G_VAR_WRONG_TYPE;
        return 0;
	}

	if (error)
		*error = GV_SUCCESS; 
	return (*(int*)(sx3_global_varlist[var_index].data));
}  /* sx3_get_global_int */


/*
** sx3_get_global_float
** 
** Returns the value of the specified global variable.  If an 
** error occurs, the function returns 0, and the error code is
** returned in error (if the error pointer passed in was not NULL)
**
** INPUT:  var - The name of the variable to retrieve
** OUTPUT: error - error value (ignored if input as NULL)
**                   GV_SUCCESS
**                   GV_ERROR_G_VAR_NOT_FOUND
**                   GV_ERROR_G_VAR_WRONG_TYPE
**
** RETURN: value of the desired variable
*/
float
sx3_get_global_float (
    const char *var, 
    GV_ERROR_CODE *error
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
	{
		if (error)
			*error = GV_ERROR_G_VAR_NOT_FOUND;
        return 0;
	}

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_FLOAT)
	{
		if (error)
			*error = GV_ERROR_G_VAR_WRONG_TYPE;
        return 0;
	}

	if (error)
		*error = GV_SUCCESS; 
	return (*(float*)(sx3_global_varlist[var_index].data));
}  /* sx3_get_global_float */


/*
** sx3_get_global_double
** 
** Returns the value of the specified global variable.  If an 
** error occurs, the function returns 0, and the error code is
** returned in error (if the error pointer passed in was not NULL)
**
** INPUT:  var - The name of the variable to retrieve
** OUTPUT: error - error value (ignored if input as NULL)
**                   GV_SUCCESS
**                   GV_ERROR_G_VAR_NOT_FOUND
**                   GV_ERROR_G_VAR_WRONG_TYPE
**
** RETURN: value of the desired variable
*/
double
sx3_get_global_double (
    const char *var, 
    GV_ERROR_CODE *error
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
	{
		if (error)
			*error = GV_ERROR_G_VAR_NOT_FOUND;
        return 0;
	}

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_DOUBLE)
	{
		if (error)
			*error = GV_ERROR_G_VAR_WRONG_TYPE;
        return 0;
	}

	if (error)
		*error = GV_SUCCESS; 
	return (*(double*)(sx3_global_varlist[var_index].data));
}  /* sx3_get_global_double */


/*
** sx3_get_global_bool
** 
** Returns the value of the specified global variable.  If an 
** error occurs, the function returns 0, and the error code is
** returned in error (if the error pointer passed in was not NULL)
**
** INPUT:  var - The name of the variable to retrieve
** OUTPUT: error - error value (ignored if input as NULL)
**                   GV_SUCCESS
**                   GV_ERROR_G_VAR_NOT_FOUND
**                   GV_ERROR_G_VAR_WRONG_TYPE
**
** RETURN: value of the desired variable
*/
int
sx3_get_global_bool (
    const char *var, 
    GV_ERROR_CODE *error
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
	{
		if (error)
			*error = GV_ERROR_G_VAR_NOT_FOUND;
        return 0;
	}

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_BOOL)
	{
		if (error)
			*error = GV_ERROR_G_VAR_WRONG_TYPE;
		return 0;
	}

	if (error)
		*error = GV_SUCCESS; 
    return (*(int*)(sx3_global_varlist[var_index].data));
}  /* sx3_get_global_bool */


/*
** sx3_get_global_string
** 
** Returns the value of the specified global variable.  If an 
** error occurs, the function returns 0, and the error code is
** returned in error (if the error pointer passed in was not NULL)
**
** INPUT:  var - The name of the variable to retrieve
**         buffer - Ptr to buffer where we want the global string copied
**         length - Size (in bytes) of the buffer
** OUTPUT: error - error value (ignored if input as NULL)
**                   GV_SUCCESS
**                   GV_ERROR_G_VAR_NOT_FOUND
**                   GV_ERROR_G_VAR_WRONG_TYPE
** RETURN: The number of bytes copied to 'value' buffer (excluding the
**         terminating NULL)
*/
int
sx3_get_global_string (
    const char *var, 
    char *buffer,
    int length,
	GV_ERROR_CODE *error
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
	{
		if (error)
			*error = GV_ERROR_G_VAR_NOT_FOUND;
        return 0;
	}

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_STRING)
	{
		if (error)
			*error = GV_ERROR_G_VAR_WRONG_TYPE;
        return 0;
	}

    /* Is the 'buffer' buffer long enough?  (Don't forget about terminating NULL) */
    if ( (int)(strlen(sx3_global_varlist[var_index].data) + 1) > length )
	{
		if (error)
			*error = GV_ERROR_BUFFER_TOO_SMALL;  /* Buffer size insufficient */
        return 0;
	}

    strcpy (buffer, sx3_global_varlist[var_index].data);
	if (error)
		*error = GV_SUCCESS; 

    return (strlen(buffer));  /* Success! */
}  /* sx3_get_global_string */


/*
** sx3_get_global_char
** 
** Returns the value of the specified global variable.  If an 
** error occurs, the function returns 0, and the error code is
** returned in error (if the error pointer passed in was not NULL)
**
** INPUT:  var - The name of the variable to retrieve
** OUTPUT: error - error value (ignored if input as NULL)
**                   GV_SUCCESS
**                   GV_ERROR_G_VAR_NOT_FOUND
**                   GV_ERROR_G_VAR_WRONG_TYPE
**
** RETURN: value of the desired variable
*/
char
sx3_get_global_char (
    const char *var, 
    GV_ERROR_CODE *error
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
	{
		if (error)
			*error = GV_ERROR_G_VAR_NOT_FOUND;
        return 0;
	}

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_CHAR)
	{
		if (error)
			*error = GV_ERROR_G_VAR_WRONG_TYPE;
        return 0;
	}

	if (error)
		*error = GV_SUCCESS; 
	return (*(char*)(sx3_global_varlist[var_index].data));
}  /* sx3_get_global_char */

   
/*
** sx3_set_global_value
** 
** Sets a variable to the desired value.  
**
** INPUT:  var - The name of the variable to retrieve
**         value - Ptr to string containing new value
** OUTPUT: 
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_STRING_TOO_SHORT
**         GV_ERROR_G_VAR_READ_ONLY
**         GV_ERROR_G_VAR_BAD_DATA_TYPE
*/
GV_ERROR_CODE
sx3_set_global_value (
    const char *var, 
    const char *value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Determine the data type */
    switch (sx3_global_varlist[var_index].data_type)
    {
    case SX3_GLOBAL_STRING:
        return sx3_set_global_string (var, value);
    case SX3_GLOBAL_INT:
        return sx3_set_global_int (var, atoi(value));
    case SX3_GLOBAL_BOOL:
        if ( 0==strcasecmp(value, "true") ||
             0==strcasecmp(value, "on") )
            return sx3_set_global_bool (var, 1);
        if ( 0==strcasecmp(value, "false") ||
             0==strcasecmp(value, "off") )
            return sx3_set_global_bool (var, 0);
        return sx3_set_global_bool (var, atoi(value));
    case SX3_GLOBAL_FLOAT:
        return sx3_set_global_float (var, (float)atof(value));
    case SX3_GLOBAL_DOUBLE:
        return sx3_set_global_double (var, (double)atof(value));
    case SX3_GLOBAL_CHAR:
        return sx3_set_global_char (var, *value);
    default:
        /* We should never get here */
        return GV_ERROR_G_VAR_BAD_DATA_TYPE;
    }  /* switch on data type */
}  /* sx3_set_global_value */


/*
** sx3_force_set_global_value
** 
** Sets a variable to the desired value, regardless of read-only 
** status.
**
** INPUT:  var - The name of the variable to retrieve
**         value - Ptr to string containing new value
** OUTPUT: 
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_STRING_TOO_SHORT
*/
GV_ERROR_CODE
sx3_force_set_global_value (
    const char *var, 
    const char *value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Determine the data type */
    switch (sx3_global_varlist[var_index].data_type)
    {
    case SX3_GLOBAL_STRING:
        return sx3_force_set_global_string (var, value);
    case SX3_GLOBAL_INT:
        return sx3_force_set_global_int (var, atoi(value));
    case SX3_GLOBAL_BOOL:
        if ( 0==strcasecmp(value, "true") ||
             0==strcasecmp(value, "on") )
            return sx3_force_set_global_bool (var, 1);
        if ( 0==strcasecmp(value, "false") ||
             0==strcasecmp(value, "off") )
            return sx3_force_set_global_bool (var, 0);
        return sx3_force_set_global_bool (var, atoi(value));
    case SX3_GLOBAL_FLOAT:
        return sx3_force_set_global_float (var, (float)atof(value));
    case SX3_GLOBAL_DOUBLE:
        return sx3_force_set_global_double (var, (double)atof(value));
    default:
        /* We should never get here */
        return GV_ERROR_G_VAR_BAD_DATA_TYPE;
    }  /* switch on data type */
}  /* sx3_force_set_global_value */

   
/*
** sx3_force_set_global_int
** 
** Sets the desired global variable to the value specified, regardless of read-only 
** status. 
**
** INPUT:  var - The name of the variable to set
**         value - the new value for this variable
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
*/
GV_ERROR_CODE
sx3_force_set_global_int (
    const char *var, 
    const int value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_INT)
        return GV_ERROR_G_VAR_WRONG_TYPE;

	/* set value */
    *(int*)(sx3_global_varlist[var_index].data) = value;

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_force_set_global_int */


/*
** sx3_force_set_global_float
** 
** Sets the desired global variable to the value specified, regardless of read-only 
** status.
**
** INPUT:  var - The name of the variable to set
**         value - the new value for this variable
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
*/
GV_ERROR_CODE
sx3_force_set_global_float (
    const char *var, 
    const float value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_FLOAT)
        return GV_ERROR_G_VAR_WRONG_TYPE;

	/* set value */
    *(float*)(sx3_global_varlist[var_index].data) = value;

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_force_set_global_float */


/*
** sx3_force_set_global_double
** 
** Sets the desired global variable to the value specified, regardless of read-only 
** status.
**
** INPUT:  var - The name of the variable to set
**         value - the new value for this variable
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
*/
GV_ERROR_CODE
sx3_force_set_global_double (
    const char *var, 
    const double value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_DOUBLE)
        return GV_ERROR_G_VAR_WRONG_TYPE;

	/* set value */
    *(double*)(sx3_global_varlist[var_index].data) = value;

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_force_set_global_double */

   
/*
** sx3_force_set_global_string
** 
** Sets the desired global variable to the value specified, regardless of read-only 
** status.
**
** INPUT:  var - The name of the variable to set
**         value - pointer to the new string buffer (NULL terminated)
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
**         GV_ERROR_G_VAR_STRING_TOO_SHORT
*/
GV_ERROR_CODE
sx3_force_set_global_string (
    const char *var, 
    const char *value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_STRING)
        return GV_ERROR_G_VAR_WRONG_TYPE;

    /* Is the global variable string long enough? (remember the terminating NULL) */
    if ( (int)(strlen(value) + 1) > 
         sx3_global_varlist[var_index].max_size )
         return GV_ERROR_G_VAR_STRING_TOO_SHORT;

	/* set value */
    strcpy (sx3_global_varlist[var_index].data, value);

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_force_set_global_string */


/*
** sx3_force_set_global_bool
** 
** Sets the desired global variable to the value specified, regardless of read-only 
** status.
**
** INPUT:  var - The name of the variable to set
**         value - The new value for this variable
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
**         GV_ERROR_G_VAR_WRONG_TYPE
*/
GV_ERROR_CODE
sx3_force_set_global_bool (
    const char *var, 
    const int value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_BOOL)
        return GV_ERROR_G_VAR_WRONG_TYPE;

	/* set value */
    *(int*)(sx3_global_varlist[var_index].data) = value;

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_force_set_global_bool */

   
/*
** sx3_force_set_global_char
** 
** Sets the desired global variable to the value specified 
**
** INPUT:  var - The name of the variable to set
**         value - the new value for this variable
** OUTPUT: none
**
** RETURN: GV_SUCCESS
**         GV_ERROR_G_VAR_NOT_FOUND
*/
GV_ERROR_CODE
sx3_force_set_global_char (
    const char *var, 
    const char value
    )
{
    int var_index;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Does the global variable exist? */
    if ((var_index=find_sx3_global_variable(var)) < 0)
        return GV_ERROR_G_VAR_NOT_FOUND;

    /* Is the variable of the correct data type? */
    if (sx3_global_varlist[var_index].data_type!=SX3_GLOBAL_CHAR)
        return GV_ERROR_G_VAR_WRONG_TYPE;

	/* set value */
    *(char*)(sx3_global_varlist[var_index].data) = value;

	/* If there is a callback defined, call it */
	if (sx3_global_varlist[var_index].update_function)
		sx3_global_varlist[var_index].update_function();

    return GV_SUCCESS;  /* Success! */
}  /* sx3_force_set_global_char */


/*
** sx3_find_global_var
** 
** This function will search through the global variable database
** and return the names of any variables that match the search string.
** The function only compares the first n characters, where 
** n = strlen (search_string), so it can be used as a pseudo-
** command line completion search.  Only one var name is returned per
** call to sx3_find_global_var: subsequent calls with a NULL search
** string will return subsequent matching vars in the database.
**
** If the variable name contains a period ('.'), then this function
** will only return the portion of the name before (and up to) the 
** period.  This makes it possible to group var names into pseudo-
** structures (ie: color.r).  This will only work correctly if the
** variables in question have been entered in sequential order.
**
** INPUT:  ss - Search string (var names are compared against this)
**              If (ss==NULL) then continue previous search.
** OUTPUT: var_name - The name of the next matching var
**              If (var_name==NULL) then no matches found.
**              CAUTION:
**              strlen (var_name) must be >= MAX_LINE_LENGTH
**
** RETURN: GV_SUCCESS
*/
GV_ERROR_CODE
sx3_find_global_var (
    const char *ss, 
    char *var_name
    )
{
    static char current_ss [MAX_SEARCH_STRING_LENGTH] = "";    
    static int var_index = 0;
    static int ss_length = 0;
	char *temp_ptr;

	MAKE_SURE_DATABASE_IS_INITIALIZED

    /* Determine search string and length */
    if (ss!=NULL)
    {
        /* New search string: initialize static vars */
        if ( (ss_length=strlen(ss)) >= MAX_SEARCH_STRING_LENGTH )
            ss_length = MAX_SEARCH_STRING_LENGTH-1; 
        strncpy (current_ss, ss, ss_length);
        current_ss [ss_length] = (char)0;
        var_index = 0;
    }

    /* Return if there is not search defined */
    /*if (0 == ss_length)
    {
        var_name [0] = (char)0;
        return GV_SUCCESS;
    }*/

    /* Search for the var */
    while (sx3_global_varlist[var_index].data)
    {
        if ( (0 == strncasecmp(current_ss, 
                    sx3_global_varlist[var_index].name,
                    ss_length)) )
        {
            strcpy (var_name, sx3_global_varlist[var_index].name);
            var_index++;
		  	if ((temp_ptr = strchr (var_name + ss_length, '.')) != NULL)
			{	
				var_name [temp_ptr-var_name+1] = '\0';
				/*strncpy (temp_string, var_name, temp_ptr - var_name); */
				while (sx3_global_varlist[var_index].data &&
				       !strncasecmp(var_name,
					   		sx3_global_varlist[var_index].name,
							strlen (var_name)))
					   var_index++;
			}
            return GV_SUCCESS;  /* Match found! */
        }
        var_index++;
    }

    /* No matches found */
    var_name[0] = (char)0;
    return GV_SUCCESS;
}  /* sx3_find_global_var */

