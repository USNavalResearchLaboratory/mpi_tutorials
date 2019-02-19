/* 
MPI_Comm_free

   Marks the communicator object for deallocation
int MPI_Comm_free(
  MPI_Comm *comm
);

Parameters

   comm
          [in] Communicator to be destroyed (handle)

Remarks

   This routine frees a communicator. Because the communicator may still
   be in use by other MPI routines, the actual communicator storage will
   not be freed until all references to this communicator are removed. For
   most users, the effect of this routine is the same as if it was in fact
   freed at this time of this call.

   This collective operation marks the communication object for
   deallocation. The handle is set to MPI_COMM_NULL. Any pending
   operations that use this communicator will complete normally; the
   object is actually deallocated only if there are no other active
   references to it. This call applies to intra- and inter-communicators.
   The delete callback functions for all cached attributes are called in
   arbitrary order.

   Null Handles
   The MPI 1.1 specification, in the section on opaque objects, explicitly
   disallows freeing a null communicator. The text from the standard is
 A null handle argument is an erroneous IN argument in MPI calls, unless an
 exception is explicitly stated in the text that defines the function. Such
 exception is allowed for handles to request objects in Wait and Test calls
 (sections Communication Completion and Multiple Completions ). Otherwise, a
 null handle can only be passed to a function that allocates a new object and
 returns a reference to it in the handle.

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
/* Test that communicators have reference count semantics */
#define NELM 128
#define NCOMM 1020
int main( int argc, char *argv[] )
{
    int errs = 0;
    int rank, size, source, dest, i;
    MPI_Comm comm;
    MPI_Comm tmpComm[NCOMM];
    MPI_Status status;
    MPI_Request req;
    int *buf=0;
    MPI_Init( &argc, &argv );
    MPI_Comm_dup( MPI_COMM_WORLD, &comm );
    /* This is similar to the datatype test, except that we post
        an irecv on a simple data buffer but use a rank-reordered
communicator.
        In this case, an error in handling the reference count will
most
        likely cause the program to hang, so this should be run only
        if (a) you are confident that the code is correct or (b)
        a timeout is set for mpiexec
    */
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );
    if (size < 2) {
        fprintf( stderr, "This test requires at least two processes."
);fflush(stderr);
        MPI_Abort( MPI_COMM_WORLD, 1 );
    }
    source = 0;
    dest = size - 1;
    if (rank == dest) {
        buf = (int *)malloc( NELM * sizeof(int) );
        for (i=0; i<NELM; i++) buf[i] = -i;
        MPI_Irecv( buf, NELM, MPI_INT, source, 0, comm, &req );
        MPI_Comm_free( &comm );
        if (comm != MPI_COMM_NULL) {
            errs++;
            printf( "Freed comm was not set to COMM_NULL\n" );
        }
        for (i=0; i<NCOMM; i++) {
            MPI_Comm_split( MPI_COMM_WORLD, 0, size - rank, &tmpComm[i]
);
        }
        MPI_Sendrecv( 0, 0, MPI_INT, source, 1, 0, 0, MPI_INT, source,
1, MPI_COMM_WORLD, &status );
        MPI_Wait( &req, &status );
        for (i=0; i<NELM; i++) {
            if (buf[i] != i) {
                errs++;
                if (errs < 10) {
                    printf( "buf[%d] = %d, expected %d\n", i, buf[i], i
);
                }
            }
        }
        for (i=0; i<NCOMM; i++) {
            MPI_Comm_free( &tmpComm[i] );
        }
        free( buf );
    }
    else if (rank == source) {
        buf = (int *)malloc( NELM * sizeof(int) );
        for (i=0; i<NELM; i++) buf[i] = i;
        for (i=0; i<NCOMM; i++) {
            MPI_Comm_split( MPI_COMM_WORLD, 0, size - rank, &tmpComm[i]
);
        }
        /* Synchronize with the receiver */
        MPI_Sendrecv( 0, 0, MPI_INT, dest, 1, 0, 0, MPI_INT, dest, 1,
MPI_COMM_WORLD, &status );
        MPI_Send( buf, NELM, MPI_INT, dest, 0, comm );
        free( buf );
    }
    else {
        for (i=0; i<NCOMM; i++) {
            MPI_Comm_split( MPI_COMM_WORLD, 0, size - rank, &tmpComm[i]
);
        }
    }
    MPI_Barrier( MPI_COMM_WORLD );
    if (rank != dest) {
        /* Clean up the communicators */
        for (i=0; i<NCOMM; i++) {
            MPI_Comm_free( &tmpComm[i] );
        }
    }
    MPI_Finalize();
    return 0;
}

