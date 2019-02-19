/* 
MPI_Grequest_complete

   Notify MPI that a user-defined request is complete
int MPI_Grequest_complete(
  MPI_Request request
);

Parameters

   request
          [in] Generalized request to mark as complete

Remarks

   The call informs MPI that the operations represented by the generalized
   request request are complete. A call to MPI_WAIT(request, status) will
   return and a call to MPI_TEST(request, flag, status) will return
   flag=true only after a call to MPI_GREQUEST_COMPLETE has declared that
   these operations are complete.

   MPI imposes no restrictions on the code executed by the callback
   functions. However, new nonblocking operations should be defined so
   that the general semantic rules about MPI calls such as MPI_TEST,
   MPI_REQUEST_FREE, or MPI_CANCEL still hold. For example, all these
   calls are supposed to be local and nonblocking. Therefore, the callback
   functions query_fn, free_fn, or cancel_fn should invoke blocking MPI
   communication calls only if the context is such that these calls are
   guaranteed to return in finite time. Once MPI_CANCEL is invoked, the
   cancelled operation should complete in finite time, irrespective of the
   state of other processes (the operation has acquired "local"
   semantics). It should either succeed, or fail without side-effects. The
   user should guarantee these same properties for newly defined
   operations.

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

See Also

   MPI_Grequest_start

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

/* Simple test of generalized requests */

int query_fn( void *extra_state, MPI_Status *status )
{
    /* Set a default status */
    status->MPI_SOURCE = MPI_UNDEFINED;
    status->MPI_TAG = MPI_UNDEFINED;
    MPI_Status_set_cancelled( status, 0 );
    MPI_Status_set_elements( status, MPI_BYTE, 0 );
    return 0;
}

int free_fn( void *extra_state )
{
    int *b = (int *)extra_state;
    if (b) *b = *b - 1;
    /* The value returned by the free function is the error code
       returned by the wait/test function */
    return 0;
}

int cancel_fn( void *extra_state, int complete )
{
    return 0;
}

/*
* This is a very simple test of generalized requests. Normally, the
* MPI_Grequest_complete function would be called from another routine,
* often running in a separate thread. This simple code allows us to
* check that requests can be created, tested, and waited on in the
* case where the request is complete before the wait is called.
*
* Note that MPI did *not* define a routine that can be called within
* test or wait to advance the state of a generalized request.
* Most uses of generalized requests will need to use a separate thread.
*/
int main( int argc, char *argv[] )
{
    int errs = 0;
    int counter, flag;
    MPI_Status status;
    MPI_Request request;

    MPI_Init( &argc, &argv );

    MPI_Grequest_start( query_fn, free_fn, cancel_fn, NULL, &request );
    MPI_Test( &request, &flag, &status );
    if (flag) {
        errs++;
        fprintf( stderr, "Generalized request marked as complete\n" );
    }

    MPI_Grequest_complete( request );

    MPI_Wait( &request, &status );

    counter = 1;
    MPI_Grequest_start( query_fn, free_fn, cancel_fn, &counter, &request );
    MPI_Grequest_complete( request );
    MPI_Wait( &request, MPI_STATUS_IGNORE );
    if (counter) {
        errs++;
        fprintf( stderr, "Free routine not called, or not called with extra_data" );
	fflush(stderr);
    }

    MPI_Finalize();
    return 0;
}

