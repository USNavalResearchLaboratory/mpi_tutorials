/* 
MPI_Win_fence

   Perform an MPI fence synchronization on a MPI window
int MPI_Win_fence(
  int assert,
  MPI_Win win
);

Parameters

   assert
          [in] program assertion (integer)

   win
          [in] window object (handle)

Remarks

   The MPI call MPI_WIN_FENCE(assert, win) synchronizes RMA calls on win.
   The call is collective on the group of win. All RMA operations on win
   originating at a given process and started before the fence call will
   complete at that process before the fence call returns. They will be
   completed at their target before the fence call returns at the target.
   RMA operations on win started by a process after the fence call returns
   will access their target window only after MPI_WIN_FENCE has been
   called by the target process.

   The call completes an RMA access epoch if it was preceded by another
   fence call and the local process issued RMA communication calls on win
   between these two calls. The call completes an RMA exposure epoch if it
   was preceded by another fence call and the local window was the target
   of RMA accesses between these two calls. The call starts an RMA access
   epoch if it is followed by another fence call and by RMA communication
   calls issued between these two fence calls. The call starts an exposure
   epoch if it is followed by another fence call and the local window is
   the target of RMA accesses between these two fence calls. Thus, the
   fence call is equivalent to calls to a subset of post, start, complete,
   wait.

   A fence call usually entails a barrier synchronization: a process
   completes a call to MPI_WIN_FENCE only after all other processes in the
   group entered their matching call. However, a call to MPI_WIN_FENCE
   that is known not to end any epoch (in particular, a call with assert =
   MPI_MODE_NOPRECEDE) does not necessarily act as a barrier.

   The assert argument is used to indicate special conditions for the
   fence that an implementation may use to optimize the MPI_Win_fence
   operation. The value zero is always correct. Other assertion values may
   be or'ed together. Assertions that are valid for MPI_Win_fence are:

   MPI_MODE_NOSTORE
          the local window was not updated by local stores (or local get
          or receive calls) since last synchronization.

   MPI_MODE_NOPUT
          the local window will not be updated by put or accumulate calls
          after the fence call, until the ensuing (fence) synchronization.

   MPI_MODE_NOPRECEDE
          the fence does not complete any sequence of locally issued RMA
          calls. If this assertion is given by any process in the window
          group, then it must be given by all processes in the group.

   MPI_MODE_NOSUCCEED
          the fence does not start any sequence of locally issued RMA
          calls. If the assertion is given by any process in the window
          group, then it must be given by all processes in the group.

          Advice to users.

          Calls to MPI_WIN_FENCE should both precede and follow calls to
          put, get or accumulate that are synchronized with fence calls.

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

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

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
        printf("Run this program with 2 processes\n");
	fflush(stdout);
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
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD,&win);
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
        MPI_Win_create(A, NROWS*NCOLS*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
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
                            A[j][i], i*NCOLS + j + j*NCOLS +i);
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

