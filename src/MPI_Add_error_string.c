/* 
MPI_Add_error_string

   Associates an error string with an MPI error code or class
int MPI_Add_error_string(
  int errorcode,
  char *string
);

int MPI_Add_error_string(
  int errorcode,
  wchar_t *string
);

Parameters

   errorcode
          [in] error code or class (integer)

   string
          [in] text corresponding to errorcode (string)

Remarks

   Associates an error string with an error code or class. The string must
   be no more than MPI_MAX_ERROR_STRING characters long. The length of the
   string is as defined in the calling language. The length of the string
   does not include the null terminator in C or C++. Trailing blanks will
   be stripped in Fortran. Calling MPI_ADD_ERROR_STRING for an errorcode
   that already has a string will replace the old string with the new
   string.  Note that the string is const even though the MPI standard
   does not specify it that way.

   According to the MPI-2 standard, it is erroneous to call
   MPI_Add_error_string for an error code or class with a value less than
   or equal to MPI_ERR_LASTCODE. Thus, you cannot replace the predefined
   error messages with this routine.

   If MPI_ERROR_STRING is called when no string has been set, it will
   return a empty string (all spaces in Fortran, "" in C and C++).

Thread and Interrupt Safety

   This routine is thread-safe. This means that this routine may be safely
   used by multiple threads without the need for any user-provided thread
   locks. However, the routine is not interrupt safe. Typically, this is
   due to the use of memory allocation routines such as malloc or other
   non-MPICH runtime routines that are themselves not interrupt-safe.

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Create NCLASSES new classes, each with 5 codes (160 total) */
#define NCLASSES 32
#define NCODES 5
int main( int argc, char *argv[] )
{
    int errs = 0;
    char string[MPI_MAX_ERROR_STRING], outstring[MPI_MAX_ERROR_STRING];
    int newclass[NCLASSES], newcode[NCLASSES][NCODES];
    int i, j, slen, outclass;
    MPI_Init( &argc, &argv );
    /* Initialize the new codes */
    for (i=0; i<NCLASSES; i++) {
        MPI_Add_error_class( &newclass[i] );
        for (j=0; j<NCODES; j++) {
            MPI_Add_error_code( newclass[i], &newcode[i][j] );
            sprintf( string, "code for class %d code %d\n", i, j );
            MPI_Add_error_string( newcode[i][j], string );
        }
    }
    /* check the values */
    for (i=0; i<NCLASSES; i++) {
        MPI_Error_class( newclass[i], &outclass );
        if (outclass != newclass[i]) {
            errs++;
            printf( "Error class %d is not a valid error code %x %x\n",
i,
                        outclass, newclass[i]);fflush(stdout);
        }
        for (j=0; j<NCODES; j++) {
            MPI_Error_class( newcode[i][j], &outclass );
            if (outclass != newclass[i]) {
                errs++;
                printf( "Class of code for %d is not correct %x %x\n",
j,
                            outclass, newclass[i] );fflush(stdout);
            }
            MPI_Error_string( newcode[i][j], outstring, &slen );
            sprintf( string, "code for class %d code %d\n", i, j );
            if (strcmp( outstring, string )) {
                errs++;
                printf( "Error string is :%s: but should be :%s:\n",
                            outstring, string );fflush(stdout);
            }
        }
    }
    MPI_Finalize();
    return 0;
}

