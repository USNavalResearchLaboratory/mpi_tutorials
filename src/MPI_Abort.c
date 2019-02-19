/* 
MPI_Abort

   Terminates MPI execution environment
int MPI_Abort(
  MPI_Comm comm,
  int errorcode
);

Parameters

   comm
          [in] communicator of tasks to abort

   errorcode
          [in] error code to return to invoking environment

Remarks

   Terminates all MPI processes associated with the communicator comm;
   currently this function terminates all processes.  This function does
   not return so any return value is an error.  The process calling
   MPI_Abort must be a member of the communicator passed in.  If the
   communicator is invalid or the process is not a member of the
   communicator, MPI_Abort is called on MPI_COMM_WORLD.

Standard

   This routine makes a "best attempt" to abort all tasks in the group of
   comm. This function does not require that the invoking environment take
   any action with the error code. However, a Unix or POSIX environment
   should handle this as a return errorcode from the main program or an
   abort(errorcode).

   MPI implementations are required to define the behavior of MPI_ABORT at
   least for a comm of MPI_COMM_WORLD. MPI implementations may ignore the
   comm argument and act as if the comm was MPI_COMM_WORLD.

Thread and Interrupt Safety

   The user is responsible for ensuring that multiple threads do not try
   to update the same MPI object from different threads. This routine
   should not be used from within a signal handler.

   The MPI standard defined a thread-safe interface but this does not mean
   that all routines may be called without any thread locks. For example,
   two threads must not attempt to change the contents of the same
   MPI_Info object concurrently. The user is responsible in this case for
   using some mechanism, such as thread locks, to ensure that only one
   thread at a time makes use of this routine. Because the MPI_Abort
   routine is intended to ensure that an MPI process exits (and possibly
   an entire job), it cannot wait for a thread to release a lock or other
   mechanism for atomic access.

Notes for Fortran

   All MPI routines in Fortran (except for MPI_WTIME and MPI_WTICK) have
   an additional argument ierr at the end of the argument list. ierr is an
   integer and has the same meaning as the return value of the routine in
   C. In Fortran, MPI routines are subroutines, and are invoked with the
   call statement.

   All MPI objects (e.g., MPI_Datatype, MPI_Comm) are of type INTEGER in
   Fortran.

Errors

   This function does not return so any return value is erroneous, even
   MPI_SUCCESS.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include <mpi.h>

int main(int argc, char *argv[])
{
    MPI_Init(NULL, NULL);

    MPI_Abort(MPI_COMM_WORLD, 911);

    /* No further code will execute */
    MPI_Finalize();

    return 0;
}

