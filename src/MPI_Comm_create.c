/* 
MPI_Comm_create

   Creates a new communicator
int MPI_Comm_create(
  MPI_Comm comm,
  MPI_Group group,
  MPI_Comm *newcomm
);

Parameters

   comm
          [in] communicator (handle)

   group
          [in] group, which is a subset of the group of comm (handle)

   newcomm
          [out] new communicator (handle)

Remarks

   This function creates a new communicator newcomm with communication
   group defined by group and a new context. No cached information
   propagates from comm to newcomm. The function returns MPI_COMM_NULL to
   processes that are not in group. The call is erroneous if not all group
   arguments have the same value, or if group is not a subset of the group
   associated with comm. Note that the call is to be executed by all
   processes in comm, even if they do not belong to the new group. This
   call applies only to intra-communicators.

   Rationale.

   The requirement that the entire group of comm participate in the call
   stems from the following considerations:
     * It allows the implementation to layer MPI_COMM_CREATE on top of
       regular collective communications.
     * It provides additional safety, in particular in the case where
       partially overlapping groups are used to create new communicators.
     * It permits implementations sometimes to avoid communication related
       to context creation.

   Advice to users.

   MPI_COMM_CREATE provides a means to subset a group of processes for the
   purpose of separate MIMD computation, with separate communication
   space. newcomm, which emerges from MPI_COMM_CREATE can be used in
   subsequent calls to MPI_COMM_CREATE (or other communicator
   constructors) further to subdivide a computation into parallel
   sub-computations. A more general service is provided by MPI_COMM_SPLIT.

   If comm_in is an intercommunicator, then the output communicator is
   also an intercommunicator where the local group consists only of those
   processes contained in group. The group argument should only contain
   those processes in the local group of the input intercommunicator that
   are to be a part of comm_out. If either group does not specify at least
   one process in the local group of the intercommunicator, or if the
   calling process is not included in the group, MPI_COMM_NULL is
   returned.

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

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

   MPI_ERR_GROUP
          Null or invalid group passed to function.

See Also

   MPI_Comm_free

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include <mpi.h>
#include <stdio.h>
int main(int argc, char* argv[] )
{
    MPI_Comm dup_comm_world, world_comm;
    MPI_Group world_group;
    int world_rank, world_size, rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &world_rank );
    MPI_Comm_size( MPI_COMM_WORLD, &world_size );
    MPI_Comm_dup( MPI_COMM_WORLD, &dup_comm_world );
    /* Exercise Comm_create by creating an equivalent to dup_comm_world
(sans attributes) */
    MPI_Comm_group( dup_comm_world, &world_group );
    MPI_Comm_create( dup_comm_world, world_group, &world_comm );
    MPI_Comm_rank( world_comm, &rank );
    if (rank != world_rank) {
        printf( "incorrect rank in world comm: %d\n", rank
);fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 3001 );
    }
    MPI_Finalize();
    return 0;
}

