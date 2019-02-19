/* 
MPI_Grequest_start

   Create and return a user-defined request
int MPI_Grequest_start(
  MPI_Grequest_query_function *query_fn,
  MPI_Grequest_free_function *free_fn,
  MPI_Grequest_cancel_function *cancel_fn,
  void *extra_state,
  MPI_Request *request
);

Parameters

   query_fn
          [in] callback function invoked when request status is queried
          (function)

   free_fn
          [in] callback function invoked when request is freed (function)

   cancel_fn
          [in] callback function invoked when request is cancelled
          (function)

   extra_state
          [in] Extra state passed to the above functions.

   request
          [out] Generalized request (handle)

Remarks

   The call starts a generalized request and returns a handle to it in
   request.

   Advice to users.

   Note that a generalized request belongs, in C++, to the class
   MPI::Grequest, which is a derived class of MPI::Request. It is of the
   same type as regular requests, in C and Fortran. ( End of advice to
   users.)

   The syntax and meaning of the callback functions are listed below. All
   callback functions are passed the extra_state argument that was
   associated with the request by the starting call MPI_GREQUEST_START.
   This can be used to maintain user-defined state for the request. In C,
   the query function is

   typedef int MPI_Grequest_query_function(void *extra_state, MPI_Status
   *status);

   in Fortran

   SUBROUTINE GREQUEST_QUERY_FUNCTION(EXTRA_STATE, STATUS, IERROR)
   INTEGER STATUS(MPI_STATUS_SIZE), IERROR
   INTEGER(KIND=MPI_ADDRESS_KIND) EXTRA_STATE

   and in C++

   typedef int MPI::Grequest::Query_function(void* extra_state,
   MPI::Status& status);

   query_fn function computes the status that should be returned for the
   generalized request. The status also includes information about
   successful/unsuccessful cancellation of the request (result to be
   returned by MPI_TEST_CANCELLED).

   query_fn callback is invoked by the MPI_{WAIT|TEST}{ANY|SOME|ALL} call
   that completed the generalized request associated with this callback.
   The callback function is also invoked by calls to
   MPI_REQUEST_GET_STATUS, if the request is complete when the call
   occurs. In both cases, the callback is passed a reference to the
   corresponding status variable passed by the user to the MPI call; the
   status set by the callback function is returned by the MPI call. If the
   user provided MPI_STATUS_IGNORE or MPI_STATUSES_IGNORE to the MPI
   function that causes query_fn to be called, then MPI will pass a valid
   status object to query_fn, and this status will be ignored upon return
   of the callback function. Note that query_fn is invoked only after
   MPI_GREQUEST_COMPLETE is called on the request; it may be invoked
   several times for the same generalized request, e.g., if the user calls
   MPI_REQUEST_GET_STATUS several times for this request. Note also that a
   call to MPI_{WAIT|TEST}{SOME|ALL} may cause multiple invocations of
   query_fn callback functions, one for each generalized request that is
   completed by the MPI call. The order of these invocations is not
   specified by MPI.

   In C, the free function is

   typedef int MPI_Grequest_free_function(void *extra_state);

   and in Fortran

   SUBROUTINE GREQUEST_FREE_FUNCTION(EXTRA_STATE, IERROR)
   INTEGER IERROR
   INTEGER(KIND=MPI_ADDRESS_KIND) EXTRA_STATE

   and in C++

   typedef int MPI::Grequest::Free_function(void* extra_state);

   free_fn function is invoked to clean up user-allocated resources when
   the generalized request is freed.

   free_fn callback is invoked by the MPI_{WAIT|TEST}{ANY|SOME|ALL} call
   that completed the generalized request associated with this callback.
   free_fn is invoked after the call to query_fn for the same request.
   However, if the MPI call completed multiple generalized requests, the
   order in which free_fn callback functions are invoked is not specified
   by MPI.

   free_fn callback is also invoked for generalized requests that are
   freed by a call to MPI_REQUEST_FREE (no call to
   WAIT_{WAIT|TEST}{ANY|SOME|ALL} will occur for such a request). In this
   case, the callback function will be called either in the MPI call
   MPI_REQUEST_FREE(request), or in the MPI call
   MPI_GREQUEST_COMPLETE(request), whichever happens last. I.e., in this
   case the actual freeing code is executed as soon as both calls
   MPI_REQUEST_FREE and MPI_GREQUEST_COMPLETE have occurred. The request
   is not deallocated until after free_fn completes. Note that free_fn
   will be invoked only once per request by a correct program.

   Advice to users.

   Calling MPI_REQUEST_FREE(request) will cause the request handle to be
   set to MPI_REQUEST_NULL. This handle to the generalized request is no
   longer valid. However, user copies of this handle are valid until after
   free_fn completes since MPI does not deallocate the object until then.
   Since free_fn is not called until after MPI_GREQUEST_COMPLETE, the user
   copy of the handle can be used to make this call. Users should note
   that MPI will deallocate the object after free_fn executes. At this
   point, user copies of the request handle no longer point to a valid
   request. MPI will not set user copies to MPI_REQUEST_NULL in this case,
   so it is up to the user to avoid accessing this stale handle. This is a
   special case where MPI defers deallocating the object until a later
   time that is known by the user. ( End of advice to users.)

   In C, the cancel function is

   typedef int MPI_Grequest_cancel_function(void *extra_state, int
   complete);

   in Fortran

   SUBROUTINE GREQUEST_CANCEL_FUNCTION(EXTRA_STATE, COMPLETE, IERROR)
   INTEGER IERROR
   INTEGER(KIND=MPI_ADDRESS_KIND) EXTRA_STATE
   LOGICAL COMPLETE

   and in C++

   typedef int MPI::Grequest::Cancel_function(void* extra_state, bool
   complete);

   cancel_fn function is invoked to start the cancelation of a generalized
   request. It is called by MPI_REQUEST_CANCEL(request). MPI passes to the
   callback function complete=true if MPI_GREQUEST_COMPLETE was already
   called on the request, and complete=false otherwise.

   All callback functions return an error code. The code is passed back
   and dealt with as appropriate for the error code by the MPI function
   that invoked the callback function. For example, if error codes are
   returned then the error code returned by the callback function will be
   returned by the MPI function that invoked the callback function. In the
   case of MPI_{WAIT|TEST}{ANY} call that invokes both query_fn and
   free_fn, the MPI call will return the error code returned by the last
   callback, namely free_fn. If one or more of the requests in a call to
   MPI_{WAIT|TEST}{SOME|ALL} failed, then the MPI call will return
   MPI_ERR_IN_STATUS. In such a case, if the MPI call was passed an array
   of statuses, then MPI will return in each of the statuses that
   correspond to a completed generalized request the error code returned
   by the corresponding invocation of its free_fn callback function.
   However, if the MPI function was passed MPI_STATUSES_IGNORE, then the
   individual error codes returned by each callback functions will be
   lost.

   Advice to users.

   query_fn must not set the error field of status since query_fn may be
   called by MPI_WAIT or MPI_TEST, in which case the error field of status
   should not change. The MPI library knows the "context" in which
   query_fn is invoked and can decide correctly when to put in the error
   field of status the returned error code.

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

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

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
