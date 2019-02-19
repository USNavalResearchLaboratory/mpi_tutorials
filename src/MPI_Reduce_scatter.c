/* 
MPI_Reduce_scatter

   Combines values and scatters the results
int MPI_Reduce_scatter(
  void *sendbuf,
  void *recvbuf,
  int *recvcnts,
  MPI_Datatype datatype,
  MPI_Op op,
  MPI_Comm comm
);

Parameters

   sendbuf
          [in] starting address of send buffer (choice)

   recvbuf
          [out] starting address of receive buffer (choice)

   recvcounts
          [in] integer array specifying the number of elements in result
          distributed to each process. Array must be identical on all
          calling processes.

   datatype
          [in] data type of elements of input buffer (handle)

   op
          [in] operation (handle)

   comm
          [in] communicator (handle)

Remarks

   MPI_REDUCE_SCATTER first does an element-wise reduction on vector of
   [img138.gif] elements in the send buffer defined by sendbuf, count and
   datatype. Next, the resulting vector of results is split into n
   disjoint segments, where n is the number of members in the group.
   Segment i contains recvcounts[i] elements. The ith segment is sent to
   process i and stored in the receive buffer defined by recvbuf,
   recvcounts[i] and datatype.

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

   MPI_ERR_OP
          Invalid operation. MPI operations (objects of type MPI_Op) must
          either be one of the predefined operations (e.g., MPI_SUM) or
          created with MPI_Op_create.

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
#include <stdlib.h>

int main( int argc, char **argv )
{
    int err = 0;
    int *sendbuf, recvbuf, *recvcounts;
    int size, rank, i, sumval;
    MPI_Comm comm;

    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    MPI_Comm_size( comm, &size );
    MPI_Comm_rank( comm, &rank );
    sendbuf = (int *) malloc( size * sizeof(int) );
    for (i=0; i<size; i++)
        sendbuf[i] = rank + i;
    recvcounts = (int *)malloc( size * sizeof(int) );
    for (i=0; i<size; i++)
        recvcounts[i] = 1;

    MPI_Reduce_scatter( sendbuf, &recvbuf, recvcounts, MPI_INT, MPI_SUM, comm );

    sumval = size * rank + ((size - 1) * size)/2;
    /* recvbuf should be size * (rank + i) */
    if (recvbuf != sumval) {
        err++;
        fprintf( stdout, "Did not get expected value for reduce scatter\n" );
        fprintf( stdout, "[%d] Got %d expected %d\n", rank, recvbuf, sumval );
	fflush(stdout);
    }

    MPI_Finalize( );
    return err;
}

