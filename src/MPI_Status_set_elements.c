/* 
MPI_Status_set_elements

   Set the number of elements in a status
int MPI_Status_set_elements(
  MPI_Status *status,
  MPI_Datatype datatype,
  int count
);

Parameters

   status
          [in] status to associate count with (Status)

   datatype
          [in] datatype associated with count (handle)

   count
          [in] number of elements to associate with status (integer)

Remarks

   This call modifies the opaque part of status so that a call to
   MPI_GET_ELEMENTS will return count. MPI_GET_COUNT will return a
   compatible value.

   Rationale.

   The number of elements is set instead of the count because the former
   can deal with nonintegral number of datatypes. ( End of rationale.)

   A subsequent call to MPI_GET_COUNT(status, datatype, count) or to
   MPI_GET_ELEMENTS(status, datatype, count) must use a datatype argument
   that has the same type signature as the datatype argument that was used
   in the call to MPI_STATUS_SET_ELEMENTS.

   Rationale.

   This is similar to the restriction that holds when when count is set by
   a receive operation: in that case, the calls to MPI_GET_COUNT and
   MPI_GET_ELEMENTS must use a datatype with the same signature as the
   datatype used in the receive call.

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

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

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
