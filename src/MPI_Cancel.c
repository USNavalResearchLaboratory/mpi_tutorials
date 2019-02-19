/* 
MPI_Cancel

   Cancels a communication request
int MPI_Cancel(
  MPI_Request *request
);

Parameters

   request
          [in] communication request (handle)

Remarks

   The MPI_CANCEL operation allows pending communications to be canceled.
   This is required for cleanup. Posting a send or a receive ties up user
   resources (send or receive buffers), and a cancel may be needed to free
   these resources gracefully.

   A call to MPI_CANCEL marks for cancellation a pending, nonblocking
   communication operation (send or receive). The cancel call is local. It
   returns immediately, possibly before the communication is actually
   canceled. It is still necessary to complete a communication that has
   been marked for cancellation, using a call to MPI_REQUEST_FREE,
   MPI_WAIT or MPI_TEST (or any of the derived operations).

   If a communication is marked for cancellation, then a MPI_WAIT call for
   that communication is guaranteed to return, irrespective of the
   activities of other processes (i.e., MPI_WAIT behaves as a local
   function); similarly if MPI_TEST is repeatedly called in a busy wait
   loop for a canceled communication, then MPI_TEST will eventually be
   successful.

   MPI_CANCEL can be used to cancel a communication that uses a persistent
   request, in the same way it is used for nonpersistent requests. A
   successful cancellation cancels the active communication, but not the
   request itself. After the call to MPI_CANCEL and the subsequent call to
   MPI_WAIT or MPI_TEST, the request becomes inactive and can be activated
   for a new communication.

   The successful cancellation of a buffered send frees the buffer space
   occupied by the pending message.

   Either the cancellation succeeds, or the communication succeeds, but
   not both. If a send is marked for cancellation, then it must be the
   case that either the send completes normally, in which case the message
   sent was received at the destination process, or that the send is
   successfully canceled, in which case no part of the message was
   received at the destination. Then, any matching receive has to be
   satisfied by another send. If a receive is marked for cancellation,
   then it must be the case that either the receive completes normally, or
   that the receive is successfully canceled, in which case no part of the
   receive buffer is altered. Then, any matching send has to be satisfied
   by another receive.

   If the operation has been canceled, then information to that effect
   will be returned in the status argument of the operation that completes
   the communication.

   The primary expected use of MPI_Cancel is in multi-buffering schemes,
   where speculative MPI_Irecvs are made. When the computation completes,
   some of these receive requests may remain; using MPI_Cancel allows the
   user to cancel these unsatisfied requests.

   Cancelling a send operation is much more difficult, in large part
   because the send will usually be at least partially complete (the
   information on the tag, size, and source are usually sent immediately
   to the destination). Users are advised that cancelling a send, while a
   local operation (as defined by the MPI standard), is likely to be
   expensive (usually generating one or more internal messages).

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

Null Handles

   The MPI 1.1 specification, in the section on opaque objects, explicitly
   disallows freeing a null communicator. The text from the standard is:
 A null handle argument is an erroneous IN argument in MPI calls, unless an
 exception is explicitly stated in the text that defines the function. Such
 exception is allowed for handles to request objects in Wait and Test calls
 (sections Communication Completion and Multiple Completions ). Otherwise, a
 null handle can only be passed to a function that allocates a new object and
 returns a reference to it in the handle.

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
#include <stdlib.h>

/* Test of various send cancel calls */
int main( int argc, char *argv[] )
{
    int errs = 0;
    int rank, size, source, dest;
    MPI_Comm comm;
    MPI_Status status;
    MPI_Request req;
    static int bufsizes[4] = { 1, 100, 10000, 1000000 };
    char *buf;
    int cs, flag, n;

    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );
    source = 0;
    dest = size - 1;

    for (cs=0; cs<4; cs++) {
        if (rank == 0) {
            n = bufsizes[cs];
            buf = (char *)malloc( n );
            if (!buf) {
                fprintf( stderr, "Unable to allocate %d bytes\n", n);
		fflush(stderr);
                MPI_Abort( MPI_COMM_WORLD, 1 );
            }
	    
            MPI_Isend( buf, n, MPI_CHAR, dest, cs+n+1, comm, &req );
            MPI_Cancel( &req );
            MPI_Wait( &req, &status );
            MPI_Test_cancelled( &status, &flag );

	    if (!flag) {
                errs ++;
                printf( "Failed to cancel a Isend request\n" );
                fflush(stdout);
            }
            else
            {
                n = 0;
            }

	    /* Send the size, zero for successfully cancelled */
            MPI_Send( &n, 1, MPI_INT, dest, 123, comm );
            /* Send the tag so the message can be received */
            n = cs+n+1;
            MPI_Send( &n, 1, MPI_INT, dest, 123, comm );
            free( buf );
        }
        else if (rank == dest)
        {
            int n, tag;
            char *btemp;
            MPI_Recv( &n, 1, MPI_INT, 0, 123, comm, &status );
            MPI_Recv( &tag, 1, MPI_INT, 0, 123, comm, &status );
            if (n > 0)
            {
                /* If the message was not cancelled, receive it here */
                btemp = (char*)malloc( n );
                if (!btemp)
                {
                    fprintf( stderr, "Unable to allocate %d bytes\n",n);
		    fflush(stderr);
                    MPI_Abort( MPI_COMM_WORLD, 1 );
                }
                MPI_Recv( btemp, n, MPI_CHAR, 0, tag, comm, &status );
                free(btemp);
            }
        }
	
        MPI_Barrier( comm );
        if (rank == 0) {
            char *bsendbuf;
            int bsendbufsize;
            int bf, bs;
            n = bufsizes[cs];
            buf = (char *)malloc( n );
            if (!buf) {
                fprintf( stderr, "Unable to allocate %d bytes\n", n);
		fflush(stderr);
                MPI_Abort( MPI_COMM_WORLD, 1 );
            }
	    
            bsendbufsize = n + MPI_BSEND_OVERHEAD;
            bsendbuf = (char *)malloc( bsendbufsize );

	    if (!bsendbuf) {
                fprintf( stderr, "Unable to allocate %d bytes for bsend\n", n );
		fflush(stderr);
                MPI_Abort( MPI_COMM_WORLD, 1 );
            }
	    
            MPI_Buffer_attach( bsendbuf, bsendbufsize );
            MPI_Ibsend( buf, n, MPI_CHAR, dest, cs+n+2, comm, &req );
            MPI_Cancel( &req );
            MPI_Wait( &req, &status );
            MPI_Test_cancelled( &status, &flag );

	    if (!flag) {
                errs ++;
                printf( "Failed to cancel a Ibsend request\n" );
                fflush(stdout);
            }
            else
            {
                n = 0;
            }

	    /* Send the size, zero for successfully cancelled */
            MPI_Send( &n, 1, MPI_INT, dest, 123, comm );
            /* Send the tag so the message can be received */
            n = cs+n+2;
            MPI_Send( &n, 1, MPI_INT, dest, 123, comm );
            free( buf );
            MPI_Buffer_detach( &bf, &bs );
            free( bsendbuf );
        }
        else if (rank == dest)
        {
            int n, tag;
            char *btemp;
            MPI_Recv( &n, 1, MPI_INT, 0, 123, comm, &status );
            MPI_Recv( &tag, 1, MPI_INT, 0, 123, comm, &status );
            if (n > 0)
            {
                /* If the message was not cancelled, receive it here */
                btemp = (char*)malloc( n );
                if (!btemp)
                {
                    fprintf( stderr, "Unable to allocate %d bytes\n",n);
		    fflush(stderr);
                    MPI_Abort( MPI_COMM_WORLD, 1 );
                }
                MPI_Recv( btemp, n, MPI_CHAR, 0, tag, comm, &status );
                free(btemp);
            }
        }
        MPI_Barrier( comm );
        if (rank == 0) {
            n = bufsizes[cs];
            buf = (char *)malloc( n );
            if (!buf) {
                fprintf( stderr, "Unable to allocate %d bytes\n", n);
		fflush(stderr);
                MPI_Abort( MPI_COMM_WORLD, 1 );
            }
	    
            MPI_Issend( buf, n, MPI_CHAR, dest, cs+n+4, comm, &req );
            MPI_Cancel( &req );
            MPI_Wait( &req, &status );
            MPI_Test_cancelled( &status, &flag );
            if (!flag) {
                errs ++;
                printf( "Failed to cancel a Issend request\n" );
                fflush(stdout);
            }
            else
            {
                n = 0;
            }

	    /* Send the size, zero for successfully cancelled */
            MPI_Send( &n, 1, MPI_INT, dest, 123, comm );
            /* Send the tag so the message can be received */
            n = cs+n+4;
            MPI_Send( &n, 1, MPI_INT, dest, 123, comm );
            free( buf );
        }
        else if (rank == dest)
        {
            int n, tag;
            char *btemp;
            MPI_Recv( &n, 1, MPI_INT, 0, 123, comm, &status );
            MPI_Recv( &tag, 1, MPI_INT, 0, 123, comm, &status );
            if (n > 0)
            {
                /* If the message was not cancelled, receive it here */
                btemp = (char*)malloc( n );
                if (!btemp)
                {
                    fprintf( stderr, "Unable to allocate %d bytes\n", n);
		    fflush(stderr);
                    MPI_Abort( MPI_COMM_WORLD, 1 );
                }
                MPI_Recv( btemp, n, MPI_CHAR, 0, tag, comm, &status );
                free(btemp);
            }
        }
        MPI_Barrier( comm );
    }
    MPI_Finalize();
    return 0;
}

