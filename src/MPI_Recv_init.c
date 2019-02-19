/* 
MPI_Recv_init

   Create a persistent request for a receive
int MPI_Recv_init(
  void *buf,
  int count,
  MPI_Datatype datatype,
  int source,
  int tag,
  MPI_Comm comm,
  MPI_Request *request
);

Parameters

   buf
          [in] initial address of receive buffer (choice)

   count
          [in] number of elements received (integer)

   datatype
          [in] type of each element (handle)

   source
          [in] rank of source or MPI_ANY_SOURCE (integer)

   tag
          [in] message tag or MPI_ANY_TAG (integer)

   comm
          [in] communicator (handle)

   request
          [out] communication request (handle)

Remarks

   Creates a persistent communication request for a receive operation. The
   argument buf is marked as OUT because the user gives permission to
   write on the receive buffer by passing the argument to MPI_RECV_INIT.

   A persistent communication request is inactive after it was created ---
   no active communication is attached to the request.

   A communication (send or receive) that uses a persistent request is
   initiated by the function MPI_START.

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

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

   MPI_ERR_INTERN
          This error is returned when some part of the MPICH
          implementation is unable to acquire memory.

See Also

   MPI_Start, MPI_Startall, MPI_Request_free

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
        MPI_Request *rr = (MPI_Request *)malloc(size * sizeof(MPI_Request));
        for (i=0; i<size; i++) {
            MPI_Irecv( rbuf, 10, MPI_INT, i, tag, MPI_COMM_WORLD, &rr[i] );
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
        MPI_Request rr;
        /* Create a persistent receive request */
        MPI_Recv_init( rbuf, 10, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &r );
        MPI_Isend( buf, 10, MPI_INT, 0, tag, MPI_COMM_WORLD, &rr );
        for (i=0; i<size; i++) {
            MPI_Start( &r );
            MPI_Wait( &r, &s );
        }
        MPI_Wait( &rr, &s );
        MPI_Request_free( &r );
    }
    else {
        MPI_Send( buf, 10, MPI_INT, 0, tag, MPI_COMM_WORLD );
    }

    MPI_Finalize();
    return 0;
}

