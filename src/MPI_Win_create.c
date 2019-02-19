/* 
MPI_Win_create

   Create an MPI Window object for one-sided communication
int MPI_Win_create(
  void *base,
  MPI_Aint size,
  int disp_unit,
  MPI_Info info,
  MPI_Comm comm,
  MPI_Win *win
);

Parameters

   base
          [in] initial address of window (choice)

   size
          [in] size of window in bytes (nonnegative integer)

   disp_unit
          [in] local unit size for displacements, in bytes (positive
          integer)

   info
          [in] info argument (handle)

   comm
          [in] communicator (handle)

   win
          [out] window object returned by the call (handle)

Remarks

   The initialization operation allows each process in an
   intracommunicator group to specify, in a collective operation, a
   "window" in its memory that is made accessible to accesses by remote
   processes. The call returns an opaque object that represents the group
   of processes that own and access the set of windows, and the attributes
   of each window, as specified by the initialization call.

   This is a collective call executed by all processes in the group of
   comm. It returns a window object that can be used by these processes to
   perform RMA operations. Each process specifies a window of existing
   memory that it exposes to RMA accesses by the processes in the group of
   comm. The window consists of size bytes, starting at address base. A
   process may elect to expose no memory by specifying size = 0.

   The displacement unit argument is provided to facilitate address
   arithmetic in RMA operations: the target displacement argument of an
   RMA operation is scaled by the factor disp_unit specified by the target
   process, at window creation.

   Rationale.

   The window size is specified using an address sized integer, so as to
   allow windows that span more than 4 GB of address space. (Even if the
   physical memory size is less than 4 GB, the address range may be larger
   than 4 GB, if addresses are not contiguous.)
   Advice to users.

   Common choices for disp_unit are 1 (no scaling), and (in C syntax)
   sizeof(type), for a window that consists of an array of elements of
   type type. The later choice will allow one to use array indices in RMA
   calls, and have those scaled correctly to byte displacements, even in a
   heterogeneous environment. ( End of advice to users.)
   The info argument provides optimization hints to the runtime about the
   expected usage pattern of the window. The following info key is
   predefined:



          no_locks --- if set to true, then the implementation may assume
          that the local window is never locked (by a call to
          MPI_WIN_LOCK). This implies that this window is not used for
          3-party communication, and RMA can be implemented with no (less)
          asynchronous agent activity at this process.

   The various processes in the group of comm may specify completely
   different target windows, in location, size, displacement units and
   info arguments. As long as all the get, put and accumulate accesses to
   a particular process fit their specific target window this should pose
   no problem. The same area in memory may appear in multiple windows,
   each associated with a different window object. However, concurrent
   communications to distinct, overlapping windows may lead to erroneous
   results.

   Advice to users.

   A window can be created in any part of the process memory. However, on
   some systems, the performance of windows in memory allocated by
   MPI_ALLOC_MEM will be better. Also, on some systems, performance is
   improved when window boundaries are aligned at "natural" boundaries
   (word, double-word, cache line, page frame, etc.).

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

   MPI_ERR_INFO
          Invalid Info

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

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
        MPI_Win_create(A, NROWS*NCOLS*sizeof(int), sizeof(int),
		       MPI_INFO_NULL, MPI_COMM_WORLD, &win);
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

