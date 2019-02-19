/* 
MPI_Errhandler_free

   Frees an MPI-style errorhandler
int MPI_Errhandler_free(
  MPI_Errhandler *errhandler
);

Parameters

   errhandler
          [in/out] MPI error handler (handle). Set to MPI_ERRHANDLER_NULL
          on exit.

Remarks

   Marks the error handler associated with errhandler for deallocation and
   sets errhandler to MPI_ERRHANDLER_NULL. The error handler will be
   deallocated after all communicators associated with it have been
   deallocated.

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

