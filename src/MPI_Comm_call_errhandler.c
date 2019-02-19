/* 
MPI_Comm_call_errhandler

   Call the error handler installed on a communicator
int MPI_Comm_call_errhandler(
  MPI_Comm comm,
  int errorcode
);

Parameters

   comm
          [in] communicator with error handler (handle)

   errorcode
          [in] error code (integer)

Remarks

   This function invokes the error handler assigned to the communicator
   with the error code supplied. This function returns MPI_SUCCESS in C
   and C++ and the same value in IERROR if the error handler was
   successfully called (assuming the process is not aborted and the error
   handler returns).

   Advice to users.

   Users should note that the default error handler is
   MPI_ERRORS_ARE_FATAL. Thus, calling MPI_COMM_CALL_ERRHANDLER will abort
   the comm processes if the default error handler has not been changed
   for this communicator or on the parent before the communicator was
   created.

Thread and Interrupt Safety

   This routine is thread and interrupt safe only if no MPI routine that
   updates or frees the same MPI object may be called concurrently with
   this routine.

   The MPI standard defined a thread-safe interface but this does not mean
   that all routines may be called without any thread locks. For example,
   two threads must not attempt to change the contents of the same
   MPI_Info object concurrently. The user is responsible in this case for
   using some mechanism, such as thread locks, to ensure that only one
   thread at a time makes use of this routine.

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

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
static int calls = 0;
static int errs = 0;
static MPI_Comm mycomm;
void eh( MPI_Comm *comm, int *err, ... )
{
    if (*err != MPI_ERR_OTHER) {
        errs++;
        printf( "Unexpected error code\n" );fflush(stdout);
    }
    if (*comm != mycomm) {
        errs++;
        printf( "Unexpected communicator\n" );fflush(stdout);
    }
    calls++;
    return;
}
int main( int argc, char *argv[] )
{
    MPI_Comm comm;
    MPI_Errhandler newerr;
    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    mycomm = comm;
    MPI_Comm_create_errhandler( eh, &newerr );
    MPI_Comm_set_errhandler( comm, newerr );
    MPI_Comm_call_errhandler( comm, MPI_ERR_OTHER );
    MPI_Errhandler_free( &newerr );
    if (calls != 1) {
        errs++;
        printf( "Error handler not called\n" );fflush(stdout);
    }
    MPI_Finalize();
    return 0;
}

