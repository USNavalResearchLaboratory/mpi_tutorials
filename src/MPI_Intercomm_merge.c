/* 
MPI_Intercomm_merge

   Creates an intracommuncator from an intercommunicator
int MPI_Intercomm_merge(
  MPI_Comm intercomm,
  int high,
  MPI_Comm *newintracomm
);

Parameters

   comm
          [in] Intercommunicator (handle)

   high
          [in] Used to order the groups within comm (logical) when
          creating the new communicator. This is a boolean value; the
          group that sets high true has its processes ordered after the
          group that sets this value to false. If all processes in the
          intercommunicator provide the same value, the choice of which
          group is ordered first is arbitrary.

   comm_out
          [out] Created intracommunicator (handle)

Remarks

   This function creates an intra-communicator from the union of the two
   groups that are associated with intercomm. All processes should provide
   the same high value within each of the two groups. If processes in one
   group provided the value high = false and processes in the other group
   provided the value high = true then the union orders the "low" group
   before the "high" group. If all processes provided the same high
   argument then the order of the union is arbitrary. This call is
   blocking and collective within the union of the two groups.

   While all processes may provide the same value for the high parameter,
   this requires the MPI implementation to determine which group of
   processes should be ranked first. The MPICH implementation uses various
   techniques to determine which group should go first, but there is a
   possibility that the implementation will be unable to break the tie.
   Robust applications should avoid using the same value for high in both
   groups.

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

Algorithm

   s Allocate contexts Local and remote group leaders swap high values
   Determine the high value. Merge the two groups and make the
   intra-communicator e

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

   MPI_ERR_INTERN
          This error is returned when some part of the MPICH
          implementation is unable to acquire memory.

See Also

   MPI_Intercomm_create, MPI_Comm_free

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

/* Test intercomm merge, including the choice of the high value */
void GetIntercomm( MPI_Comm *comm, int *isLeftGroup, int min_size )
{
    int size, rank, remsize, merr;
    int done=0;
    MPI_Comm mcomm;
    int rleader;

    /* Split comm world in half */
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    if (size > 1) {
        merr = MPI_Comm_split( MPI_COMM_WORLD, (rank < size/2), rank, &mcomm );
        if (rank == 0) {
            rleader = size/2;
        }
        else if (rank == size/2) {
            rleader = 0;
        }
        else {
            /* Remote leader is signficant only for the processes
               designated local leaders */
            rleader = -1;
        }
        *isLeftGroup = rank < size/2;
        merr = MPI_Intercomm_create( mcomm, 0, MPI_COMM_WORLD,
				     rleader, 12345, comm );
        merr = MPI_Comm_free( &mcomm );
    }
    else
        *comm = MPI_COMM_NULL;
}

int main( int argc, char *argv[] )
{
    int errs = 0;
    int rank, size, rsize;
    int nsize, nrank;
    int minsize = 2;
    int isLeft;
    MPI_Comm comm, comm1, comm2, comm3, comm4;

    MPI_Init( &argc, &argv );

    GetIntercomm( &comm, &isLeft, minsize );
    /* Determine the sender and receiver */
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_remote_size( comm, &rsize );
    MPI_Comm_size( comm, &size );

    /* Try building intercomms */
    MPI_Intercomm_merge( comm, isLeft, &comm1 );
    /* Check the size and ranks */
    MPI_Comm_size( comm1, &nsize );
    MPI_Comm_rank( comm1, &nrank );
    if (nsize != size + rsize) {
        errs++;
        printf( "(1) Comm size is %d but should be %d\n", nsize, size + rsize );
	fflush(stdout);
        if (isLeft) {
            /* The left processes should be high */
            if (nrank != rsize + rank) {
                errs++;
                printf( "(1) rank for high process is %d should be %d\n",
			nrank, rsize + rank );
		fflush(stdout);
            }
        }
        else {
            /* The right processes should be low */
            if (nrank != rank) {
                errs++;
                printf( "(1) rank for low process is %d should be %d\n",
			nrank, rank );
		fflush(stdout);
            }
        }
    }

    MPI_Intercomm_merge( comm, !isLeft, &comm2 );
    /* Check the size and ranks */
    MPI_Comm_size( comm1, &nsize );
    MPI_Comm_rank( comm1, &nrank );
    if (nsize != size + rsize) {
        errs++;
        printf( "(2) Comm size is %d but should be %d\n", nsize, size + rsize );
	fflush(stdout);
        if (!isLeft) {
            /* The right processes should be high */
            if (nrank != rsize + rank) {
                errs++;
                printf( "(2) rank for high process is %d should be %d\n",
			nrank, rsize + rank );
		fflush(stdout);
            }
        }
        else {
            /* The left processes should be low */
            if (nrank != rank) {
                errs++;
                printf( "(2) rank for low process is %d should be %d\n",
			nrank, rank );
		fflush(stdout);
            }
        }
    }

    MPI_Intercomm_merge( comm, 0, &comm3 );
    MPI_Intercomm_merge( comm, 1, &comm4 );

    MPI_Comm_free( &comm1 );
    MPI_Comm_free( &comm2 );
    MPI_Comm_free( &comm3 );
    MPI_Comm_free( &comm4 );

    MPI_Finalize();
    return errs;
}
