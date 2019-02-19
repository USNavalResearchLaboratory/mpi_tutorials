/* 
MPI_Put

   Put data into a memory window on a remote process
int MPI_Put(
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
          [in] initial address of origin buffer (choice)

   origin_count
          [in] number of entries in origin buffer (nonnegative integer)

   origin_datatype
          [in] datatype of each entry in origin buffer (handle)

   target_rank
          [in] rank of target (nonnegative integer)

   target_disp
          [in] displacement from start of window to target buffer
          (nonnegative integer)

   target_count
          [in] number of entries in target buffer (nonnegative integer)

   target_datatype
          [in] datatype of each entry in target buffer (handle)

   win
          [in] window object used for communication (handle)

Remarks

   The execution of a put operation is similar to the execution of a send
   by the origin process and a matching receive by the target process. The
   obvious difference is that all arguments are provided by one call ---
   the call executed by the origin process.

   Transfers origin_count successive entries of the type specified by the
   origin_datatype, starting at address origin_addr on the origin node to
   the target node specified by the win, target_rank pair. The data are
   written in the target buffer at address target_addr = window_base +
   target_disp�disp_unit, where window_base and disp_unit are the base
   address and window displacement unit specified at window
   initialization, by the target process.

   The target buffer is specified by the arguments target_count and
   target_datatype.

   The data transfer is the same as that which would occur if the origin
   process executed a send operation with arguments origin_addr,
   origin_count, origin_datatype, target_rank, tag, comm, and the target
   process executed a receive operation with arguments target_addr,
   target_count, target_datatype, source, tag, comm, where target_addr is
   the target buffer address computed as explained above, and comm is a
   communicator for the group of win.

   The communication must satisfy the same constraints as for a similar
   message-passing communication. The target_datatype may not specify
   overlapping entries in the target buffer. The message sent must fit,
   without truncation, in the target buffer. Furthermore, the target
   buffer must fit in the target window.

   The target_datatype argument is a handle to a datatype object defined
   at the origin process. However, this object is interpreted at the
   target process: the outcome is as if the target datatype object was
   defined at the target process, by the same sequence of calls used to
   define it at the origin process. The target datatype must contain only
   relative displacements, not absolute addresses. The same holds for get
   and accumulate.

   Advice to users.

   The target_datatype argument is a handle to a datatype object that is
   defined at the origin process, even though it defines a data layout in
   the target process memory. This causes no problems in a homogeneous
   environment, or in a heterogeneous environment, if only portable
   datatypes are used.

   The performance of a put transfer can be significantly affected, on
   some systems, from the choice of window location and the shape and
   location of the origin and target buffer: transfers to a target window
   in memory allocated by MPI_ALLOC_MEM may be much faster on shared
   memory systems; transfers from contiguous buffers will be faster on
   most, if not all, systems; the alignment of the communication buffers
   may also impact performance.

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

