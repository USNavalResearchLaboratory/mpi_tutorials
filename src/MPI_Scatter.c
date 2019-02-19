/* 
MPI_Scatter

   Sends data from one process to all other processes in a communicator
int MPI_Scatter(
  void *sendbuf,
  int sendcnt,
  MPI_Datatype sendtype,
  void *recvbuf,
  int recvcnt,
  MPI_Datatype recvtype,
  int root,
  MPI_Comm comm
);

Parameters

   sendbuf
          [in] address of send buffer (choice, significant only at root)

   sendcount
          [in] number of elements sent to each process (integer,
          significant only at root)

   sendtype
          [in] data type of send buffer elements (significant only at
          root) (handle)

   recvbuf
          [out] address of receive buffer (choice)

   recvcount
          [in] number of elements in receive buffer (integer)

   recvtype
          [in] data type of receive buffer elements (handle)

   root
          [in] rank of sending process (integer)

   comm
          [in] communicator (handle)

Remarks

   MPI_SCATTER is the inverse operation to MPI_GATHER.

   The outcome is as if the root executed n send operations,

   [img112.gif]

   and each process executed a receive,

   [img113.gif]

   An alternative description is that the root sends a message with
   MPI_Send(sendbuf, sendcount [img114.gif] n, sendtype, ...). This
   message is split into n equal segments, the ith segment is sent to the
   ith process in the group, and each process receives this message as
   above.

   The send buffer is ignored for all non-root processes.

   The type signature associated with sendcount, sendtype at the root must
   be equal to the type signature associated with recvcount, recvtype at
   all processes (however, the type maps may be different). This implies
   that the amount of data sent must be equal to the amount of data
   received, pairwise between each process and the root. Distinct type
   maps between sender and receiver are still allowed.

   All arguments to the function are significant on process root, while on
   other processes, only arguments recvbuf, recvcount, recvtype, root,
   comm are significant. The arguments root and comm must have identical
   values on all processes.

   The specification of counts and types should not cause any location on
   the root to be read more than once.

   The "in place" option for intracommunicators is specified by passing
   MPI_IN_PLACE as the value of recvbuf at the root. In such case,
   recvcount and recvtype are ignored, and root "sends" no data to itself.
   The scattered vector is still assumed to contain n segments, where n is
   the group size; the root-th segment, which root should "send to
   itself," is not moved.

   If comm is an intercommunicator, then the call involves all processes
   in the intercommunicator, but with one group (group A) defining the
   root process. All processes in the other group (group B) pass the same
   value in argument root, which is the rank of the root in group A. The
   root passes the value MPI_ROOT in root. All other processes in group A
   pass the value MPI_PROC_NULL in root. Data is scattered from the root
   to all processes in group B. The receive buffer arguments of the
   processes in group B must be consistent with the send buffer argument
   of the root.

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

#define MAX_PROCESSES 10

int main( int argc, char **argv )
{
    int rank, size, i,j;
    int table[MAX_PROCESSES][MAX_PROCESSES];
    int row[MAX_PROCESSES];
    int errors=0;
    int participants;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    /* A maximum of MAX_PROCESSES processes can participate */
    if ( size > MAX_PROCESSES ) participants = MAX_PROCESSES;
    else participants = size;
    if ( (rank < participants) ) {
        int send_count = MAX_PROCESSES;
        int recv_count = MAX_PROCESSES;

        /* If I'm the root (process 0), then fill out the big table */
        if (rank == 0)
            for ( i=0; i<participants; i++)
                for ( j=0; j<MAX_PROCESSES; j++ )
                    table[i][j] = i+j;
        /* Scatter the big table to everybody's little table */
        MPI_Scatter(&table[0][0], send_count, MPI_INT,
                         &row[0] , recv_count, MPI_INT, 0,
MPI_COMM_WORLD);

        /* Now see if our row looks right */
        for (i=0; i<MAX_PROCESSES; i++)
            if ( row[i] != i+rank ) errors++;
    }

    MPI_Finalize();
    return errors;
}

