/* 
MPI_Dims_create

   Creates a division of processors in a cartesian grid
int MPI_Dims_create(
  int nnodes,
  int ndims,
  int *dims
);

Parameters

   nnodes
          [in] number of nodes in a grid (integer)

   ndims
          [in] number of cartesian dimensions (integer)

   dims
          [in/out] integer array of size ndims specifying the number of
          nodes in each dimension. A value of 0 indicates that
          MPI_Dims_create should fill in a suitable value.

Remarks

   For cartesian topologies, the function MPI_DIMS_CREATE helps the user
   select a balanced distribution of processes per coordinate direction,
   depending on the number of processes in the group to be balanced and
   optional constraints that can be specified by the user. One use is to
   partition all the processes (the size of MPI_COMM_WORLD's group) into
   an n-dimensional topology.

   The entries in the array dims are set to describe a cartesian grid with
   ndims dimensions and a total of nnodes nodes. The dimensions are set to
   be as close to each other as possible, using an appropriate
   divisibility algorithm. The caller may further constrain the operation
   of this routine by specifying elements of array dims. If dims[i] is set
   to a positive number, the routine will not modify the number of nodes
   in dimension i; only those entries where dims[i] = 0 are modified by
   the call.

   Negative input values of dims[i] are erroneous. An error will occur if
   nnodes is not a multiple of [img169.gif] .

   For dims[i] set by the call, dims[i] will be ordered in non-increasing
   order. Array dims is suitable for use as input to routine
   MPI_CART_CREATE. MPI_DIMS_CREATE is local.

   [img170.gif]

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

int prodof( int, const int[] );

/* Test edge cases of Dims_create */
int prodof( int ndims, const int dims[] )
{
    int i, prod=1;
    for (i=0; i<ndims; i++)
        prod *= dims[i];
    return prod;
}

int main( int argc, char *argv[] )
{
    int errs = 0;
    int dims[4], nnodes;
    MPI_Init( &argc, &argv );
    /* 2 dimensional tests */
    for (nnodes=1; nnodes <= 32; nnodes = nnodes * 2) {
        dims[0] = 0;
        dims[1] = nnodes;
        MPI_Dims_create( nnodes, 2, dims );
        if (prodof(2, dims) != nnodes) {
            errs++;
            printf( "Dims_create returned the wrong decomposition. " );
            printf( "Is [%d x %d], should be 1 x %d\n", dims[0], dims[1], nnodes );
            fflush(stdout);
        }
        /* Try calling Dims_create with nothing to do (all dimensions specified) */
        dims[0] = 1;
        dims[1] = nnodes;
        MPI_Dims_create( nnodes, 2, dims );
        if (prodof(2, dims) != nnodes) {
            errs++;
            printf( "Dims_create returned the wrong decomposition (all given). " );
            printf( "Is [%d x %d], should be 1 x %d\n", dims[0], dims[1], nnodes );
            fflush(stdout);
        }
    }
    /* 4 dimensional tests */
    for (nnodes=4; nnodes <= 32; nnodes = nnodes * 2) {
        dims[0] = 0;
        dims[1] = nnodes/2;
        dims[2] = 0;
        dims[3] = 2;
        MPI_Dims_create( nnodes, 4, dims );
        if (prodof(4, dims) != nnodes) {
            errs++;
            printf( "Dims_create returned the wrong decomposition. " );
            printf( "Is [%d x %d x %d x %d], should be 1 x %d x 1 x 2\n",
		    dims[0], dims[1], dims[2], dims[3], nnodes/2 );
            fflush(stdout);
        }
        /* Try calling Dims_create with nothing to do (all dimensions specified) */
        dims[0] = 1;
        dims[1] = nnodes/2;
        dims[2] = 1;
        dims[3] = 2;
        MPI_Dims_create( nnodes, 2, dims );
        if (prodof(4, dims) != nnodes) {
            errs++;
            printf( "Dims_create returned the wrong decomposition (all given). " );
            printf( "Is [%d x %d x %d x %d], should be 1 x %d x 1 x 2\n",
		    dims[0], dims[1], dims[2], dims[3], nnodes/2 );
            fflush(stdout);
        }
    }
    
    MPI_Finalize();
    return errs;
}
