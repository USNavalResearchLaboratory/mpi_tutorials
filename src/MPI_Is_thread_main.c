/* 
MPI_Is_thread_main

   Returns a flag indicating whether this thread called MPI_Init or
   MPI_Init_thread
int MPI_Is_thread_main(
  int *flag
);

Parameters

   flag
          [out] Flag is true if MPI_Init or MPI_Init_thread has been
          called by this thread and false otherwise. (logical)

Remarks

   This function can be called by a thread to find out whether it is the
   main thread (the thread that called MPI_INIT or MPI_INIT_THREAD).

   Advice to users.

   It is possible to spawn threads before MPI is initialized, but no MPI
   call other than MPI_INITIALIZED should be executed by these threads,
   until MPI_INIT_THREAD is invoked by one thread (which, thereby, becomes
   the main thread). In particular, it is possible to enter the MPI
   execution with a multi-threaded process.

   The level of thread support provided is a global property of the MPI
   process that can be specified only once, when MPI is initialized on
   that process (or before). Portable third party libraries have to be
   written so as to accommodate any provided level of thread support.
   Otherwise, their usage will be restricted to specific level(s) of
   thread support. If such a library can run only with specific level(s)
   of thread support, e.g., only with MPI_THREAD_MULTIPLE, then
   MPI_QUERY_THREAD can be used to check whether the user initialized MPI
   to the correct level of thread support and, if not, raise an exception.

Thread and Interrupt Safety

   This routine is both thread- and interrupt-safe. This means that this
   routine may safely be used by multiple threads and from within a signal
   handler.

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

int main( int argc, char *argv[] )
{
    int errs = 0;
    int provided, flag, claimed;

    MPI_Init_thread( 0, 0, MPI_THREAD_MULTIPLE, &provided );

    MPI_Is_thread_main( &flag );
    if (!flag) {
        errs++;
        printf( "This thread called init_thread but Is_thread_main gave false\n" );
	fflush(stdout);
    }
    
    MPI_Query_thread( &claimed );
    if (claimed != provided) {
        errs++;
        printf( "Query thread gave thread level %d but Init_thread gave %d\n",
		claimed, provided );
	fflush(stdout);
    }

    MPI_Finalize();
    return errs;
}

