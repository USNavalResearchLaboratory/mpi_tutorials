/* 
MPI_Win_wait

   Completes an RMA exposure epoch begun with MPI_Win_post
int MPI_Win_wait(
  MPI_Win win
);

Parameters

   win
          [in] window object (handle)

Remarks

   Completes an RMA exposure epoch started by a call to MPI_WIN_POST on
   win. This call matches calls to MPI_WIN_COMPLETE(win) issued by each of
   the origin processes that were granted access to the window during this
   epoch. The call to MPI_WIN_WAIT will block until all matching calls to
   MPI_WIN_COMPLETE have occurred. This guarantees that all these origin
   processes have completed their RMA accesses to the local window. When
   the call returns, all these RMA accesses will have completed at the
   target window.

   Figure 4 illustrates the use of these four functions:

                                [2party.gif]


   Figure 4: active target communication. Dashed arrows represent
   synchronizations and solid arrows represent data transfer.

   Process 0 puts data in the windows of processes 1 and 2 and process 3
   puts data in the window of process 2. Each start call lists the ranks
   of the processes whose windows will be accessed; each post call lists
   the ranks of the processes that access the local window. The figure
   illustrates a possible timing for the events, assuming strong
   synchronization; in a weak synchronization, the start, put or complete
   calls may occur ahead of the matching post calls.

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

   MPI_ERR_WIN
          Invalid MPI window object

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include "mpi.h"
#include "stdio.h"

/* tests put and get with post/start/complete/wait on 2 processes */

#define SIZE1 100
#define SIZE2 200

int main(int argc, char *argv[])
{
    int rank, destrank, nprocs, *A, *B, i;
    MPI_Group comm_group, group;
    MPI_Win win;
    int errs = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if (nprocs != 2) {
        printf("Run this program with 2 processes\n");fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    i = MPI_Alloc_mem(SIZE2 * sizeof(int), MPI_INFO_NULL, &A);
    if (i) {
        printf("Can't allocate memory in test program\n");
	fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    i = MPI_Alloc_mem(SIZE2 * sizeof(int), MPI_INFO_NULL, &B);
    if (i) {
        printf("Can't allocate memory in test program\n");
	fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Comm_group(MPI_COMM_WORLD, &comm_group);

    if (rank == 0) {
        for (i=0; i<SIZE2; i++) A[i] = B[i] = i;
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        destrank = 1;
        MPI_Group_incl(comm_group, 1, &destrank, &group);
        MPI_Win_start(group, 0, win);
        for (i=0; i<SIZE1; i++)
            MPI_Put(A+i, 1, MPI_INT, 1, i, 1, MPI_INT, win);
        for (i=0; i<SIZE1; i++)
            MPI_Get(B+i, 1, MPI_INT, 1, SIZE1+i, 1, MPI_INT, win);

        MPI_Win_complete(win);

        for (i=0; i<SIZE1; i++)
            if (B[i] != (-4)*(i+SIZE1)) {
                printf("Get Error: B[i] is %d, should be %d\n", B[i], (-4)*(i+SIZE1));
		fflush(stdout);
                errs++;
            }
    }
    else { /* rank=1 */
        for (i=0; i<SIZE2; i++) B[i] = (-4)*i;
        MPI_Win_create(B, SIZE2*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        destrank = 0;
        MPI_Group_incl(comm_group, 1, &destrank, &group);
        MPI_Win_post(group, 0, win);
        MPI_Win_wait(win);
        for (i=0; i<SIZE1; i++) {
            if (B[i] != i) {
                printf("Put Error: B[i] is %d, should be %d\n", B[i],i);
		fflush(stdout);
                errs++;
            }
        }
    }

    MPI_Group_free(&group);
    MPI_Group_free(&comm_group);
    MPI_Win_free(&win);
    MPI_Free_mem(A);
    MPI_Free_mem(B);

    MPI_Finalize();
    return errs;
}

