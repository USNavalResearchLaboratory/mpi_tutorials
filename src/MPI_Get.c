/* 
MPI_Get

   Get data from a memory window on a remote process
int MPI_Get(
  void *origin_addr,
  int origin_count,
  MPI_Datatype origin_datatype,
  int target_rank,
  MPI_Aint target_disp,
  int target_count,
  MPI_Datatype target_datatype,
  MPI_Win win
);

Parameters

   origin_addr
          [in] Address of the buffer in which to receive the data

   origin_count
          [in] number of entries in origin buffer (nonnegative integer)

   origin_datatype
          [in] datatype of each entry in origin buffer (handle)

   target_rank
          [in] rank of target (nonnegative integer)

   target_disp
          [in] displacement from window start to the beginning of the
          target buffer (nonnegative integer)

   target_count
          [in] number of entries in target buffer (nonnegative integer)

   target_datatype
          [in] datatype of each entry in target buffer (handle)

   win
          [in] window object used for communication (handle)

Remarks

   Similar to MPI_PUT, except that the direction of data transfer is
   reversed. Data are copied from the target memory to the origin. The
   origin_datatype may not specify overlapping entries in the origin
   buffer. The target buffer must be contained within the target window,
   and the copied data must fit, without truncation, in the origin buffer.

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
   call statement. All MPI objects (e.g., MPI_Datatype, MPI_Comm) are of
   type INTEGER in Fortran.

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

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

   MPI_ERR_COUNT
          Invalid count argument. Count arguments must be non-negative; a
          count of zero is often valid.

   MPI_ERR_RANK
          Invalid source or destination rank. Ranks must be between zero
          and the size of the communicator minus one; ranks in a receive
          (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be
          MPI_ANY_SOURCE.

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

   MPI_ERR_WIN
          Invalid MPI window object

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "stdio.h"
#include "mpi.h"

/* This does a transpose with a get operation, fence, and derived
    datatypes. Uses vector and hvector (Example 3.32 from MPI 1.1
    Standard). Run on 2 processes */

#define NROWS 100
#define NCOLS 100

int main(int argc, char *argv[])
{
    int rank, nprocs, A[NROWS][NCOLS], i, j;
    MPI_Win win;
    MPI_Datatype column, xpose;
    int errs = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if (nprocs != 2) {
        printf("Run this program with 2 processes\n");
	fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0)
    {
        for (i=0; i<NROWS; i++)
            for (j=0; j<NCOLS; j++)
                A[i][j] = -1;

        /* create datatype for one column */
        MPI_Type_vector(NROWS, 1, NCOLS, MPI_INT, &column);
        /* create datatype for matrix in column-major order */
        MPI_Type_hvector(NCOLS, 1, sizeof(int), column, &xpose);
        MPI_Type_commit(&xpose);
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);

        MPI_Win_fence(0, win);

        MPI_Get(A, NROWS*NCOLS, MPI_INT, 1, 0, 1, xpose, win);
        MPI_Type_free(&column);
        MPI_Type_free(&xpose);

        MPI_Win_fence(0, win);

        for (j=0; j<NCOLS; j++)
        {
            for (i=0; i<NROWS; i++)
            {
                if (A[j][i] != i*NCOLS + j)
                {
                    if (errs < 50)
                    {
                        printf("Error: A[%d][%d]=%d should be %d\n", j, i, A[j][i], i*NCOLS + j);
			fflush(stdout);
                    }
                    errs++;
                }
            }
        }
        if (errs >= 50)
        {
            printf("Total number of errors: %d\n", errs);
	    fflush(stdout);
        }
    }
    else
    { /* rank = 1 */
        for (i=0; i<NROWS; i++)
            for (j=0; j<NCOLS; j++)
                A[i][j] = i*NCOLS + j;
        MPI_Win_create(A, NROWS*NCOLS*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        MPI_Win_fence(0, win);
        MPI_Win_fence(0, win);
    }

    MPI_Win_free(&win);
    MPI_Finalize();
    return errs;
}

