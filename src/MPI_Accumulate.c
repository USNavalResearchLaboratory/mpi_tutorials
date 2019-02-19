/* 
MPI_Accumulate

   Accumulate data into the target process using remote memory access
int MPI_Accumulate(
  void *origin_addr,
  int origin_count,
  MPI_Datatype origin_datatype,
  int target_rank,
  MPI_Aint target_disp,
  int target_count,
  MPI_Datatype target_datatype,
  MPI_Op op,
  MPI_Win win
) ;

Parameters

   origin_addr
          [in] initial address of buffer (choice)

   origin_count
          [in] number of entries in buffer (nonnegative integer)

   origin_datatype
          [in] datatype of each buffer entry (handle)

   target_rank
          [in] rank of target (nonnegative integer)

   target_disp
          [in] displacement from start of window to beginning of target
          buffer (nonnegative integer)

   target_count
          [in] number of entries in target buffer (nonnegative integer)

   target_datatype
          [in] datatype of each entry in target buffer (handle)

   op
          [in] predefined reduce operation (handle)

   win
          [in] window object (handle)

Remarks

   It is often useful in a put operation to combine the data moved to the
   target process with the data that resides at that process, rather then
   replacing the data there. This will allow, for example, the
   accumulation of a sum by having all involved processes add their
   contribution to the sum variable in the memory of one process.

   Accumulate the contents of the origin buffer (as defined by
   origin_addr, origin_count and origin_datatype) to the buffer specified
   by arguments target_count and target_datatype, at offset target_disp,
   in the target window specified by target_rank and win, using the
   operation op. This is like MPI_PUT except that data is combined into
   the target area instead of overwriting it.

   Any of the predefined operations for MPI_REDUCE can be used.
   User-defined functions cannot be used. For example, if op is MPI_SUM,
   each element of the origin buffer is added to the corresponding element
   in the target, replacing the former value in the target.

   Each datatype argument must be a predefined datatype or a derived
   datatype, where all basic components are of the same predefined
   datatype. Both datatype arguments must be constructed from the same
   predefined datatype. The operation op applies to elements of that
   predefined type. target_datatype must not specify overlapping entries,
   and the target buffer must fit in the target window.

   A new predefined operation, MPI_REPLACE, is defined. It corresponds to
   the associative function f(a,b) = b; i.e., the current value in the
   target memory is replaced by the value supplied by the origin.
   The basic components of both the origin and target datatype must be the
   same predefined datatype (e.g., all MPI_INT or all
   MPI_DOUBLE_PRECISION).

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

#include "mpi.h"
#include "stdio.h"
/* This does a transpose-cum-accumulate operation. Uses vector and
hvector datatypes (Example 3.32 from MPI 1.1 Standard). Run on 2
processes */
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
    if (nprocs != 2)
    {
        printf("Run this program with 2 processes\n");fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    if (rank == 0)
    {
        for (i=0; i<NROWS; i++)
            for (j=0; j<NCOLS; j++)
                A[i][j] = i*NCOLS + j;
        /* create datatype for one column */
        MPI_Type_vector(NROWS, 1, NCOLS, MPI_INT, &column);
        /* create datatype for matrix in column-major order */
        MPI_Type_hvector(NCOLS, 1, sizeof(int), column, &xpose);
        MPI_Type_commit(&xpose);
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        MPI_Win_fence(0, win);
        MPI_Accumulate(A, NROWS*NCOLS, MPI_INT, 1, 0, 1, xpose, MPI_SUM, win);
        MPI_Type_free(&column);
        MPI_Type_free(&xpose);
        MPI_Win_fence(0, win);
    }
    else
    { /* rank = 1 */
        for (i=0; i<NROWS; i++)
            for (j=0; j<NCOLS; j++)
                A[i][j] = i*NCOLS + j;
        MPI_Win_create(A, NROWS*NCOLS*sizeof(int), sizeof(int), MPI_INFO_NULL,
                       MPI_COMM_WORLD, &win);
        MPI_Win_fence(0, win);
        MPI_Win_fence(0, win);
        for (j=0; j<NCOLS; j++)
        {
            for (i=0; i<NROWS; i++)
            {
                if (A[j][i] != i*NCOLS + j + j*NCOLS + i)
                {
                    if (errs < 50)
                    {
                        printf("Error: A[%d][%d]=%d should be %d\n", j, i,
                            A[j][i], i*NCOLS + j + j*NCOLS + i);
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
    MPI_Win_free(&win);
    MPI_Finalize();
    return 0;
}

