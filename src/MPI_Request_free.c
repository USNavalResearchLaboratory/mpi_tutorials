/* 
MPI_Request_free

   Frees a communication request object
int MPI_Request_free(
  MPI_Request *request
);

Parameters

   request
          [in] communication request (handle)

Remarks

   Mark the request object for deallocation and set request to
   MPI_REQUEST_NULL. An ongoing communication that is associated with the
   request will be allowed to complete. The request will be deallocated
   only after its completion.

   This routine is normally used to free inactive persistent requests
   created with either MPI_Recv_init or MPI_Send_init and friends. It is
   also permissible to free an active request. However, once freed, the
   request can no longer be used in a wait or test routine (e.g.,
   MPI_Wait) to determine completion.

   This routine may also be used to free a non-persistent requests such as
   those created with MPI_Irecv or MPI_Isend and friends. Like active
   persistent requests, once freed, the request can no longer be used with
   test/wait routines to determine completion.

   Advice to users.

   Once a request is freed by a call to MPI_REQUEST_FREE, it is not
   possible to check for the successful completion of the associated
   communication with calls to MPI_WAIT or MPI_TEST. Also, if an error
   occurs subsequently during the communication, an error code cannot be
   returned to the user --- such an error must be treated as fatal.
   Questions arise as to how one knows when the operations have completed
   when using MPI_REQUEST_FREE. Depending on the program logic, there may
   be other ways in which the program knows that certain operations have
   completed and this makes usage of MPI_REQUEST_FREE practical. For
   example, an active send request could be freed when the logic of the
   program is such that the receiver sends a reply to the message sent ---
   the arrival of the reply informs the sender that the send has completed
   and the send buffer can be reused. An active receive request should
   never be freed as the receiver will have no way to verify that the
   receive has completed and the receive buffer can be reused.

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

See Also

   also: MPI_Isend, MPI_Irecv, MPI_Issend, MPI_Ibsend, MPI_Irsend,
   MPI_Recv_init, MPI_Send_init, MPI_Ssend_init, MPI_Rsend_init, MPI_Wait,
   MPI_Test, MPI_Waitall, MPI_Waitany, MPI_Waitsome, MPI_Testall,
   MPI_Testany, MPI_Testsome

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

