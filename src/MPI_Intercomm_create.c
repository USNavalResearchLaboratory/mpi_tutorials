/* 
MPI_Intercomm_create

   Creates an intercommuncator from two intracommunicators
int MPI_Intercomm_create(
  MPI_Comm local_comm,
  int local_leader,
  MPI_Comm peer_comm,
  int remote_leader,
  int tag,
  MPI_Comm *newintercomm
);

Parameters

   local_comm
          [in] Local (intra)communicator

   local_leader
          [in] Rank in local_comm of leader (often 0)

   peer_comm
          [in] Communicator used to communicate between a designated
          process in the other communicator. Significant only at the
          process in local_comm with rank local_leader.

   remote_leader
          [in] Rank in peer_comm of remote leader (often 0)

   tag
          [in] Message tag to use in constructing intercommunicator; if
          multiple MPI_Intercomm_creates are being made, they should use
          different tags (more precisely, ensure that the local and remote
          leaders are using different tags for each MPI_intercomm_create).

   comm_out
          [out] Created intercommunicator

Remarks

   This call creates an inter-communicator. It is collective over the
   union of the local and remote groups. Processes should provide
   identical local_comm and local_leader arguments within each group.
   Wildcards are not permitted for remote_leader, local_leader, and tag.

   This call uses point-to-point communication with communicator
   peer_comm, and with tag tag between the leaders. Thus, care must be
   taken that there be no pending communication on peer_comm that could
   interfere with this communication.

   peer_comm is significant only for the process designated the
   local_leader in the local_comm.

   The MPI 1.1 Standard contains two mutually exclusive comments on the
   input intracommunicators. One says that their repective groups must be
   disjoint; the other that the leaders can be the same process. After
   some discussion by the MPI Forum, it has been decided that the groups
   must be disjoint. Note that the reason given for this in the standard
   is not the reason for this choice; rather, the other operations on
   intercommunicators (like MPI_Intercomm_merge) do not make sense if the
   groups are not disjoint.

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

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

   MPI_ERR_TAG
          Invalid tag argument. Tags must be non-negative; tags in a
          receive (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be
          MPI_ANY_TAG. The largest tag value is available through the the
          attribute MPI_TAG_UB.

   MPI_ERR_INTERN
          This error is returned when some part of the MPICH
          implementation is unable to acquire memory.

   MPI_ERR_RANK
          Invalid source or destination rank. Ranks must be between zero
          and the size of the communicator minus one; ranks in a receive
          (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be
          MPI_ANY_SOURCE.

See Also

   MPI_Intercomm_merge, MPI_Comm_free, MPI_Comm_remote_group,
   MPI_Comm_remote_size

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include <stdio.h>
#include "mpi.h"
#define BUFSIZE 2000
int main( int argc, char *argv[] )
{
    MPI_Status status;
    MPI_Comm comm,scomm;
    int a[10], b[10];
    int buf[BUFSIZE], *bptr, bl, i, j, rank, size, color, errs=0;
    MPI_Init( 0, 0 );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    color = rank % 2;
    MPI_Comm_split( MPI_COMM_WORLD, color, rank, &scomm );
    MPI_Intercomm_create( scomm, 0, MPI_COMM_WORLD, 1-color, 52,
&comm);
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_remote_size( comm, &size );
    MPI_Buffer_attach( buf, BUFSIZE );
    for (j=0; j<10; j++) {
        for (i=0; i<10; i++) {
            a[i] = (rank + 10 * j) * size + i;
        }
        MPI_Bsend( a, 10, MPI_INT, 0, 27+j, comm );
    }
    if (rank == 0) {
        for (i=0; i<size; i++) {
            for (j=0; j<10; j++) {
                int k;
                status.MPI_TAG = -10;
                status.MPI_SOURCE = -20;
                MPI_Recv( b, 10, MPI_INT, i, 27+j, comm, &status );
                if (status.MPI_TAG != 27+j) {
                    errs++;
                    printf( "Wrong tag = %d\n", status.MPI_TAG
);fflush(stdout);
                }
                if (status.MPI_SOURCE != i) {
                    errs++;
                    printf( "Wrong source = %d\n", status.MPI_SOURCE
);fflush(stdout);
                }
                for (k=0; k<10; k++) {
                    if (b[k] != (i + 10 * j) * size + k) {
                        errs++;
                        printf( "received b[%d] = %d from %d tag %d\n",
k, b[k], i, 27+j );fflush(stdout);
                    }
                }
            }
        }
    }
    MPI_Buffer_detach( &bptr, &bl );
    MPI_Comm_free(&scomm);
    MPI_Comm_free(&comm);
    MPI_Finalize();
    return errs;
}

