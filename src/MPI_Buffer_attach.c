/* 
MPI_Buffer_attach

   Attaches a user-provided buffer for sending
int MPI_Buffer_attach(
  void *buffer,
  int size
);

Parameters

   buffer
          [in] initial buffer address (choice)

   size
          [in] buffer size, in bytes (integer)

Remarks

   Provides to MPI a buffer in the user's memory to be used for buffering
   outgoing messages. The buffer is used only by messages sent in buffered
   mode. Only one buffer can be attached to a process at a time.

   The size given should be the sum of the sizes of all outstanding Bsends
   that you intend to have, plus MPI_BSEND_OVERHEAD for each Bsend that
   you do. For the purposes of calculating size, you should use
   MPI_Pack_size. In other words, in the code
     MPI_Buffer_attach( buffer, size );
     MPI_Bsend( ..., count=20, datatype=type1,  ... );
     ...
     MPI_Bsend( ..., count=40, datatype=type2, ... );

   the value of size in the MPI_Buffer_attach call should be greater than
   the value computed by
     MPI_Pack_size( 20, type1, comm, &s1 );
     MPI_Pack_size( 40, type2, comm, &s2 );
     size = s1 + s2 + 2 * MPI_BSEND_OVERHEAD;

   The MPI_BSEND_OVERHEAD gives the maximum amount of space that may be
   used in the buffer for use by the BSEND routines in using the buffer.
   This value is in mpi.h (for C) and mpif.h (for Fortran).

Thread and Interrupt Safety

   The user is responsible for ensuring that multiple threads do not try
   to update the same MPI object from different threads. This routine
   should not be used from within a signal handler.

   The MPI standard defined a thread-safe interface but this does not mean
   that all routines may be called without any thread locks. For example,
   two threads must not attempt to change the contents of the same
   MPI_Info object concurrently. The user is responsible in this case for
   using some mechanism, such as thread locks, to ensure that only one
   thread at a time makes use of this routine. Because the buffer for
   buffered sends (e.g., MPI_Bsend) is shared by all threads in a process,
   the user is responsible for ensuring that only one thread at a time
   calls this routine or MPI_Buffer_detach.

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

   MPI_ERR_BUFFER
          Invalid buffer pointer. Usually a null buffer where one is not
          valid.

   MPI_ERR_INTERN
          An internal error has been detected. This is fatal. Please send
          a bug report to mpi-bugs@mcs.anl.gov.

See Also

   MPI_Buffer_detach, MPI_Bsend

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include <stdio.h>
#include "mpi.h"
#define BUFSIZE 2000
int main( int argc, char *argv[] )
{
    MPI_Status status;
    MPI_Request request;
    int a[10], b[10];
    int buf[BUFSIZE], *bptr, bl, i, j, rank, size;
    int errs = 0;
    MPI_Init( 0, 0 );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Buffer_attach( buf, BUFSIZE );
    for (j=0; j<10; j++) {
        MPI_Bsend_init( a, 10, MPI_INT, 0, 27+j, MPI_COMM_WORLD,
&request );
        for (i=0; i<10; i++) {
            a[i] = (rank + 10 * j) * size + i;
        }
        MPI_Start( &request );
        MPI_Wait( &request, &status );
        MPI_Request_free( &request );
    }
    if (rank == 0) {
        for (i=0; i<size; i++) {
            for (j=0; j<10; j++) {
                int k;
                status.MPI_TAG = -10;
                status.MPI_SOURCE = -20;
                MPI_Recv( b, 10, MPI_INT, i, 27+j, MPI_COMM_WORLD,
&status );
                if (status.MPI_TAG != 27+j) {
                    errs++;
                    printf( "Wrong tag = %d\n", status.MPI_TAG
);fflush(stdout);
                }
                if (status.MPI_SOURCE != i) {
                    errs++;
                    printf( "Wrong source = %d\n", status.MPI_SOURCE
);fflush(stdout);
                }
                for (k=0; k<10; k++) {
                    if (b[k] != (i + 10 * j) * size + k) {
                        errs++;
                        printf( "received b[%d] = %d from %d tag %d\n",
                                k, b[k], i, 27+j );fflush(stdout);
                    }
                }
            }
        }
    }
    MPI_Buffer_detach( &bptr, &bl );
    MPI_Finalize();
    return 0;
}

