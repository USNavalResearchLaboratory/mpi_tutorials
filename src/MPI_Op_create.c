/* 
MPI_Op_create

   Creates a user-defined combination function handle
int MPI_Op_create(
  MPI_User_function *function,
  int commute,
  MPI_Op *op
);

Parameters

   function
          [in] user defined function (function)

   commute
          [in] true if commutative; false otherwise. (logical)

   op
          [out] operation (handle)

Remarks

   MPI_OP_CREATE binds a user-defined global operation to an op handle
   that can subsequently be used in MPI_REDUCE, MPI_ALLREDUCE,
   MPI_REDUCE_SCATTER, and MPI_SCAN. The user-defined operation is assumed
   to be associative. If commute = true, then the operation should be both
   commutative and associative. If commute = false, then the order of
   operands is fixed and is defined to be in ascending, process rank
   order, beginning with process zero. The order of evaluation can be
   changed, talking advantage of the associativity of the operation. If
   commute = true then the order of evaluation can be changed, taking
   advantage of commutativity and associativity.

   function is the user-defined function, which must have the following
   four arguments: invec, inoutvec, len and datatype.

   The ANSI-C prototype for the function is the following.

typedef void MPI_User_function( void *invec, void *inoutvec, int *len,
                                                MPI_Datatype *datatype);

   The Fortran declaration of the user-defined function appears below.

FUNCTION USER_FUNCTION( INVEC(*), INOUTVEC(*), LEN, TYPE)
<type> INVEC(LEN), INOUTVEC(LEN)
 INTEGER LEN, TYPE

   The datatype argument is a handle to the data type that was passed into
   the call to MPI_REDUCE. The user reduce function should be written such
   that the following holds: Let u[0], ... , u[len-1] be the len elements
   in the communication buffer described by the arguments invec, len and
   datatype when the function is invoked; let v[0], ... , v[len-1] be len
   elements in the communication buffer described by the arguments
   inoutvec, len and datatype when the function is invoked; let w[0], ...
   , w[len-1] be len elements in the communication buffer described by the
   arguments inoutvec, len and datatype when the function returns; then
   w[i] = u[i] [img133.gif] v[i], for i=0 , ... , len-1, where
   [img134.gif] is the reduce operation that the function computes.

   Informally, we can think of invec and inoutvec as arrays of len
   elements that function is combining. The result of the reduction
   over-writes values in inoutvec, hence the name. Each invocation of the
   function results in the pointwise evaluation of the reduce operator on
   len elements: I.e, the function returns in inoutvec[i] the value
   [img135.gif] , for [img136.gif] , where [img137.gif] is the combining
   operation computed by the function.

   [] Rationale.

   The len argument allows MPI_REDUCE to avoid calling the function for
   each element in the input buffer. Rather, the system can choose to
   apply the function to chunks of input. In C, it is passed in as a
   reference for reasons of compatibility with Fortran.

   By internally comparing the value of the datatype argument to known,
   global handles, it is possible to overload the use of a single
   user-defined function for several, different data types. ( End of
   rationale.)
   General datatypes may be passed to the user function. However, use of
   datatypes that are not contiguous is likely to lead to inefficiencies.

   No MPI communication function may be called inside the user function.
   MPI_ABORT may be called inside the function in case of an error.

   Advice to users.

   Suppose one defines a library of user-defined reduce functions that are
   overloaded: the datatype argument is used to select the right execution
   path at each invocation, according to the types of the operands. The
   user-defined reduce function cannot "decode" the datatype argument that
   it is passed, and cannot identify, by itself, the correspondence
   between the datatype handles and the datatype they represent. This
   correspondence was established when the datatypes were created. Before
   the library is used, a library initialization preamble must be
   executed. This preamble code will define the datatypes that are used by
   the library, and store handles to these datatypes in global, static
   variables that are shared by the user code and the library code.

   The Fortran version of MPI_REDUCE will invoke a user-defined reduce
   function using the Fortran calling conventions and will pass a
   Fortran-type datatype argument; the C version will use C calling
   convention and the C representation of a datatype handle. Users who
   plan to mix languages should define their reduction functions
   accordingly.

Notes on the user function

   The calling list for the user function type is
 typedef void (MPI_User_function) ( void * a, void * b, int * len, MPI_Datatype
* );

   where the operation is b[i] = a[i] op b[i], for i=0,...,len-1. A
   pointer to the datatype given to the MPI collective computation routine
   (i.e., MPI_Reduce, MPI_Allreduce, MPI_Scan, or MPI_Reduce_scatter) is
   also passed to the user-specified routine.

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

See Also

   MPI_Op_free

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

void addem ( int *, int *, int *, MPI_Datatype * );

void addem(int *invec, int *inoutvec, int *len, MPI_Datatype *dtype)
{
    int i;
    for ( i=0; i<*len; i++ )
        inoutvec[i] += invec[i];
}

int main( int argc, char **argv )
{
    int rank, size, i;
    int data;
    int errors=0;
    int result = -100;
    int correct_result;
    MPI_Op op;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    data = rank;
    MPI_Op_create( (MPI_User_function *)addem, 1, &op );
    MPI_Reduce ( &data, &result, 1, MPI_INT, op, 0, MPI_COMM_WORLD );
    MPI_Bcast ( &result, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Op_free( &op );
    correct_result = 0;
    for(i=0;i<size;i++)
        correct_result += i;
    if (result != correct_result) errors++;

    MPI_Finalize();
    return errors;
}

