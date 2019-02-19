/* 
MPI_Add_error_code

   Add and MPI error code to an MPI error class
int MPI_Add_error_code(
  int errorclass,
  int *errorcode
);

Parameters

   errorclass
          [in] Error class to add an error code.

   errorcode
          [out] New error code for this error class.

Remarks

   Creates new error code associated with errorclass and returns its value
   in errorcode.

   Rationale: To avoid conflicts with existing error codes and classes,
   the value of the new error code is set by the implementation and not by
   the user.

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

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

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

