/* 
MPI_Start

   Initiates a communication with a persistent request handle
int MPI_Start(
  MPI_Request *request
);

Parameters

   request
          [in] communication request (handle)

Remarks

   The argument, request, is a handle returned by one of the
   MPI_xsend/recv_init calls. The associated request should be inactive.
   The request becomes active once the call is made.

   If the request is for a send with ready mode, then a matching receive
   should be posted before the call is made. The communication buffer
   should not be accessed after the call, and until the operation
   completes.

   The call is local, with similar semantics to the nonblocking
   communication operations. That is, a call to MPI_START with a request
   created by MPI_SEND_INIT starts a communication in the same manner as a
   call to MPI_ISEND; a call to MPI_START with a request created by
   MPI_BSEND_INIT starts a communication in the same manner as a call to
   MPI_IBSEND; and so on.

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include "mpi.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    MPI_Request r;
    MPI_Status s;
    int flag;
    int buf[10];
    int rbuf[10];
    int tag = 27;
    int dest = 0;
    int rank, size, i;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    /* Create a persistent send request */
    MPI_Send_init( buf, 10, MPI_INT, dest, tag, MPI_COMM_WORLD, &r );

    /* Use that request */
    if (rank == 0) {
        int i;
        MPI_Request *rr = (MPI_Request *)malloc(size *
sizeof(MPI_Request));
        for (i=0; i<size; i++) {
            MPI_Irecv( rbuf, 10, MPI_INT, i, tag, MPI_COMM_WORLD,
&rr[i] );
        }
        MPI_Start( &r );
        MPI_Wait( &r, &s );
        MPI_Waitall( size, rr, MPI_STATUSES_IGNORE );
        free(rr);
    }
    else {
        MPI_Start( &r );
        MPI_Wait( &r, &s );
    }

    MPI_Request_free( &r );


    if (rank == 0)
    {
        MPI_Request sr;
        /* Create a persistent receive request */
        MPI_Recv_init( rbuf, 10, MPI_INT, MPI_ANY_SOURCE, tag,
MPI_COMM_WORLD, &r );
        MPI_Isend( buf, 10, MPI_INT, 0, tag, MPI_COMM_WORLD, &sr );
        for (i=0; i<size; i++) {
            MPI_Start( &r );
            MPI_Wait( &r, &s );
        }
        MPI_Wait( &sr, &s );
        MPI_Request_free( &r );
    }
    else {
        MPI_Send( buf, 10, MPI_INT, 0, tag, MPI_COMM_WORLD );
    }

    MPI_Finalize();
    return 0;
}

