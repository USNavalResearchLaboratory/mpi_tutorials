/* 
MPI_Buffer_detach

   Removes an existing buffer (for use in MPI_Bsend etc)
int MPI_Buffer_detach(
  void *buffer,
  int *size
);

Parameters

   buffer
          [out] initial buffer address (choice)

   size
          [out] buffer size, in bytes (integer)

Remarks

   Detach the buffer currently associated with MPI. The call returns the
   address and the size of the detached buffer. This operation will block
   until all messages currently in the buffer have been transmitted. Upon
   return of this function, the user may reuse or deallocate the space
   taken by the buffer.

   The reason that MPI_Buffer_detach returns the address and size of the
   buffer being detached is to allow nested libraries to replace and
   restore the buffer. For example, consider

    int size, mysize, idummy;
    void *ptr, *myptr, *dummy;
    MPI_Buffer_detach( &ptr, &size );
    MPI_Buffer_attach( myptr, mysize );
    ...
    ... library code ...
    ...
    MPI_Buffer_detach( &dummy, &idummy );
    MPI_Buffer_attach( ptr, size );

   This is much like the action of the Unix signal routine and has the
   same strengths (it is simple) and weaknesses (it only works for nested
   usages).

   Note that for this approach to work, MPI_Buffer_detach must return
   MPI_SUCCESS even when there is no buffer to detach. In that case, it
   returns a size of zero. The MPI 1.1 standard for MPI_BUFFER_DETACH
   contains the text

   The statements made in this section describe the behavior of MPI for
   buffered-mode sends. When no buffer is currently associated, MPI behaves
   as if a zero-sized buffer is associated with the process.

   This could be read as applying only to the various Bsend routines. This
   implementation takes the position that this applies to
   MPI_BUFFER_DETACH as well.

   Advice to users.

   Even though the C functions MPI_Buffer_attach and MPI_Buffer_detach
   both have a first argument of type void*, these arguments are used
   differently: A pointer to the buffer is passed to MPI_Buffer_attach;
   the address of the pointer is passed to MPI_Buffer_detach, so that this
   call can return the pointer value.
   Rationale.

   Both arguments are defined to be of type void* (rather than void* and
   void**, respectively), so as to avoid complex type casts. E.g., in the
   last example, &buff, which is of type char**, can be passed as argument
   to MPI_Buffer_detach without type casting. If the formal parameter had
   type void** then we would need a type cast before and after the call. (
   End of rationale.)

   The statements made in this section describe the behavior of MPI for
   buffered-mode sends. When no buffer is currently associated, MPI
   behaves as if a zero-sized buffer is associated with the process.

   MPI must provide as much buffering for outgoing messages as if outgoing
   message data were buffered by the sending process, in the specified
   buffer space, using a circular, contiguous-space allocation policy. We
   outline below a model implementation that defines this policy. MPI may
   provide more buffering, and may use a better buffer allocation
   algorithm than described below. On the other hand, MPI may signal an
   error whenever the simple buffering allocator described below would run
   out of space. In particular, if no buffer is explicitly associated with
   the process, then any buffered send may cause an error.

   MPI does not provide mechanisms for querying or controlling buffering
   done by standard mode sends. It is expected that vendors will provide
   such information for their implementations.

   Rationale.

   There is a wide spectrum of possible implementations of buffered
   communication: buffering can be done at sender, at receiver, or both;
   buffers can be dedicated to one sender-receiver pair, or be shared by
   all communications; buffering can be done in real or in virtual memory;
   it can use dedicated memory, or memory shared by other processes;
   buffer space may be allocated statically or be changed dynamically;
   etc. It does not seem feasible to provide a portable mechanism for
   querying or controlling buffering that would be compatible with all
   these choices, yet provide meaningful information.

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
   calls this routine or MPI_Buffer_attach.

Notes for Fortran

   All MPI routines in Fortran (except for MPI_WTIME and MPI_WTICK) have
   an additional argument ierr at the end of the argument list. ierr is an
   integer and has the same meaning as the return value of the routine in
   C. In Fortran, MPI routines are subroutines, and are invoked with the
   call statement.

   All MPI objects (e.g., MPI_Datatype, MPI_Comm) are of type INTEGER in
   Fortran.

   The Fortran binding for this routine is different. Because Fortran does
   not have pointers, it is impossible to provide a way to use the output
   of this routine to exchange buffers. In this case, only the size field
   is set.

Notes for C

   Even though the bufferptr argument is declared as void *, it is really
   the address of a void pointer. See the rationale in the standard for
   more details.

See Also

   MPI_Buffer_attach

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

