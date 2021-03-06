/* 
MPI_Waitsome

   Waits for some given MPI Requests to complete
int MPI_Waitsome(
  int incount,
  MPI_Request array_of_requests[],
  int *outcount,
  int array_of_indices[],
  MPI_Status array_of_statuses[]
);

Parameters

   incount
          [in] length of array_of_requests (integer)

   array_of_requests
          [in] array of requests (array of handles)

   outcount
          [out] number of completed requests (integer)

   array_of_indices
          [out] array of indices of operations that completed (array of
          integers)

   array_of_statuses
          [out] array of status objects for operations that completed
          (array of Status). May be MPI_STATUSES_IGNORE.

Remarks

   Waits until at least one of the operations associated with active
   handles in the list have completed. Returns in outcount the number of
   requests from the list array_of_requests that have completed. Returns
   in the first outcount locations of the array array_of_indices the
   indices of these operations (index within the array array_of_requests;
   the array is indexed from zero in C and from one in Fortran). Returns
   in the first outcount locations of the array array_of_status the status
   for these completed operations. If a request that completed was
   allocated by a nonblocking communication call, then it is deallocated,
   and the associated handle is set to MPI_REQUEST_NULL.

   If the list contains no active handles, then the call returns
   immediately with outcount = MPI_UNDEFINED.

   When one or more of the communications completed by MPI_WAITSOME fails,
   then it is desirable to return specific information on each
   communication. The arguments outcount, array_of_indices and
   array_of_statuses will be adjusted to indicate completion of all
   communications that have succeeded or failed. The call will return the
   error code MPI_ERR_IN_STATUS and the error field of each status
   returned will be set to indicate success or to indicate the specific
   error that occurred. The call will return MPI_SUCCESS if no request
   resulted in an error, and will return another error code if it failed
   for other reasons (such as invalid arguments). In such cases, it will
   not update the error fields of the statuses.
   The array of indicies are in the range 0 to incount - 1 for C and in
   the range 1 to incount for Fortran.

   Null requests are ignored; if all requests are null, then the routine
   returns with outcount set to MPI_UNDEFINED.

   While it is possible to list a request handle more than once in the
   array_of_requests, such an action is considered erroneous and may cause
   the program to unexecpectedly terminate or produce incorrect results.

   MPI_Waitsome provides an interface much like the Unix select or poll
   calls and, in a high qualilty implementation, indicates all of the
   requests that have completed when MPI_Waitsome is called. However,
   MPI_Waitsome only guarantees that at least one request has completed;
   there is no guarantee that all completed requests will be returned, or
   that the entries in array_of_indices will be in increasing order. Also,
   requests that are completed while MPI_Waitsome is executing may or may
   not be returned, depending on the timing of the completion of the
   message.

Note on status for send operations

   For send operations, the only use of status is for MPI_Test_cancelled
   or in the case that there is an error, in which case the MPI_ERROR
   field of status will be set.

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

   MPI_ERR_REQUEST
          Invalid MPI_Request. Either null or, in the case of a MPI_Start
          or MPI_Startall, not a persistent request.

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

   MPI_ERR_IN_STATUS
          The actual error value is in the MPI_Status argument. This error
          class is returned only from the multiple-completion routines
          (MPI_Testall, MPI_Testany, MPI_Testsome, MPI_Waitall,
          MPI_Waitany, and MPI_Waitsome). The field MPI_ERROR in the
          status argument contains the error value or MPI_SUCCESS (no
          error and complete) or MPI_ERR_PENDING to indicate that the
          request has not completed.

   The MPI Standard does not specify what the result of the multiple
   completion routines is when an error occurs. For example, in an
   MPI_WAITALL, does the routine wait for all requests to either fail or
   complete, or does it return immediately (with the MPI definition of
   immediately, which means independent of actions of other MPI
   processes)? MPICH has chosen to make the return immediate (alternately,
   local in MPI terms), and to use the error class MPI_ERR_PENDING
   (introduced in MPI 1.1) to indicate which requests have not completed.
   In most cases, only one request with an error will be detected in each
   call to an MPI routine that tests multiple requests. The requests that
   have not been processed (because an error occured in one of the
   requests) will have their MPI_ERROR field marked with MPI_ERR_PENDING.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int i, index[4], count, remaining;
    int buffer[400];
    MPI_Request request[4];
    MPI_Status status[4];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size > 4)
    {
        printf("Please run with 4 processes.\n");
	fflush(stdout);
        MPI_Finalize();
        return 1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
    {
        for (i=0; i<size * 100; i++)
            buffer[i] = i/100;
        for (i=0; i<size-1; i++)
        {
            MPI_Isend(&buffer[i*100], 100, MPI_INT, i+1, 123, MPI_COMM_WORLD, &request[i]);
        }
        remaining = size-1;
        while (remaining > 0)
        {
            MPI_Waitsome(size-1, request, &count, index, status);
            if (count > 0)
            {
                printf("%d sends completed\n", count);
		fflush(stdout);
                remaining = remaining - count;
            }
        }
    }
    else
    {
        MPI_Recv(buffer, 100, MPI_INT, 0, 123, MPI_COMM_WORLD, &status[0]);
        printf("%d: buffer[0] = %d\n", rank, buffer[0]);
	fflush(stdout);
    }
    MPI_Finalize();
    return 0;
}

