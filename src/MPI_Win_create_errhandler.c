/* 
MPI_Win_create_errhandler

   Create an error handler for use with MPI window objects
int MPI_Win_create_errhandler(
  MPI_Win_errhandler_fn *function,
  MPI_Errhandler *errhandler
);

Parameters

   function
          [in] user defined error handling procedure (function)

   errhandler
          [out] MPI error handler (handle)

Remarks

   The user routine should be, in C, a function of type
   MPI_Win_errhandler_fn, which is defined as

   typedef void MPI_Win_errhandler_fn(MPI_Win *, int *, ...);

   The first argument is the window in use, the second is the error code
   to be returned.

   In Fortran, the user routine should be of the form:

   SUBROUTINE WIN_ERRHANDLER_FN(WIN, ERROR_CODE, ... )
   INTEGER WIN, ERROR_CODE

   In C++, the user routine should be of the form:

   typedef void MPI::Win::Errhandler_fn(MPI::Win &, int *, ... );

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

/* Test the routines to control error handlers on windows */
static int calls = 0;
static int errs = 0;
static MPI_Win mywin;
static int expected_err_class = MPI_ERR_OTHER;

void weh( MPI_Win *win, int *err, ... )
{
    int errclass;
    MPI_Error_class( *err, &errclass );
    if (errclass != expected_err_class) {
        errs++;
        printf( "Unexpected error code (class = %d)\n", errclass );
	fflush(stdout);
    }
    if (*win != mywin) {
        errs++;
        printf( "Unexpected window (got %x expected %x)\n",
		(int)*win, (int)mywin );
	fflush(stdout);
    }
    calls++;
    return;
}

int main( int argc, char *argv[] )
{
    int err;
    int buf[2];
    MPI_Win       win;
    MPI_Comm      comm;
    MPI_Errhandler newerr, olderr;
    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    MPI_Win_create_errhandler( weh, &newerr );
    MPI_Win_create( buf, 2*sizeof(int), sizeof(int), MPI_INFO_NULL, comm, &win );

    mywin = win;
    MPI_Win_get_errhandler( win, &olderr );
    if (olderr != MPI_ERRORS_ARE_FATAL) {
        errs++;
        printf( "Expected errors are fatal\n" );
	fflush(stdout);
    }
    MPI_Win_set_errhandler( win, newerr );

    expected_err_class = MPI_ERR_RANK;
    err = MPI_Put( buf, 1, MPI_INT, -5, 0, 1, MPI_INT, win );
    if (calls != 1) {
        errs ++;
        printf( "newerr not called\n" );
	fflush(stdout);
        calls = 1;
    }
    expected_err_class = MPI_ERR_OTHER;
    MPI_Win_call_errhandler( win, MPI_ERR_OTHER );
    if (calls != 2) {
        errs ++;
        printf( "newerr not called (2)\n" );
	fflush(stdout);
    }
    
    MPI_Win_free( &win );
    MPI_Errhandler_free( &newerr );

    MPI_Finalize();
    return errs;
}
