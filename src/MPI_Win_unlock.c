/* 
MPI_Win_unlock

   Completes an RMA access epoch at the target process
int MPI_Win_unlock(
  int rank,
  MPI_Win win
);

Parameters

   rank
          [in] rank of window (nonnegative integer)

   win
          [in] window object (handle)

Remarks

   Completes an RMA access epoch started by a call to
   MPI_WIN_LOCK(...,win). RMA operations issued during this period will
   have completed both at the origin and at the target when the call
   returns.

   Locks are used to protect accesses to the locked target window effected
   by RMA calls issued between the lock and unlock call, and to protect
   local load/store accesses to a locked local window executed between the
   lock and unlock call. Accesses that are protected by an exclusive lock
   will not be concurrent at the window site with other accesses to the
   same window that are lock protected. Accesses that are protected by a
   shared lock will not be concurrent at the window site with accesses
   protected by an exclusive lock to the same window.

   It is erroneous to have a window locked and exposed (in an exposure
   epoch) concurrently. I.e., a process may not call MPI_WIN_LOCK to lock
   a target window if the target process has called MPI_WIN_POST and has
   not yet called MPI_WIN_WAIT; it is erroneous to call MPI_WIN_POST while
   the local window is locked.

   Rationale.

   An alternative is to require MPI to enforce mutual exclusion between
   exposure epochs and locking periods. But this would entail additional
   overheads when locks or active target synchronization do not interact
   in support of those rare interactions between the two mechanisms. The
   programming style that we encourage here is that a set of windows is
   used with only one synchronization mechanism at a time, with shifts
   from one mechanism to another being rare and involving global
   synchronization. ( End of rationale.)
   Advice to users.

   Users need to use explicit synchronization code in order to enforce
   mutual exclusion between locking periods and exposure epochs on a
   window. ( End of advice to users.)

   Implementors may restrict the use of RMA communication that is
   synchronized by lock calls to windows in memory allocated by
   MPI_ALLOC_MEM. Locks can be used portably only in such memory.

   Rationale.

   The implementation of passive target communication when memory is not
   shared requires an asynchronous agent. Such an agent can be implemented
   more easily, and can achieve better performance, if restricted to
   specially allocated memory. It can be avoided altogether if shared
   memory is used. It seems natural to impose restrictions that allows one
   to use shared memory for 3-rd party communication in shared memory
   machines.

   The downside of this decision is that passive target communication
   cannot be used without taking advantage of nonstandard Fortran
   features: namely, the availability of C-like pointers; these are not
   supported by some Fortran compilers (g77 and Windows/NT compilers, at
   the time of writing). Also, passive target communication cannot be
   portably targeted to COMMON blocks, or other statically declared
   Fortran arrays. ( End of rationale.)
   Consider the sequence of calls in the example below:
   MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, assert, win)
   MPI_Put(..., rank, ..., win)
   MPI_Win_unlock(rank, win)

   The call to MPI_WIN_UNLOCK will not return until the put transfer has
   completed at the origin and at the target. This still leaves much
   freedom to implementors. The call to MPI_WIN_LOCK may block until an
   exclusive lock on the window is acquired; or, the call MPI_WIN_LOCK may
   not block, while the call to MPI_PUT blocks until a lock is acquired;
   or, the first two calls may not block, while MPI_WIN_UNLOCK blocks
   until a lock is acquired --- the update of the target window is then
   postponed until the call to MPI_WIN_UNLOCK occurs. However, if the call
   to MPI_WIN_LOCK is used to lock a local window, then the call must
   block until the lock is acquired, since the lock may protect local
   load/store accesses to the window issued after the lock call returns.

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

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

See Also

   MPI_Win_lock

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
        printf("Run this program with 2 processes\n");fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    if (rank == 0) {
        for (i=0; i<SIZE2; i++) A[i] = B[i] = i;
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
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

