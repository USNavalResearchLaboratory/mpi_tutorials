/* 
MPI_Win_lock

   Begin an RMA access epoch at the target process.
int MPI_Win_lock(
  int lock_type,
  int rank,
  int assert,
  MPI_Win win
);

Parameters

   lock_type
          [in] Indicates whether other processes may access the target
          window at the same time (if MPI_LOCK_SHARED) or not
          (MPI_LOCK_EXCLUSIVE)

   rank
          [in] rank of locked window (nonnegative integer)

   assert
          [in] Used to optimize this call; zero may be used as a default.
          See notes. (integer)

   win
          [in] window object (handle)

Remarks

   Starts an RMA access epoch. Only the window at the process with rank
   rank can be accessed by RMA operations on win during that epoch.

   The name of this routine is misleading. In particular, this routine
   need not block, except when the target process is the calling process.

   Implementations may restrict the use of RMA communication that is
   synchronized by lock calls to windows in memory allocated by
   MPI_Alloc_mem. Locks can be used portably only in such memory.

   The assert argument is used to indicate special conditions for the
   fence that an implementation may use to optimize the MPI_Win_fence
   operation. The value zero is always correct. Other assertion values may
   be or'ed together. Assertions that are valid for MPI_Win_fence are:

   MPI_MODE_NOCHECK
          no other process holds, or will attempt to acquire a conflicting
          lock, while the caller holds the window lock. This is useful
          when mutual exclusion is achieved by other means, but the
          coherence operations that may be attached to the lock and unlock
          calls are still required.

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

   MPI_ERR_RANK
          Invalid source or destination rank. Ranks must be between zero
          and the size of the communicator minus one; ranks in a receive
          (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be
          MPI_ANY_SOURCE.

   MPI_ERR_WIN
          Invalid MPI window object

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"

/* tests passive target RMA on 2 processes */

#define SIZE1 100
#define SIZE2 200

int main(int argc, char *argv[])
{
    int rank, nprocs, A[SIZE2], B[SIZE2], i;
    MPI_Win win;
    int errs = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if (nprocs != 2) {
        printf("Run this program with 2 processes\n");
	fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    if (rank == 0) {
        for (i=0; i<SIZE2; i++) A[i] = B[i] = i;
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win)
;
        for (i=0; i<SIZE1; i++) {
            MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win);
            MPI_Put(A+i, 1, MPI_INT, 1, i, 1, MPI_INT, win);
            MPI_Win_unlock(1, win);
        }
        for (i=0; i<SIZE1; i++) {
            MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win);
            MPI_Get(B+i, 1, MPI_INT, 1, SIZE1+i, 1, MPI_INT, win);
            MPI_Win_unlock(1, win);
        }
        MPI_Win_free(&win);
        for (i=0; i<SIZE1; i++)
            if (B[i] != (-4)*(i+SIZE1)) {
                printf("Get Error: B[%d] is %d, should be %d\n", i, B[i], (-4)*(i+SIZE1));
		fflush(stdout);
                errs++;
            }
    }
    else {  /* rank=1 */
        for (i=0; i<SIZE2; i++) B[i] = (-4)*i;
        MPI_Win_create(B, SIZE2*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        MPI_Win_free(&win);

        for (i=0; i<SIZE1; i++) {
            if (B[i] != i) {
                printf("Put Error: B[%d] is %d, should be %d\n", i, B[i], i);
                errs++;
            }
        }
    }
    MPI_Finalize();
    return errs;
}

