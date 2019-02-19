/* 
MPI_Get_elements

   Get the number of basic elements received
int MPI_Get_elements(
  MPI_Status *status,
  MPI_Datatype datatype,
  int *elements
);

Parameters

   status
          [in] status

   datatype
          [in] datatype

   elements
          [out] elements

Remarks

   The number of basic elements received can be retrieved from status
   using the query function MPI_GET_ELEMENTS.  If the size of the datatype
   is zero and the amount of data returned as determined by status is also
   zero, this routine will return a count of zero. This is consistent with
   a clarification made by the MPI Forum.

   Usage of MPI_GET_COUNT and MPI_GET_ELEMENT.

...
CALL MPI_TYPE_CONTIGUOUS(2, MPI_REAL, Type2, ierr)
CALL MPI_TYPE_COMMIT(Type2, ierr)
...
CALL MPI_COMM_RANK(comm, rank, ierr)
IF(rank.EQ.0) THEN
      CALL MPI_SEND(a, 2, MPI_REAL, 1, 0, comm, ierr)
      CALL MPI_SEND(a, 3, MPI_REAL, 1, 0, comm, ierr)
ELSE
      CALL MPI_RECV(a, 2, Type2, 0, 0, comm, stat, ierr)
      CALL MPI_GET_COUNT(stat, Type2, i, ierr)     ! returns i=1
      CALL MPI_GET_ELEMENTS(stat, Type2, i, ierr)  ! returns i=2
      CALL MPI_RECV(a, 2, Type2, 0, 0, comm, stat, ierr)
      CALL MPI_GET_COUNT(stat, Type2, i, ierr)     ! returns i=MPI_UNDEFINED
      CALL MPI_GET_ELEMENTS(stat, Type2, i, ierr)  ! returns i=3
END IF

   The function MPI_GET_ELEMENTS can also be used after a probe to find
   the number of elements in the probed message. Note that the two
   functions MPI_GET_COUNT and MPI_GET_ELEMENTS return the same values
   when they are used with basic datatypes.

   Rationale.

   The extension given to the definition of MPI_GET_COUNT seems natural:
   one would expect this function to return the value of the count
   argument, when the receive buffer is filled. Sometimes datatype
   represents a basic unit of data one wants to transfer, for example, a
   record in an array of records (structures). One should be able to find
   out how many components were received without bothering to divide by
   the number of elements in each component. However, on other occasions,
   datatype is used to define a complex layout of data in the receiver
   memory, and does not represent a basic unit of data for transfers. In
   such cases, one needs to use the function MPI_GET_ELEMENTS.

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

/* Receive partial datatypes and check that MPI_Getelements gives the
correct version */

int main( int argc, char *argv[] )
{
    int errs = 0;
    MPI_Datatype outtype, oldtypes[2];
    MPI_Aint offsets[2];
    int blklens[2];
    MPI_Comm comm;
    int size, rank, src, dest, tag;

    MPI_Init( &argc, &argv );

    comm = MPI_COMM_WORLD;
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );
    if (size < 2) {
        printf( "This test requires at least 2 processes\n");
	fflush(stdout);
        MPI_Abort( MPI_COMM_WORLD, 1 );
    }

    src = 0;
    dest = 1;

    if (rank == src) {
        int buf[128], position, cnt;
        /* sender */
        /* Create a datatype and send it (multiple of sizeof(int)) */
        /* Create a send struct type */
        oldtypes[0] = MPI_INT;
        oldtypes[1] = MPI_CHAR;
        blklens[0] = 1;
        blklens[1] = 4*sizeof(int);
        offsets[0] = 0;
        offsets[1] = sizeof(int);
        MPI_Type_struct( 2, blklens, offsets, oldtypes, &outtype );
        MPI_Type_commit( &outtype );

        buf[0] = 4*sizeof(int);
        MPI_Send( buf, 1, outtype, dest, 0, comm );
        MPI_Type_free( &outtype );

        /* Create a datatype and send it (not a multiple of sizeof(int)) */
        /* Create a send struct type */
        oldtypes[0] = MPI_INT;
        oldtypes[1] = MPI_CHAR;
        blklens[0] = 1;
        blklens[1] = 4*sizeof(int)+1;
        offsets[0] = 0;
        offsets[1] = sizeof(int);
        MPI_Type_struct( 2, blklens, offsets, oldtypes, &outtype );
        MPI_Type_commit( &outtype );

        buf[0] = 4*sizeof(int) + 1;
        MPI_Send( buf, 1, outtype, dest, 1, comm );
        MPI_Type_free( &outtype );

        /* Pack data and send as packed */
        position = 0;
        cnt = 7;
        MPI_Pack( &cnt, 1, MPI_INT, buf, 128*sizeof(int), &position, comm );
        MPI_Pack( "message", 7, MPI_CHAR, buf, 128*sizeof(int), &position, comm );
        MPI_Send( buf, position, MPI_PACKED, dest, 2, comm );
    }
    else if (rank == dest) {
        MPI_Status status;
        int buf[128], i, elms, count;
        /* Receiver */
        /* Create a receive struct type */
        oldtypes[0] = MPI_INT;
        oldtypes[1] = MPI_CHAR;
        blklens[0] = 1;
        blklens[1] = 256;
        offsets[0] = 0;
        offsets[1] = sizeof(int);
        MPI_Type_struct( 2, blklens, offsets, oldtypes, &outtype );
        MPI_Type_commit( &outtype );

        for (i=0; i<3; i++) {
            tag = i;
            MPI_Recv( buf, 1, outtype, src, tag, comm, &status );
            MPI_Get_elements( &status, outtype, &elms );
            if (elms != buf[0] + 1) {
                errs++;
                printf( "For test %d, Get elements gave %d but should be %d\n", i, elms, buf[0] + 1 );
		fflush(stdout);
            }
            MPI_Get_count( &status, outtype, &count );
            if (count != MPI_UNDEFINED) {
                errs++;
                printf( "For partial send, Get_count did not return MPI_UNDEFINED\n" );
		fflush(stdout);
            }
        }
        MPI_Type_free( &outtype );
    }

    MPI_Finalize();
    return errs;
}
