/* 
MPI_Cart_sub

   Partitions a communicator into subgroups which form lower-dimensional
   cartesian subgrids
int MPI_Cart_sub(
  MPI_Comm comm,
  int *remain_dims,
  MPI_Comm *newcomm
);

Parameters

   comm
          [in] communicator with cartesian structure (handle)

   remain_dims
          [in] the ith entry of remain_dims specifies whether the ith
          dimension is kept in the subgrid (true) or is dropped (false)
          (logical vector)

   newcomm
          [out] communicator containing the subgrid that includes the
          calling process (handle)

Remarks

   If a cartesian topology has been created with MPI_CART_CREATE, the
   function
   MPI_CART_SUB can be used to partition the communicator group into
   subgroups that form lower-dimensional cartesian subgrids, and to build
   for each subgroup a communicator with the associated subgrid cartesian
   topology. (This function is closely related to MPI_COMM_SPLIT.)

   Example

   Assume that MPI_CART_CREATE(..., comm) has defined a [img188.gif] grid.
   Let remain_dims = (true, false, true). Then a call to,

MPI_CART_SUB(comm, remain_dims, comm_new),

   will create three communicators each with eight processes in a
   [img189.gif] cartesian topology. If remain_dims = (false, false, true)
   then the call to MPI_CART_SUB(comm, remain_dims, comm_new) will create
   six non-overlapping communicators, each with four processes, in a
   one-dimensional cartesian topology.

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

   MPI_ERR_TOPOLOGY
          Invalid topology. Either there is no topology associated with
          this communicator, or it is not the correct type (e.g., MPI_CART
          when expecting MPI_GRAPH).

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
int main( int argc, char *argv[] )
{
    int errs = 0;
    int size, dims[2], periods[2], remain[2];
    int result;
    MPI_Comm comm, newcomm;
    MPI_Init( &argc, &argv );
    /* First, create a 1-dim cartesian communicator */
    periods[0] = 0;
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    dims[0] = size;
    MPI_Cart_create( MPI_COMM_WORLD, 1, dims, periods, 0, &comm );
    /* Now, extract a communicator with no dimensions */
    remain[0] = 0;
    MPI_Cart_sub( comm, remain, &newcomm );
    /* This should be congruent to MPI_COMM_SELF */
    MPI_Comm_compare( MPI_COMM_SELF, newcomm, &result );
    if (result != MPI_CONGRUENT) {
        errs++;
        printf( "cart sub to size 0 did not give self\n"
);fflush(stdout);
    }
    /* Free the new communicator */
    MPI_Comm_free( &newcomm );
    MPI_Comm_free( &comm );
    MPI_Finalize();
    return 0;
}

