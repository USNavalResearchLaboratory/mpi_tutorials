/* 
MPI_Win_test

   Test whether an RMA exposure epoch has completed
int MPI_Win_test(
  MPI_Win win,
  int *flag
);

Parameters

   win
          [in] window object (handle)

   flag
          [out] success flag (logical)

Remarks

   This is the nonblocking version of MPI_Win_wait.

   It returns flag = true if MPI_WIN_WAIT would return, flag = false,
   otherwise. The effect of return of MPI_WIN_TEST with flag = true is the
   same as the effect of a return of MPI_WIN_WAIT. If flag = false is
   returned, then the call has no visible effect.

   MPI_WIN_TEST should be invoked only where MPI_WIN_WAIT can be invoked.
   Once the call has returned flag = true, it must not be invoked anew,
   until the window is posted anew.

   Assume that window win is associated with a "hidden" communicator
   wincomm, used for communication by the processes of win. The rules for
   matching of post and start calls and for matching complete and wait
   call can be derived from the rules for matching sends and receives, by
   considering the following (partial) model implementation.


   { MPI_WIN_POST(group,0,win)}
          initiate a nonblocking send with tag tag0 to each process in
          group, using wincomm. No need to wait for the completion of
          these sends.

   { MPI_WIN_START(group,0,win)}
          initiate a nonblocking receive with tag tag0 from each process
          in group, using wincomm. An RMA access to a window in target
          process i is delayed until the receive from i is completed.

   { MPI_WIN_COMPLETE(win)}
          initiate a nonblocking send with tag tag1 to each process in the
          group of the preceding start call. No need to wait for the
          completion of these sends.

   { MPI_WIN_WAIT(win)}
          initiate a nonblocking receive with tag tag1 from each process
          in the group of the preceding post call. Wait for the completion
          of all receives.

   No races can occur in a correct program: each of the sends matches a
   unique receive, and vice-versa.

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

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

See Also

   MPI_Win_wait, MPI_Win_post

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include "stdio.h"
/* tests put and get with post/start/complete/test on 2 processes */
#define SIZE1 10
#define SIZE2 20
int main(int argc, char *argv[])
{
    int rank, destrank, nprocs, A[SIZE2], B[SIZE2], i;
    MPI_Group comm_group, group;
    MPI_Win win;
    int errs = 0, flag;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if (nprocs != 2) {
        printf("Run this program with 2 processes\n");fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD,1);
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
    else {  /* rank=1 */
        for (i=0; i<SIZE2; i++) B[i] = (-4)*i;
        MPI_Win_create(B, SIZE2*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        destrank = 0;
        MPI_Group_incl(comm_group, 1, &destrank, &group);
        MPI_Win_post(group, 0, win);
        flag = 0;
        while (!flag)
            MPI_Win_test(win, &flag);

        for (i=0; i<SIZE1; i++) {
            if (B[i] != i) {
                printf("Put Error: B[i] is %d, should be %d\n", B[i], i);
		fflush(stdout);
                errs++;
            }
        }
    }
    MPI_Group_free(&group);
    MPI_Group_free(&comm_group);
    MPI_Win_free(&win);
    MPI_Finalize();
    return errs;
}

