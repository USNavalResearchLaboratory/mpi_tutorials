/* 
MPI_Type_create_subarray

   Create a datatype for a subarray of a regular, multidimensional array
int MPI_Type_create_subarray(
  int ndims,
  int array_of_sizes[],
  int array_of_subsizes[],
  int array_of_starts[],
  int order,
  MPI_Datatype oldtype,
  MPI_Datatype *newtype
);

Parameters

   ndims
          [in] number of array dimensions (positive integer)

   array_of_sizes
          [in] number of elements of type oldtype in each dimension of the
          full array (array of positive integers)

   array_of_subsizes
          [in] number of elements of type oldtype in each dimension of the
          subarray (array of positive integers)

   array_of_starts
          [in] starting coordinates of the subarray in each dimension
          (array of nonnegative integers)

   order
          [in] array storage order flag (state)

   oldtype
          [in] array element datatype (handle)

   newtype
          [out] new datatype (handle)

Remarks

   The subarray type constructor creates an MPI datatype describing an
   n-dimensional subarray of an n-dimensional array. The subarray may be
   situated anywhere within the full array, and may be of any nonzero size
   up to the size of the larger array as long as it is confined within
   this array. This type constructor facilitates creating filetypes to
   access arrays distributed in blocks among processes to a single file
   that contains the global array.

   This type constructor can handle arrays with an arbitrary number of
   dimensions and works for both C and Fortran ordered matrices (i.e.,
   row-major or column-major). Note that a C program may use Fortran order
   and a Fortran program may use C order.

   The ndims parameter specifies the number of dimensions in the full data
   array and gives the number of elements in array_of_sizes,
   array_of_subsizes, and array_of_starts.

   The number of elements of type oldtype in each dimension of the
   n-dimensional array and the requested subarray are specified by
   array_of_sizes and array_of_subsizes, respectively. For any dimension
   i, it is erroneous to specify array_of_subsizes[i] < 1 or
   array_of_subsizes[i] > array_of_sizes[i].

   The array_of_starts contains the starting coordinates of each dimension
   of the subarray. Arrays are assumed to be indexed starting from zero.
   For any dimension i, it is erroneous to specify array_of_starts[i] < 0
   or array_of_starts[i] > ( array_of_sizes[i] - array_of_subsizes[i]).

   Advice to users.

   In a Fortran program with arrays indexed starting from 1, if the
   starting coordinate of a particular dimension of the subarray is n,
   then the entry in array_of_starts for that dimension is n-1. ( End of
   advice to users.)
   The order argument specifies the storage order for the subarray as well
   as the full array. It must be set to one of the following:

   { MPI_ORDER_C}
          The ordering used by C arrays, (i.e., row-major order)

   { MPI_ORDER_FORTRAN}
          The ordering used by Fortran arrays, (i.e., column-major order)

   A ndims-dimensional subarray ( newtype) with no extra padding can be
   defined by the function Subarray() as follows:

   [img4.gif]

   Let the typemap of oldtype have the form:
   {(type[0],disp[0]),(type[1],disp[1]),...,(type[n-1],disp[n-1])} where
   type[i] is a predefined MPI datatype, and let ex be the extent of
   oldtype. Then we define the Subarray() function recursively using the
   following three equations. Equation 1 defines the base step. Equation 1
   defines the recursion step when order = MPI_ORDER_FORTRAN, and Equation
   1 defines the recursion step when order = MPI_ORDER_C.


   [img5.gif]

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

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int myrank;
    MPI_Status status;
    MPI_Datatype subarray;
    int array[9] = { -1, 1, 2, 3, -2, -3, -4, -5, -6 };
    int array_size[] = {9};
    int array_subsize[] = {3};
    int array_start[] = {1};
    int i;

    MPI_Init(&argc, &argv);

    /* Create a subarray datatype */
    MPI_Type_create_subarray(1, array_size, array_subsize, array_start,
MPI_ORDER_C, MPI_INT, &subarray);
    MPI_Type_commit(&subarray);

    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    if (myrank == 0)
    {
        MPI_Send(array, 1, subarray, 1, 123, MPI_COMM_WORLD);
    }
    else if (myrank == 1)
    {
        for (i=0; i<9; i++)
            array[i] = 0;
        MPI_Recv(array, 1, subarray, 0, 123, MPI_COMM_WORLD, &status);
        for (i=0; i<9; i++)
            printf("array[%d] = %d\n", i, array[i]);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}

