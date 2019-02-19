/* 
MPI_Exscan

   Computes the exclusive scan (partial reductions) of data on a
   collection of processes
int MPI_Exscan(
  void *sendbuf,
  void *recvbuf,
  int count,
  MPI_Datatype datatype,
  MPI_Op op,
  MPI_Comm comm
);

Parameters

   sendbuf
          [in] starting address of send buffer (choice)

   recvbuf
          [out] starting address of receive buffer (choice)

   count
          [in] number of elements in input buffer (integer)

   datatype
          [in] data type of elements of input buffer (handle)

   op
          [in] operation (handle)

   comm
          [in] communicator (handle)

Remarks

   MPI_Exscan is like MPI_Scan, except that the contribution from the
   calling process is not included in the result at the calling process
   (it is contributed to the subsequent processes, of course).

   MPI_EXSCAN is used to perform a prefix reduction on data distributed
   across the group. The value in recvbuf on the process with rank 0 is
   undefined, and recvbuf is not signficant on process 0. The value in
   recvbuf on the process with rank 1 is defined as the value in sendbuf
   on the process with rank 0. For processes with rank i > 1, the
   operation returns, in the receive buffer of the process with rank i,
   the reduction of the values in the send buffers of processes with ranks
   0,...,i-1 (inclusive). The type of operations supported, their
   semantics, and the constraints on send and receive buffers, are as for
   MPI_REDUCE.

   No "in place" option is supported.

   Advice to users.

   As for MPI_SCAN, MPI does not specify which processes may call the
   operation, only that the result be correctly computed. In particular,
   note that the process with rank 1 need not call the MPI_Op, since all
   it needs to do is to receive the value from the process with rank 0.
   However, all processes, even the processes with ranks zero and one,
   must provide the same op.
   Rationale.

   The exclusive scan is more general than the inclusive scan provided in
   MPI-1 as MPI_SCAN. Any inclusive scan operation can be achieved by
   using the exclusive scan and then locally combining the local
   contribution. Note that for non-invertable operations such as MPI_MAX,
   the exclusive scan cannot be computed with the inclusive scan.

   The reason that MPI-1 chose the inclusive scan is that the definition
   of behavior on processes zero and one was thought to offer too many
   complexities in definition, particularly for user-defined operations.

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

Notes on collective operations

   The reduction functions (MPI_Op) do not return an error value. As a
   result, if the functions detect an error, all they can do is either
   call MPI_Abort or silently skip the problem. Thus, if you change the
   error handler from MPI_ERRORS_ARE_FATAL to something else, for example,
   MPI_ERRORS_RETURN, then no error may be indicated.

   The reason for this is the performance problems in ensuring that all
   collective routines return the same error value.

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

   MPI_ERR_COUNT
          Invalid count argument. Count arguments must be non-negative; a
          count of zero is often valid.

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

   MPI_ERR_BUFFER
          Invalid buffer pointer. Usually a null buffer where one is not
          valid.

   MPI_ERR_BUFFER
          This error class is associcated with an error code that
          indicates that two buffer arguments are aliased; that is, the
          describe overlapping storage (often the exact same storage).
          This is prohibited in MPI (because it is prohibited by the
          Fortran standard, and rather than have a separate case for C and
          Fortran, the MPI Forum adopted the more restrictive requirements
          of Fortran).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )
{
    int errs = 0;
    int rank, size;
    int minsize = 2, count;
    int *sendbuf, *recvbuf, i;
    MPI_Comm comm;

    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );
    for (count = 1; count < 65000; count = count * 2)
    {
        sendbuf = (int *)malloc( count * sizeof(int) );
        recvbuf = (int *)malloc( count * sizeof(int) );
        for (i=0; i<count; i++) {
            sendbuf[i] = rank + i * size;
            recvbuf[i] = -1;
        }
        MPI_Exscan( sendbuf, recvbuf, count, MPI_INT, MPI_SUM, comm );
        /* Check the results. rank 0 has no data */
        if (rank > 0) {
            int result;
            for (i=0; i<count; i++) {
                result = rank * i * size + ((rank) * (rank-1))/2;
                if (recvbuf[i] != result) {
                    errs++;
                    if (errs < 10) {
                        fprintf( stderr, "Error in recvbuf[%d] = %d on %d, expected %d\n",
				 i, recvbuf[i], rank, result );
			fflush(stderr);
                    }
                }
            }
        }
        free( sendbuf );
        free( recvbuf );
    }
    
    MPI_Finalize();
    return 0;
}

