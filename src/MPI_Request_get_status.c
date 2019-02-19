/* 
MPI_Request_get_status

   Nondestructive test for the completion of a Request
int MPI_Request_get_status(
  MPI_Request request,
  int *flag,
  MPI_Status *status
);

Parameters

   request
          [in] request (handle)

   flag
          [out] true if operation has completed (logical)

   status
          [out] status object (Status). May be MPI_STATUS_IGNORE.

Remarks

   This call is useful for accessing the information associated with a
   request, without freeing the request (in case the user is expected to
   access it later). It allows one to layer libraries more conveniently,
   since multiple layers of software may access the same completed request
   and extract from it the status information.

   Sets flag=true if the operation is complete, and, if so, returns in
   status the request status. It sets flag=false if the operation is not
   complete.

   Unlike MPI_Test, MPI_Request_get_status does not deallocate or
   deactivate the request. A call to one of the test/wait routines or
   MPI_Request_free should be made to release the request object.

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

/* Test Request_get_status */

int main( int argc, char *argv[] )
{
    int errs = 0;
    int rank, size, source, dest;
    int buf[2], flag, count;
    MPI_Comm comm;
    MPI_Status status, status2;
    MPI_Request req;

    MPI_Init( &argc, &argv );

    comm = MPI_COMM_WORLD;
    /* Determine the sender and receiver */
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );
    source = 0;
    dest = size - 1;

    if (rank == source) {
        buf[0] = size;
        buf[1] = 3;
        MPI_Ssend( buf, 2, MPI_INT, dest, 10, comm );
    }
    if (rank == dest) {
        MPI_Irecv( buf, 2, MPI_INT, source, 10, comm, &req );
    }
    MPI_Barrier( comm );
    /* At this point, we know that the receive has at least started,
because of the Ssend. Check the status on the request */
    if (rank == dest) {
        status.MPI_SOURCE = -1;
        status.MPI_TAG = -1;
        MPI_Request_get_status( req, &flag, &status );
        if (flag) {
            if (status.MPI_TAG != 10) {
                errs++;
                fprintf( stderr, "Tag value %d should be 10\n",
status.MPI_TAG );fflush(stderr);
            }
            if (status.MPI_SOURCE != source) {
                errs++;
                fprintf( stderr, "Source value %d should be %d\n",
status.MPI_SOURCE, source );fflush(stderr);
            }
            MPI_Get_count( &status, MPI_INT, &count );
            if (count != 2) {
                errs++;
                fprintf( stderr, "Count value %d should be 2\n", count
);fflush(stderr);
            }
        }
        else {
            errs++;
            fprintf( stderr, "Unexpected flag value from get_status\n"
);fflush(stderr);
        }
        /* Now, complete the request */
        MPI_Wait( &req, &status2 );
        /* Check that the status is correct */
        if (status2.MPI_TAG != 10) {
            errs++;
            fprintf( stderr, "(wait)Tag value %d should be 10\n",
status2.MPI_TAG );fflush(stderr);
        }
        if (status2.MPI_SOURCE != source) {
            errs++;
            fprintf( stderr, "(wait)Source value %d should be %d\n",
status2.MPI_SOURCE, source );fflush(stderr);
        }
        MPI_Get_count( &status2, MPI_INT, &count );
        if (count != 2) {
            errs++;
            fprintf( stderr, "(wait)Count value %d should be 2\n",
count );fflush(stderr);
        }
    }

    MPI_Finalize();
    return errs;
}

