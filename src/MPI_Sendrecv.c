/* 
MPI_Sendrecv

   Sends and receives a message
int MPI_Sendrecv(
  void *sendbuf,
  int sendcount,
  MPI_Datatype sendtype,
  int dest,
  int sendtag,
  void *recvbuf,
  int recvcount,
  MPI_Datatype recvtype,
  int source,
  int recvtag,
  MPI_Comm comm,
  MPI_Status *status
);

Parameters

   sendbuf
          [in] initial address of send buffer (choice)

   sendcount
          [in] number of elements in send buffer (integer)

   sendtype
          [in] type of elements in send buffer (handle)

   dest
          [in] rank of destination (integer)

   sendtag
          [in] send tag (integer)

   recvbuf
          [out] initial address of receive buffer (choice)

   recvcount
          [in] number of elements in receive buffer (integer)

   recvtype
          [in] type of elements in receive buffer (handle)

   source
          [in] rank of source (integer)

   recvtag
          [in] receive tag (integer)

   comm
          [in] communicator (handle)

   status
          [out] status object (Status). This refers to the receive
          operation.

Remarks

   Execute a blocking send and receive operation. Both send and receive
   use the same communicator, but possibly different tags. The send buffer
   and receive buffers must be disjoint, and may have different lengths
   and datatypes.

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

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

   MPI_ERR_COUNT
          Invalid count argument. Count arguments must be non-negative; a
          count of zero is often valid.

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

   MPI_ERR_TAG
          Invalid tag argument. Tags must be non-negative; tags in a
          receive (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be
          MPI_ANY_TAG. The largest tag value is available through the the
          attribute MPI_TAG_UB.

   MPI_ERR_RANK
          Invalid source or destination rank. Ranks must be between zero
          and the size of the communicator minus one; ranks in a receive
          (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be
          MPI_ANY_SOURCE.

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

    MPI_Sendrecv(buffer, 10, MPI_INT, left, 123, buffer2, 10, MPI_INT,
right, 123, MPI_COMM_WORLD, &status);

    MPI_Finalize();
    return 0;
}

