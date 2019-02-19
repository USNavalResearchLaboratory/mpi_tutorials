/* 
MPI_Cart_create

   Makes a new communicator to which topology information has been
   attached
int MPI_Cart_create(
  MPI_Comm comm_old,
  int ndims,
  int *dims,
  int *periods,
  int reorder,
  MPI_Comm *comm_cart
);

Parameters

   comm_old
          [in] input communicator (handle)

   ndims
          [in] number of dimensions of cartesian grid (integer)

   dims
          [in] integer array of size ndims specifying the number of
          processes in each dimension

   periods
          [in] logical array of size ndims specifying whether the grid is
          periodic (true) or not (false) in each dimension

   reorder
          [in] ranking may be reordered (true) or not (false) (logical)

   comm_cart
          [out] communicator with new cartesian topology (handle)

Algorithm

   We ignore reorder info currently.

Remarks

   MPI_CART_CREATE returns a handle to a new communicator to which the
   cartesian topology information is attached. If reorder = false then the
   rank of each process in the new group is identical to its rank in the
   old group. Otherwise, the function may reorder the processes (possibly
   so as to choose a good embedding of the virtual topology onto the
   physical machine). If the total size of the cartesian grid is smaller
   than the size of the group of comm, then some processes are returned
   MPI_COMM_NULL, in analogy to MPI_COMM_SPLIT. The call is erroneous if
   it specifies a grid that is larger than the group size.

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

   MPI_ERR_DIMS
          Invalid dimension argument. A dimension argument is null or its
          length is less than or equal to zero.

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include<mpi.h>
#include<stdio.h>
 /* A two-dimensional torus of 12 processes in a 4x3 grid */
int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Comm comm;
    int dim[2], period[2], reorder;
    int coord[2], id;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 12)
    {
        printf("Please run with 12 processes.\n");fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    dim[0]=4; dim[1]=3;
    period[0]=1; period[1]=0;
    reorder=1;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dim, period, reorder, &comm);
    if (rank == 5)
    {
        MPI_Cart_coords(comm, rank, 2, coord);
        printf("Rank %d coordinates are %d %d\n", rank, coord[0],
coord[1]);fflush(stdout);
    }
    if(rank==0)
    {
        coord[0]=3; coord[1]=1;
        MPI_Cart_rank(comm, coord, &id);
        printf("The processor at position (%d, %d) has rank %d\n",
coord[0], coord[1], id);fflush(stdout);
    }
    MPI_Finalize();
    return 0;
}

