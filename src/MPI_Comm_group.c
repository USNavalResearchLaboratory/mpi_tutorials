/* 
MPI_Comm_group

   Accesses the group associated with given communicator
int MPI_Comm_group(
  MPI_Comm comm,
  MPI_Group *group
);

Parameters

   comm
          [in] Communicator (handle)

   group
          [out] Group in communicator (handle)

Remarks

   MPI_COMM_GROUP returns in group a handle to the group of comm.

   Because MPI specifies that null objects (e.g., MPI_COMM_NULL) are
   invalid as input to MPI routines unless otherwise specified, using
   MPI_COMM_NULL as input to this routine is an error.

   Group constructors are used to subset and superset existing groups.
   These constructors construct new groups from existing groups. These are
   local operations, and distinct groups may be defined on different
   processes; a process may also define a group that does not include
   itself. Consistent definitions are required when groups are used as
   arguments in communicator-building functions. MPI does not provide a
   mechanism to build a group from scratch, but only from other,
   previously defined groups. The base group, upon which all other groups
   are defined, is the group associated with the initial communicator
   MPI_COMM_WORLD (accessible through the function MPI_COMM_GROUP).

   Rationale.

   In what follows, there is no group duplication function analogous to
   MPI_COMM_DUP. There is no need for a group duplicator. A group, once
   created, can have several references to it by making copies of the
   handle. The following constructors address the need for subsets and
   supersets of existing groups.

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
        printf( "incorrect rank in world comm: %d\n", rank);
	fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 3001 );
    }
    
    MPI_Finalize();
    return 0;
}

