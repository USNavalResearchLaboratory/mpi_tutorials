/* 
MPI_Error_string

   Return a string for a given error code
int MPI_Error_string(
  int errorcode,
  char *string,
  int *resultlen
);

int MPI_Error_string(
  int errorcode,
  wchar_t *string,
  int *resultlen
);

Parameters

   errorcode
          [in] Error code returned by an MPI routine or an MPI error class

   string
          [out] Text that corresponds to the errorcode

   resultlen
          [out] Length of string

   Notes: Error codes are the values return by MPI routines (in C) or in
   the ierr argument (in Fortran). These can be converted into error
   classes with the routine MPI_Error_class.

Remarks

   Returns the error string associated with an error code or class. The
   argument string must represent storage that is at least
   MPI_MAX_ERROR_STRING characters long.

   The number of characters actually written is returned in the output
   argument, resultlen.

   Rationale.

   The form of this function was chosen to make the Fortran and C bindings
   similar. A version that returns a pointer to a string has two
   difficulties. First, the return string must be statically allocated and
   different for each error message (allowing the pointers returned by
   successive calls to MPI_ERROR_STRING to point to the correct message).
   Second, in Fortran, a function declared as returning CHARACTER*(*) can
   not be referenced in, for example, a PRINT statement.

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

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include "mpi.h"
#include <stdio.h>
int main(int argc, char *argv[])
{
    int rank, nprocs, error, eclass, len;
    char estring[MPI_MAX_ERROR_STRING];
    MPI_Init(&argc,&argv);
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
    /* Make an invalid call to generate an error */
    error = MPI_Bcast(NULL, 0, MPI_INT, -1, MPI_COMM_WORLD);
    MPI_Error_class(error, &eclass);
    MPI_Error_string(error, estring, &len);
    printf("Error %d: %s\n", eclass, estring);fflush(stdout);
    MPI_Finalize();
    return 0;
}

