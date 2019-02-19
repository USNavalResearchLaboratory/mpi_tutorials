/* 
MPI_Get_count

   Gets the number of "top level" elements
int MPI_Get_count(
  MPI_Status *status,
  MPI_Datatype datatype,
  int *count
);

Parameters

   status
          [in] return status of receive operation (Status)

   datatype
          [in] datatype of each receive buffer element (handle)

   count
          [out] number of received elements (integer)

Remarks

   Returns the number of entries received. (Again, we count entries, each
   of type datatype, not bytes.) The datatype argument should match the
   argument provided by the receive call that set the status variable.
   If the size of the datatype is zero, this routine will return a count
   of zero. If the amount of data in status is not an exact multiple of
   the size of datatype (so that count would not be integral), a count of
   MPI_UNDEFINED is returned instead.

   [] Rationale.

   Some message passing libraries use INOUT count, tag and source
   arguments, thus using them both to specify the selection criteria for
   incoming messages and return the actual envelope values of the received
   message. The use of a separate status argument prevents errors that are
   often attached with INOUT argument (e.g., using the MPI_ANY_TAG
   constant as the tag in a receive). Some libraries use calls that refer
   implicitly to the "last message received." This is not thread safe.

   The datatype argument is passed to MPI_GET_COUNT so as to improve
   performance. A message might be received without counting the number of
   elements it contains, and the count value is often not needed. Also,
   this allows the same function to be used after a call to MPI_PROBE. (
   End of rationale.)

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

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

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
