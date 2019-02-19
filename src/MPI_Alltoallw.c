/* 
MPI_Alltoallw

   Generalized all-to-all communication allowing different datatypes,
   counts, and displacements for each partner
int MPI_Alltoallw(
  void *sendbuf,
  int *sendcnts,
  int *sdispls,
  MPI_Datatype *sendtypes,
  void *recvbuf,
  int *recvcnts,
  int *rdispls,
  MPI_Datatype *recvtypes,
  MPI_Comm comm
);

Parameters

   sendbuf
          [in] starting address of send buffer (choice)

   sendcounts
          [in] integer array equal to the group size specifying the number
          of elements to send to each processor (integer)

   sdispls
          [in] integer array (of length group size). Entry j specifies the
          displacement in bytes (relative to sendbuf) from which to take
          the outgoing data destined for process j

   sendtypes
          [in] array of datatypes (of length group size). Entry j
          specifies the type of data to send to process j (handle)

        recvbuf
                [out] address of receive buffer (choice)

   recvcounts
          [in] integer array equal to the group size specifying the number
          of elements that can be received from each processor (integer)

   rdispls
          [in] integer array (of length group size). Entry i specifies the
          displacement in bytes (relative to recvbuf) at which to place
          the incoming data from process i

   recvtypes
          [in] array of datatypes (of length group size). Entry i
          specifies the type of data received from process i (handle)

   comm
          [in] communicator (handle)

Remarks

   One of the basic data movement operations needed in parallel signal
   processing is the 2-D matrix transpose. This operation has motivated a
   generalization of the MPI_ALLTOALLV function. This new collective
   operation is MPI_ALLTOALLW; the "W" indicates that it is an extension
   to MPI_ALLTOALLV.

   The following function is the most general form of All-to-all. Like
   MPI_TYPE_CREATE_STRUCT, the most general type constructor,
   MPI_ALLTOALLW allows separate specification of count, displacement and
   datatype. In addition, to allow maximum flexibility, the displacement
   of blocks within the send and receive buffers is specified in bytes.

   No "in place" option is supported.

   The j-th block sent from process i is received by process j and is
   placed in the i-th block of recvbuf. These blocks need not all have the
   same size.

   The type signature associated with sendcounts[j], sendtypes[j] at
   process i must be equal to the type signature associated with
   recvcounts[i], recvtypes[i] at process j. This implies that the amount
   of data sent must be equal to the amount of data received, pairwise
   between every pair of processes. Distinct type maps between sender and
   receiver are still allowed.

   The outcome is as if each process sent a message to every other process
   with

   MPI_Send(sendbuf+sdispls[i],sendcounts[i],sendtypes[i] ,i,...),

   and received a message from every other process with a call to

   MPI_Recv(recvbuf+rdispls[i],recvcounts[i],recvtypes[i] ,i,...).


   All arguments on all processes are significant. The argument comm must
   describe the same communicator on all processes.

   If comm is an intercommunicator, then the outcome is as if each process
   in group A sends a message to each process in group B, and vice versa.
   The j-th send buffer of process i in group A should be consistent with
   the i-th receive buffer of process j in group B, and vice versa.

   Rationale.

   The MPI_ALLTOALLW function generalizes several MPI functions by
   carefully selecting the input arguments. For example, by making all but
   one process have sendcounts[i] = 0, this achieves an MPI_SCATTERW
   function.

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

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

   MPI_ERR_COUNT
          Invalid count argument. Count arguments must be non-negative; a
          count of zero is often valid.

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * This program tests MPI_Alltoallw by having processor i send different
 * amounts of data to each processor.
 * The first test sends i items to processor i from all processors. */

int main( int argc, char **argv )
{
    MPI_Comm comm;
    int *sbuf, *rbuf;
    int rank, size;
    int *sendcounts, *recvcounts, *rdispls, *sdispls;
    int i, j, *p, err;
    MPI_Datatype *sendtypes, *recvtypes;

    MPI_Init( &argc, &argv );
    err = 0;
    comm = MPI_COMM_WORLD;
    /* Create the buffer */
    MPI_Comm_size( comm, &size );
    MPI_Comm_rank( comm, &rank );
    sbuf = (int *)malloc( size * size * sizeof(int) );
    rbuf = (int *)malloc( size * size * sizeof(int) );
    if (!sbuf || !rbuf) {
        fprintf( stderr, "Could not allocated buffers!\n" );
        fflush(stderr);
        MPI_Abort( comm, 1 );
    }
    /* Load up the buffers */
    for (i=0; i<size*size; i++) {
        sbuf[i] = i + 100*rank;
        rbuf[i] = -i;
    }
    /* Create and load the arguments to alltoallv */
    sendcounts = (int *)malloc( size * sizeof(int) );
    recvcounts = (int *)malloc( size * sizeof(int) );
    rdispls = (int *)malloc( size * sizeof(int) );
    sdispls = (int *)malloc( size * sizeof(int) );
    sendtypes = (MPI_Datatype *)malloc( size * sizeof(MPI_Datatype) );
    recvtypes = (MPI_Datatype *)malloc( size * sizeof(MPI_Datatype) );
    if (!sendcounts || !recvcounts || !rdispls || !sdispls || !sendtypes || !recvtypes) {
        fprintf( stderr, "Could not allocate arg items!\n" );
        fflush(stderr);
        MPI_Abort( comm, 1 );
    }
    /* Note that process 0 sends no data (sendcounts[0] = 0) */
    for (i=0; i<size; i++) {
        sendcounts[i] = i;
        recvcounts[i] = rank;
        rdispls[i] = i * rank * sizeof(int);
        sdispls[i] = (((i+1) * (i))/2) * sizeof(int);
        sendtypes[i] = recvtypes[i] = MPI_INT;
    }
    MPI_Alltoallw( sbuf, sendcounts, sdispls, sendtypes,
                       rbuf, recvcounts, rdispls, recvtypes, comm );
    /* Check rbuf */
    for (i=0; i<size; i++) {
        p = rbuf + rdispls[i]/sizeof(int);
        for (j=0; j<rank; j++) {
            if (p[j] != i * 100 + (rank*(rank+1))/2 + j) {
                fprintf( stderr, "[%d] got %d expected %d for %dth\n",
                                    rank, p[j],(i*(i+1))/2 + j, j );
                fflush(stderr);
                err++;
            }
        }
    }
    free( sendtypes );
    free( recvtypes );
    free( sdispls );
    free( rdispls );
    free( recvcounts );
    free( sendcounts );
    free( rbuf );
    free( sbuf );

    MPI_Finalize();
    return 0;
}
