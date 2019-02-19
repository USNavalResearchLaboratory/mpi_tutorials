/* 
MPI_Win_free

   Free an MPI RMA window
int MPI_Win_free(
  MPI_Win *win
);

Parameters

   win
          [in] window object (handle)

Remarks

   Frees the window object win and returns a null handle (equal to
   MPI_WIN_NULL). This is a collective call executed by all processes in
   the group associated with win. MPI_WIN_FREE(win) can be invoked by a
   process only after it has completed its involvement in RMA
   communications on window win: i.e., the process has called
   MPI_WIN_FENCE, or called MPI_WIN_WAIT to match a previous call to
   MPI_WIN_POST or called MPI_WIN_COMPLETE to match a previous call to
   MPI_WIN_START or called MPI_WIN_UNLOCK to match a previous call to
   MPI_WIN_LOCK. When the call returns, the window memory can be freed.

   If successfully freed, win is set to MPI_WIN_NULL.

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
        printf("Run this program with 2 processes\n");
	fflush(stdout);
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
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD,&win);
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
                printf("Get Error: B[i] is %d, should be %d\n", B[i],(-4)*(i+SIZE1));
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

