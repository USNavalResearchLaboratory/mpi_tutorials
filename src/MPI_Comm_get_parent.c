/* 
MPI_Comm_get_parent

   Return the parent communicator for this process
int MPI_Comm_get_parent(
  MPI_Comm *parent
);

Parameters

   parent
          [out] the parent communicator (handle)

Remarks

   If a process was started with MPI_Comm_spawn or
   MPI_Comm_spawn_multiple, MPI_Comm_get_parent returns the parent
   intercommunicator of the current process. This parent intercommunicator
   is created implicitly inside of MPI_Init and is the same
   intercommunicator returned by MPI_Comm_spawn in the parents.

   If the process was not spawned, MPI_Comm_get_parent returns
   MPI_COMM_NULL.

   After the parent communicator is freed or disconnected,
   MPI_Comm_get_parent returns MPI_COMM_NULL.

   Advice to users.

   MPI_COMM_GET_PARENT returns a handle to a single intercommunicator.
   Calling MPI_COMM_GET_PARENT a second time returns a handle to the same
   intercommunicator. Freeing the handle with MPI_COMM_DISCONNECT or
   MPI_COMM_FREE will cause other references to the intercommunicator to
   become invalid (dangling). Note that calling MPI_COMM_FREE on the
   parent communicator is not useful.
   Rationale.

   The desire of the Forum was to create a constant MPI_COMM_PARENT
   similar to MPI_COMM_WORLD. Unfortunately such a constant cannot be used
   (syntactically) as an argument to MPI_COMM_DISCONNECT, which is
   explicitly allowed.

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

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define NUM_SPAWNS 2
int main( int argc, char *argv[] )
{
    int np = NUM_SPAWNS;
    int errcodes[NUM_SPAWNS];
    MPI_Comm parentcomm, intercomm;
    MPI_Init( &argc, &argv );
    MPI_Comm_get_parent( &parentcomm );
    if (parentcomm == MPI_COMM_NULL)
    {
        /* Create 2 more processes - this example must be called
spawn_example.exe for this to work. */
        MPI_Comm_spawn( "spawn_example", MPI_ARGV_NULL, np,
MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm, errcodes );
        printf("I'm the parent.\n");
    }
    else
    {
        printf("I'm the spawned.\n");
    }
    fflush(stdout);
    MPI_Finalize();
    return 0;
}

