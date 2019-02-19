/* 
MPI_Wait

   Waits for an MPI request to complete
int MPI_Wait(
  MPI_Request *request,
  MPI_Status *status
);

Parameters

   request
          [in] request (handle)

   status
          [out] status object (Status). May be MPI_STATUS_IGNORE.

Remarks

   A call to MPI_WAIT returns when the operation identified by request is
   complete. If the communication object associated with this request was
   created by a nonblocking send or receive call, then the object is
   deallocated by the call to MPI_WAIT and the request handle is set to
   MPI_REQUEST_NULL. MPI_WAIT is a non-local operation.

   The call returns, in status, information on the completed operation.
   The content of the status object for a receive operation can be
   accessed. The status object for a send operation may be queried by a
   call to MPI_TEST_CANCELLED.

   One is allowed to call MPI_WAIT with a null or inactive request
   argument. In this case the operation returns immediately with empty
   status.

   Advice to users.

   Successful return of MPI_WAIT after a MPI_IBSEND implies that the user
   send buffer can be reused --- i.e., data has been sent out or copied
   into a buffer attached with MPI_BUFFER_ATTACH. Note that, at this
   point, we can no longer cancel the send. If a matching receive is never
   posted, then the buffer cannot be freed. This runs somewhat counter to
   the stated goal of MPI_CANCEL (always being able to free program space
   that was committed to the communication subsystem).

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

   The status argument must be declared as an array of size
   MPI_STATUS_SIZE, as in integer status(MPI_STATUS_SIZE).

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int myid, numprocs, left, right;
    int buffer[10], buffer2[10];
    MPI_Request request;
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    right = (myid + 1) % numprocs;
    left = myid - 1;
    if (left < 0)
        left = numprocs - 1;

    MPI_Irecv(buffer, 10, MPI_INT, left, 123, MPI_COMM_WORLD, &request);
    MPI_Send(buffer2, 10, MPI_INT, right, 123, MPI_COMM_WORLD);
    MPI_Wait(&request, &status);
    MPI_Finalize();
    return 0;
}

