/* 
MPI_Cart_map

   Maps process to Cartesian topology information
int MPI_Cart_map(
  MPI_Comm comm_old,
  int ndims,
  int *dims,
  int *periods,
  int *newrank
);

Parameters

   comm
          [in] input communicator (handle)

   ndims
          [in] number of dimensions of Cartesian structure (integer)

   dims
          [in] integer array of size ndims specifying the number of
          processes in each coordinate direction

   periods
          [in] logical array of size ndims specifying the periodicity
          specification in each coordinate direction

   newrank
          [out] reordered rank of the calling process; MPI_UNDEFINED if
          calling process does not belong to grid (integer)

Remarks

   MPI_CART_MAP computes an "optimal" placement for the calling process on
   the physical machine.  In general this function will not be called by
   the user directly, unless he or she is creating additional virtual
   topology capability other than that provided by MPI.

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

   MPI_ERR_DIMS
          Invalid dimension argument. A dimension argument is null or its
          length is less than or equal to zero.

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
    int dims[2];
    int periods[2];
    int size, rank, newrank;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    /* This defines a cartision grid with a single point */
    periods[0] = 1;
    dims[0] = 1;
    MPI_Cart_map( MPI_COMM_WORLD, 1, dims, periods, &newrank );
    if (rank > 0) {
        if (newrank != MPI_UNDEFINED) {
            errs++;
            printf( "rank outside of input communicator not UNDEFINED\n" );
	    fflush(stdout);
        }
    }
    else {
        if (rank != newrank) {
            errs++;
            printf( "Newrank not defined and should be 0\n");
	    fflush(stdout);
        }
    }
    MPI_Finalize();
    return 0;
}

