/* 
MPI_Testsome

   Tests for some given requests to complete
int MPI_Testsome(
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

   Behaves like MPI_WAITSOME, except that it returns immediately. If no
   operation has completed it returns outcount = 0. If there is no active
   handle in the list it returns outcount = MPI_UNDEFINED.

   MPI_TESTSOME is a local operation, which returns immediately, whereas
   MPI_WAITSOME willblock until a communication completes, if it was
   passed a list that contains at least one active handle. Both calls
   fulfill a fairness requirement: If a request for a receive repeatedly
   appears in a list of requests passed to MPI_WAITSOME or MPI_TESTSOME,
   and a matching send has been posted, then the receive will eventually
   succeed, unless the send is satisfied by another receive; and similarly
   for send requests.

   Errors that occur during the execution of MPI_TESTSOME are handled as
   for MPI_WAITSOME.

   Advice to users.

   The use of MPI_TESTSOME is likely to be more efficient than the use of
   MPI_TESTANY. The former returns information on all completed
   communications, with the latter, a new call is required for each
   communication that completes.

   A server with multiple clients can use MPI_WAITSOME so as not to starve
   any client. Clients send messages to the server with service requests.
   The server calls MPI_WAITSOME with one receive request for each client,
   and then handles all receives that completed. If a call to MPI_WAITANY
   is used instead, then one client could starve while requests from
   another client always sneak in first.

   While it is possible to list a request handle more than once in the
   array_of_requests, such an action is considered erroneous and may cause
   the program to unexecpectedly terminate or produce incorrect results.

Thread and Interrupt Safety

   This routine is thread-safe. This means that this routine may be safely
   used by multiple threads without the need for any user-provided thread
   locks. However, the routine is not interrupt safe. Typically, this is
   due to the use of memory allocation routines such as malloc or other
   non-MPICH runtime routines that are themselves not interrupt-safe.

Note on status for send operations

   For send operations, the only use of status is for MPI_Test_cancelled
   or in the case that there is an error, in which case the MPI_ERROR
   field of status will be set.

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
//#include <windows.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int rank, size, flag, i, count;
    int buffer[100];
    MPI_Request r[4];
    MPI_Status status[4];
    int index[4];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (size != 4)
    {
        printf("Please run with 4 processes.\n");
	fflush(stdout);
        MPI_Finalize();
        return 1;
    }

    if (rank == 0)
    {
        int remaining = 3;
        for (i=1; i<size; i++)
        {
            MPI_Irecv(&buffer[i], 1, MPI_INT, i, 123, MPI_COMM_WORLD, &r[i-1]);
        }
        while (remaining > 0)
        {
            MPI_Testsome(size-1, r, &count, index, status);
            if (count > 0)
            {
                printf("%d finished\n", count);
                remaining -= count;
            }
            else
            {
	        //Sleep(1);
	        sleep(1);
            }
        }
    }
    else
    {
        MPI_Send(buffer, 1, MPI_INT, 0, 123, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}
