/* 
MPI_Comm_dup

   Duplicates an existing communicator with all its cached information
int MPI_Comm_dup(
  MPI_Comm comm,
  MPI_Comm *newcomm
);

Parameters

   comm
          [in] Communicator to be duplicated (handle)

   newcomm
          [out] A new communicator over the same group as comm but with a
          new context. See notes. (handle)

Remarks

   This routine is used to create a new communicator that has a new
   communication context but contains the same group of processes as the
   input communicator. Since all MPI communication is performed within a
   communicator (specifies as the group of processes plus the context),
   this routine provides an effective way to create a private communicator
   for use by a software module or library. In particular, no library
   routine should use MPI_COMM_WORLD as the communicator; instead, a
   duplicate of a user-specified communicator should always be used. For
   more information, see Using MPI, 2nd edition.

   Because this routine essentially produces a copy of a communicator, it
   also copies any attributes that have been defined on the input
   communicator, using the attribute copy function specified by the
   copy_function argument to MPI_Keyval_create. This is particularly
   useful for (a) attributes that describe some property of the group
   associated with the communicator, such as its interconnection topology
   and (b) communicators that are given back to the user; the attibutes in
   this case can track subsequent MPI_Comm_dup operations on this
   communicator.

   Advice to users.

   This operation is used to provide a parallel library call with a
   duplicate communication space that has the same properties as the
   original communicator. This includes any attributes (see below), and
   topologies (see chapter Process Topologies ). This call is valid even
   if there are pending point-to-point communications involving the
   communicator comm. A typical call might involve a MPI_COMM_DUP at the
   beginning of the parallel call, and an MPI_COMM_FREE of that duplicated
   communicator at the end of the call. Other models of communicator
   management are also possible.

   This call applies to both intra- and inter-communicators.

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

See Also

   MPI_Comm_free, MPI_Keyval_create, MPI_Attr_put, MPI_Attr_delete,
   MPI_Comm_create_keyval, MPI_Comm_set_attr, MPI_Comm_delete_attr

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

