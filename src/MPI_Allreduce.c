/* 
MPI_Allreduce

   Combines values from all processes and distributes the result back to
   all processes
int MPI_Allreduce(
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
          [in] number of elements in send buffer (integer)

   datatype
          [in] data type of elements of send buffer (handle)

   op
          [in] operation (handle)

   comm
          [in] communicator (handle)

Remarks

   MPI includes variants of each of the reduce operations where the result
   is returned to all processes in the group. MPI requires that all
   processes participating in these operations receive identical results.

   Same as MPI_REDUCE except that the result appears in the receive buffer
   of all the group members.

   The "in place" option for intracommunicators is specified by passing
   the value MPI_IN_PLACE to the argument sendbuf at the root. In such
   case, the input data is taken at each process from the receive buffer,
   where it will be replaced by the output data.

   If comm is an intercommunicator, then the result of the reduction of
   the data provided by processes in group A is stored at each process in
   group B, and vice versa. Both groups should provide the same count
   value.

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

   MPI_ERR_BUFFER
          Invalid buffer pointer. Usually a null buffer where one is not
          valid.

   MPI_ERR_COUNT
          Invalid count argument. Count arguments must be non-negative; a
          count of zero is often valid.

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

   MPI_ERR_OP
          Invalid operation. MPI operations (objects of type MPI_Op) must
          either be one of the predefined operations (e.g., MPI_SUM) or
          created with MPI_Op_create.

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int count = 1000;
    int *in, *out, *sol;
    int i, fnderr=0;
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    in  = (int *) malloc( count * sizeof(int) );
    out = (int *) malloc( count * sizeof(int) );
    sol = (int *) malloc( count * sizeof(int) );

    for (i=0; i<count; i++)
    {
        *(in + i) = i;
        *(sol + i) = i*size;
        *(out + i) = 0;
    }

    MPI_Allreduce( in, out, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD );

    for (i=0; i<count; i++)
    {
        if (*(out + i) != *(sol + i))
        {
            fnderr++;
        }
    }

    if (fnderr)
    {
        fprintf( stderr, "(%d) Error for type MPI_INT and op MPI_SUM\n", rank );
        fflush(stderr);
    }
    
    free( in );
    free( out );
    free( sol );

    MPI_Finalize();
    return fnderr;
}

