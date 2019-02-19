/* 
MPI_Bsend

   Basic send with user-provided buffering
int MPI_Bsend(
  void *buf,
  int count,
  MPI_Datatype datatype,
  int dest,
  int tag,
  MPI_Comm comm
);

Parameters

   buf
          [in] initial address of send buffer (choice)

   count
          [in] number of elements in send buffer (nonnegative integer)

   datatype
          [in] datatype of each send buffer element (handle)

   dest
          [in] rank of destination (integer)

   tag
          [in] message tag (integer)

   comm
          [in] communicator (handle)

Remarks

   This send is provided as a convenience function; it allows the user to
   send messages without worring about where they are buffered (because
   the user must have provided buffer space with MPI_Buffer_attach).

   In deciding how much buffer space to allocate, remember that the buffer
   space is not available for reuse by subsequent MPI_Bsends unless you
   are certain that the message has been received (not just that it should
   have been received). For example, this code does not allocate enough
   buffer space
    MPI_Buffer_attach( b, n*sizeof(double) + MPI_BSEND_OVERHEAD );
    for (i=0; i<m; i++) {
        MPI_Bsend( buf, n, MPI_DOUBLE, ... );
    }

   because only enough buffer space is provided for a single send, and the
   loop may start a second MPI_Bsend before the first is done making use
   of the buffer.

   In C, you can force the messages to be delivered by
    MPI_Buffer_detach( &b, &n );
    MPI_Buffer_attach( b, n );

   (The MPI_Buffer_detach will not complete until all buffered messages
   are delivered.)

   A buffered mode send operation can be started whether or not a matching
   receive has been posted. It may complete before a matching receive is
   posted. However, unlike the standard send, this operation is local, and
   its completion does not depend on the occurrence of a matching receive.
   Thus, if a send is executed and no matching receive is posted, then MPI
   must buffer the outgoing message, so as to allow the send call to
   complete. An error will occur if there is insufficient buffer space.
   The amount of available buffer space is controlled by the user. Buffer
   allocation by the user may be required for the buffered mode to be
   effective.

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

   MPI_ERR_RANK
          Invalid source or destination rank. Ranks must be between zero
          and the size of the communicator minus one; ranks in a receive
          (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be
          MPI_ANY_SOURCE.

   MPI_ERR_TAG
          Invalid tag argument. Tags must be non-negative; tags in a
          receive (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be
          MPI_ANY_TAG. The largest tag value is available through the the
          attribute MPI_TAG_UB.

See Also

   MPI_Buffer_attach, MPI_Ibsend, MPI_Bsend_init

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* This is a simple program that tests bsend. It may be run as a single
* process; in addition, bsend allows send-to-self programs.
*/
int main( int argc, char *argv[] )
{
    MPI_Comm comm = MPI_COMM_WORLD;
    int dest = 0, src = 0, tag = 1;
    int s1, s2, s3;
    char *buf, *bbuf;
    char msg1[7], msg3[17];
    double msg2[2];
    char rmsg1[64], rmsg3[64];
    double rmsg2[64];
    int errs = 0, rank;
    int bufsize, bsize;
    
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    /* According to the standard, we must use the PACK_SIZE length of each
        message in the computation of the message buffer size */
    MPI_Pack_size( 7, MPI_CHAR, comm, &s1 );
    MPI_Pack_size( 2, MPI_DOUBLE, comm, &s2 );
    MPI_Pack_size( 17, MPI_CHAR, comm, &s3 );
    bufsize = 3 * MPI_BSEND_OVERHEAD + s1 + s2 + s3;
    buf = (char *)malloc( bufsize );
    MPI_Buffer_attach( buf, bufsize );
    strncpy( msg1, "012345", 7 );
    strncpy( msg3, "0123401234012341", 17 );
    msg2[0] = 1.23; msg2[1] = 3.21;

    if (rank == src) {
        /* These message sizes are chosen to expose any alignment problems */
        MPI_Bsend( msg1, 7, MPI_CHAR, dest, tag, comm );
        MPI_Bsend( msg2, 2, MPI_DOUBLE, dest, tag, comm );
        MPI_Bsend( msg3, 17, MPI_CHAR, dest, tag, comm );
    }
    if (rank == dest) {
        MPI_Recv( rmsg1, 7, MPI_CHAR, src, tag, comm, MPI_STATUS_IGNORE);
        MPI_Recv( rmsg2, 10, MPI_DOUBLE, src, tag, comm, MPI_STATUS_IGNORE );
        MPI_Recv( rmsg3, 17, MPI_CHAR, src, tag, comm, MPI_STATUS_IGNORE );
        if (strcmp( rmsg1, msg1 ) != 0) {
            errs++;
            fprintf( stderr, "message 1 (%s) should be %s\n", rmsg1, msg1 );
	    fflush(stderr);
        }
        if (rmsg2[0] != msg2[0] || rmsg2[1] != msg2[1]) {
            errs++;
            fprintf( stderr, "message 2 incorrect, values are (%f,%f) but should be (%f,%f)\n",
                     rmsg2[0], rmsg2[1], msg2[0], msg2[1]);
	    fflush(stderr);
        }
        if (strcmp( rmsg3, msg3 ) != 0) {
            errs++;
            fprintf( stderr, "message 3 (%s) should be %s\n", rmsg3, msg3 );
	    fflush(stderr);
        }
    }
    
    /* We can't guarantee that messages arrive until the detach */
    MPI_Buffer_detach( &bbuf, &bsize );
    //MPI_Finalize( errs );
    MPI_Finalize();
    return 0;
}

