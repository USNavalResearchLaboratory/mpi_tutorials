/* 
MPI_Scan

   Computes the scan (partial reductions) of data on a collection of
   processes
int MPI_Scan(
  void *sendbuf,
  void *recvbuf,
  int count,
  MPI_Datatype datatype,
  MPI_Op op,
  MPI_Comm comm
);

Parameters

   sendbuf
          [in] starting address of send buffer (choice)

   recvbuf
          [out] starting address of receive buffer (choice)

   count
          [in] number of elements in input buffer (integer)

   datatype
          [in] data type of elements of input buffer (handle)

   op
          [in] operation (handle)

   comm
          [in] communicator (handle)

Remarks

   MPI_SCAN is used to perform a prefix reduction on data distributed
   across the group. The operation returns, in the receive buffer of the
   process with rank i, the reduction of the values in the send buffers of
   processes with ranks 0,...,i (inclusive). The type of operations
   supported, their semantics, and the constraints on send and receive
   buffers are as for MPI_REDUCE.

   Rationale.

   We have defined an inclusive scan, that is, the prefix reduction on
   process i includes the data from process i. An alternative is to define
   scan in an exclusive manner, where the result on i only includes data
   up to i-1. Both definitions are useful. The latter has some advantages:
   the inclusive scan can always be computed from the exclusive scan with
   no additional communication; for non-invertible operations such as max
   and min, communication is required to compute the exclusive scan from
   the inclusive scan. There is, however, a complication with exclusive
   scan since one must define the "unit" element for the reduction in this
   case. That is, one must explicitly say what occurs for process 0. This
   was thought to be complex for user-defined operations and hence, the
   exclusive scan was dropped.

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

Notes on collective operations

   The reduction functions (MPI_Op) do not return an error value. As a
   result, if the functions detect an error, all they can do is either
   call MPI_Abort or silently skip the problem. Thus, if you change the
   error handler from MPI_ERRORS_ARE_FATAL to something else, for example,
   MPI_ERRORS_RETURN, then no error may be indicated.

   The reason for this is the performance problems in ensuring that all
   collective routines return the same error value.

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

   MPI_ERR_BUFFER
          Invalid buffer pointer. Usually a null buffer where one is not
          valid.

   MPI_ERR_BUFFER
          This error class is associcated with an error code that
          indicates that two buffer arguments are aliased; that is, the
          describe overlapping storage (often the exact same storage).
          This is prohibited in MPI (because it is prohibited by the
          Fortran standard, and rather than have a separate case for C and
          Fortran, the MPI Forum adopted the more restrictive requirements
          of Fortran).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

void addem ( int *, int *, int *, MPI_Datatype * );
void assoc ( int *, int *, int *, MPI_Datatype * );

void addem( int *invec, int *inoutvec, int *len, MPI_Datatype *dtype)
{
    int i;
    for ( i=0; i<*len; i++ )
    inoutvec[i] += invec[i];
}

#define BAD_ANSWER 100000
/*
    The operation is inoutvec[i] = invec[i] op inoutvec[i]
    (see 4.9.4). The order is important.
    Note that the computation is in process rank (in the communicator)
order, independant of the root.
*/

void assoc( int *invec, int *inoutvec, int *len, MPI_Datatype *dtype)
{
    int i;
    for ( i=0; i<*len; i++ ) {
        if (inoutvec[i] <= invec[i] ) {
            int rank;
            MPI_Comm_rank( MPI_COMM_WORLD, &rank );
            fprintf( stderr, "[%d] inout[0] = %d, in[0] = %d\n", rank, inoutvec[0], invec[0] );
	    fflush(stderr);
            inoutvec[i] = BAD_ANSWER;
        }
        else
            inoutvec[i] = invec[i];
    }
}

int main( int argc, char **argv )
{
    int rank, size, i;
    int data;
    int errors=0;
    int result = -100;
    int correct_result;
    MPI_Op op_assoc, op_addem;
    MPI_Comm comm=MPI_COMM_WORLD;

    MPI_Init( &argc, &argv );
    MPI_Op_create( (MPI_User_function *)assoc, 0, &op_assoc );
    MPI_Op_create( (MPI_User_function *)addem, 1, &op_addem );

    /* Run this for a variety of communicator sizes */

    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );

    data = rank;
    correct_result = 0;
    for (i=0;i<=rank;i++)
        correct_result += i;
    MPI_Scan ( &data, &result, 1, MPI_INT, MPI_SUM, comm );
    if (result != correct_result) {
        fprintf( stderr, "[%d] Error suming ints with scan\n", rank);
	fflush(stderr);
        errors++;
    }
    MPI_Scan ( &data, &result, 1, MPI_INT, MPI_SUM, comm );
    if (result != correct_result) {
        fprintf( stderr, "[%d] Error summing ints with scan (2)\n",rank );
	fflush(stderr);
        errors++;
    }
    data = rank;
    result = -100;
    MPI_Scan ( &data, &result, 1, MPI_INT, op_addem, comm );
    if (result != correct_result) {
        fprintf( stderr, "[%d] Error summing ints with scan (userop)\n", rank );
	fflush(stderr);
        errors++;
    }
    MPI_Scan ( &data, &result, 1, MPI_INT, op_addem, comm );
    if (result != correct_result) {
        fprintf( stderr, "[%d] Error summing ints with scan (userop2)\n", rank );
	fflush(stderr);
        errors++;
    }
    result = -100;
    data = rank;
    MPI_Scan ( &data, &result, 1, MPI_INT, op_assoc, comm );
    if (result == BAD_ANSWER) {
        fprintf( stderr, "[%d] Error scanning with non-commutative op\n", rank );
	fflush(stderr);
        errors++;
    }

    MPI_Op_free( &op_assoc );
    MPI_Op_free( &op_addem );
    MPI_Finalize();
    return errors;
}

