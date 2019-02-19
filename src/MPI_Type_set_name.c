/* 
MPI_Type_set_name

   set datatype name
int MPI_Type_set_name(
  MPI_Datatype type,
  char *type_name
);

int MPI_Type_set_name(
  MPI_Datatype type,
  wchar_t *type_name
);

Parameters

   type
          [in] datatype whose identifier is to be set (handle)

   type_name
          [in] the character string which is remembered as the name
          (string)

Remarks

   Set the name associated with a datatype.  The parameter type_name must
   not be longer than MPI_MAX_OBJECT_NAME characters including the NULL
   terminator.

Notes for Fortran

   All MPI routines in Fortran (except for MPI_WTIME and MPI_WTICK) have
   an additional argument ierr at the end of the argument list. ierr is an
   integer and has the same meaning as the return value of the routine in
   C. In Fortran, MPI routines are subroutines, and are invoked with the
   call statement.

   All MPI objects (e.g., MPI_Datatype, MPI_Comm) are of type INTEGER in
   Fortran.

Errors

   All MPI routines (except MPI_Wtime and MPI_Wtick) return an error
   value; C routines as the value of the function and Fortran routines in
   the last argument. Before the value is returned, the current MPI error
   handler is called. By default, this error handler aborts the MPI job.
   The error handler may be changed with MPI_Comm_set_errhandler (for
   communicators), MPI_File_set_errhandler (for files), and
   MPI_Win_set_errhandler (for RMA windows). The MPI-1 routine
   MPI_Errhandler_set may be used but its use is deprecated. The
   predefined error handler MPI_ERRORS_RETURN may be used to cause error
   values to be returned. Note that MPI does not guarentee that an MPI
   program can continue past an error; however, MPI implementations will
   attempt to continue whenever possible.

   MPI_SUCCESS
          No error; MPI routine completed successfully.

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <string.h>

/* Create an array with all of the MPI names in it */
typedef struct mpi_names_t { MPI_Datatype dtype; const char *name; }
mpi_names_t;

/* The MPI standard specifies that the names must be the MPI names, not
the related language names (e.g., MPI_CHAR, not char) */
static mpi_names_t mpi_names[] = {
    { MPI_CHAR, "MPI_CHAR" },
    { MPI_SIGNED_CHAR, "MPI_SIGNED_CHAR" },
    { MPI_UNSIGNED_CHAR, "MPI_UNSIGNED_CHAR" },
    { MPI_BYTE, "MPI_BYTE" },
    { MPI_WCHAR, "MPI_WCHAR" },
    { MPI_SHORT, "MPI_SHORT" },
    { MPI_UNSIGNED_SHORT, "MPI_UNSIGNED_SHORT" },
    { MPI_INT, "MPI_INT" },
    { MPI_UNSIGNED, "MPI_UNSIGNED" },
    { MPI_LONG, "MPI_LONG" },
    { MPI_UNSIGNED_LONG, "MPI_UNSIGNED_LONG" },
    { MPI_FLOAT, "MPI_FLOAT" },
    { MPI_DOUBLE, "MPI_DOUBLE" },
    { MPI_LONG_DOUBLE, "MPI_LONG_DOUBLE" },
    { MPI_LONG_LONG, "MPI_LONG_LONG" },
    { MPI_UNSIGNED_LONG_LONG, "MPI_UNSIGNED_LONG_LONG" },
    { MPI_PACKED, "MPI_PACKED" },
    { MPI_LB, "MPI_LB" },
    { MPI_UB, "MPI_UB" },
    { MPI_FLOAT_INT, "MPI_FLOAT_INT" },
    { MPI_DOUBLE_INT, "MPI_DOUBLE_INT" },
    { MPI_LONG_INT, "MPI_LONG_INT" },
    { MPI_SHORT_INT, "MPI_SHORT_INT" },
    { MPI_2INT, "MPI_2INT" },
    { MPI_LONG_DOUBLE_INT, "MPI_LONG_DOUBLE_INT" },
    { MPI_COMPLEX, "MPI_COMPLEX" },
    { MPI_DOUBLE_COMPLEX, "MPI_DOUBLE_COMPLEX" },
    { MPI_LOGICAL, "MPI_LOGICAL" },
    { MPI_REAL, "MPI_REAL" },
    { MPI_DOUBLE_PRECISION, "MPI_DOUBLE_PRECISION" },
    { MPI_INTEGER, "MPI_INTEGER" },
    { MPI_2INTEGER, "MPI_2INTEGER" },
    { MPI_2COMPLEX, "MPI_2COMPLEX" },
    { MPI_2DOUBLE_COMPLEX, "MPI_2DOUBLE_COMPLEX" },
    { MPI_2REAL, "MPI_2REAL" },
    { MPI_2DOUBLE_PRECISION, "MPI_2DOUBLE_PRECISION" },
    { MPI_CHARACTER, "MPI_CHARACTER" },
    { MPI_REAL4, "MPI_REAL4" },
    { MPI_REAL8, "MPI_REAL8" },
    { MPI_REAL16, "MPI_REAL16" },
    { MPI_COMPLEX8, "MPI_COMPLEX8" },
    { MPI_COMPLEX16, "MPI_COMPLEX16" },
    { MPI_COMPLEX32, "MPI_COMPLEX32" },
    { MPI_INTEGER1, "MPI_INTEGER1" },
    { MPI_INTEGER2, "MPI_INTEGER2" },
    { MPI_INTEGER4, "MPI_INTEGER4" },
    { MPI_INTEGER8, "MPI_INTEGER8" },
    //{ MPI_INTEGER16, "MPI_INTEGER16" },   // won't compile with this one
    { 0, (char *)0 }, /* Sentinal used to indicate the last element */
};

int main( int argc, char **argv )
{
    char name[MPI_MAX_OBJECT_NAME];
    int namelen, i;
    int errs = 0;

    MPI_Init(0,0);
    /* Sample some datatypes */
    /* See 8.4, "Naming Objects" in MPI-2. The default name is the same as the datatype name */
    MPI_Type_get_name( MPI_DOUBLE, name, &namelen );
    if (strncmp( name, "MPI_DOUBLE", MPI_MAX_OBJECT_NAME )) {
        errs++;
        fprintf( stderr, "Expected MPI_DOUBLE but got :%s:\n", name);
	fflush(stderr);
    }
    
    MPI_Type_get_name( MPI_INT, name, &namelen );
    if (strncmp( name, "MPI_INT", MPI_MAX_OBJECT_NAME )) {
        errs++;
        fprintf( stderr, "Expected MPI_INT but got :%s:\n", name);
	fflush(stderr);
    }
    
    /* Now we try them ALL */
    for (i=0; mpi_names[i].name != 0; i++) {
        /* The size-specific types may be MPI_DATATYPE_NULL */
        if (mpi_names[i].dtype == MPI_DATATYPE_NULL) continue;
        name[0] = 0;
        MPI_Type_get_name( mpi_names[i].dtype, name, &namelen );
        if (strncmp( name, mpi_names[i].name, namelen )) {
            errs++;
            fprintf( stderr, "Expected %s but got %s\n", mpi_names[i].name, name );
	    fflush(stderr);
        }
    }
    
    /* Try resetting the name */
    MPI_Type_set_name( MPI_INT, "int" );
    name[0] = 0;
    MPI_Type_get_name( MPI_INT, name, &namelen );
    if (strncmp( name, "int", MPI_MAX_OBJECT_NAME )) {
        errs++;
        fprintf( stderr, "Expected int but got :%s:\n", name);
	fflush(stderr);
    }

    MPI_Finalize();
    return errs;
}

