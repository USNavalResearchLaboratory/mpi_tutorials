/* 
MPI_Comm_rank

   Determines the rank of the calling process in the communicator
int MPI_Comm_rank(
  MPI_Comm comm,
  int *rank
);

Parameters

   comm
          [in] communicator (handle)

   rank
          [out] rank of the calling process in the group of comm (integer)

Remarks

   MPI_COMM_RANK indicates the rank of the process that calls it in the
   range from [img147.gif] size-1, where size is the return value of
   MPI_COMM_SIZE.

   Rationale.

   This function is equivalent to accessing the communicator's group with
   MPI_COMM_GROUP (see above), computing the rank using MPI_GROUP_RANK,
   and then freeing the temporary group via MPI_GROUP_FREE. However, this
   function is so commonly used, that this shortcut was introduced. ( End
   of rationale.)

   Advice to users.

   This function gives the rank of the process in the particular
   communicator's group. It is useful, as noted above, in conjunction with
   MPI_COMM_SIZE.

   Many programs will be written with the master-slave model, where one
   process (such as the rank-zero process) will play a supervisory role,
   and the other processes will serve as compute nodes. In this framework,
   the two preceding calls are useful for determining the roles of the
   various processes of a communicator.

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

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include "mpi.h"
#include <stdio.h>
int main(int argc, char *argv[])
{
    int rank, nprocs;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    printf("Hello, world.  I am %d of %d\n", rank,
nprocs);fflush(stdout);
    MPI_Finalize();
    return 0;
}

