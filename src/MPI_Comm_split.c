/* 
MPI_Comm_split

   Creates new communicators based on colors and keys
int MPI_Comm_split(
  MPI_Comm comm,
  int color,
  int key,
  MPI_Comm *newcomm
);

Parameters

   comm
          [in] communicator (handle)

   color
          [in] control of subset assignment (nonnegative integer).
          Processes with the same color are in the same new communicator

   key
          [in] control of rank assigment (integer)

   newcomm
          [out] new communicator (handle)

Remarks

   This function partitions the group associated with comm into disjoint
   subgroups, one for each value of color. Each subgroup contains all
   processes of the same color. Within each subgroup, the processes are
   ranked in the order defined by the value of the argument key, with ties
   broken according to their rank in the old group. A new communicator is
   created for each subgroup and returned in newcomm. A process may supply
   the color value MPI_UNDEFINED, in which case newcomm returns
   MPI_COMM_NULL. This is a collective call, but each process is permitted
   to provide different values for color and key.

   A call to MPI_COMM_CREATE(comm, group, newcomm) is equivalent to
   a call to MPI_COMM_SPLIT(comm, color, key, newcomm), where all members
   of group provide color~ =~0 and key~=~ rank in group, and all processes
   that are not members of group provide color~ =~ MPI_UNDEFINED. The
   function MPI_COMM_SPLIT allows more general partitioning of a group
   into one or more subgroups with optional reordering. This call applies
   only intra-communicators.

   The value of color must be nonnegative or MPI_UNDEFINED.

   Advice to users.

   This is an extremely powerful mechanism for dividing a single
   communicating group of processes into k subgroups, with k chosen
   implicitly by the user (by the number of colors asserted over all the
   processes). Each resulting communicator will be non-overlapping. Such a
   division could be useful for defining a hierarchy of computations, such
   as for multigrid, or linear algebra.

   Multiple calls to MPI_COMM_SPLIT can be used to overcome the
   requirement that any call have no overlap of the resulting
   communicators (each process is of only one color per call). In this
   way, multiple overlapping communication structures can be created.
   Creative use of the color and key in such splitting operations is
   encouraged.

   Note that, for a fixed color, the keys need not be unique. It is
   MPI_COMM_SPLIT's responsibility to sort processes in ascending order
   according to this key, and to break ties in a consistent way. If all
   the keys are specified in the same way, then all the processes in a
   given color will have the relative rank order as they did in their
   parent group. (In general, they will have different ranks.)

   Essentially, making the key value zero for all processes of a given
   color means that one doesn't really care about the rank-order of the
   processes in the new communicator.
   Rationale.

   color is restricted to be nonnegative, so as not to confict with the
   value assigned to MPI_UNDEFINED.

   The result of MPI_COMM_SPLIT on an intercommunicator is that those
   processes on the left with the same color as those processes on the
   right combine to create a new intercommunicator. The key argument
   describes the relative rank of processes on each side of the
   intercommunicator (see Figure 10). For those colors that are specified
   only on one side of the intercommunicator, MPI_COMM_NULL is returned.
   MPI_COMM_NULL is also returned to those processes that specify
   MPI_UNDEFINED as the color.



                           [collective-split2.gif]


   Figure 10:

   [ ]Intercommunicator construction achieved by splitting an existing
   intercommunicator with MPI_COMM_SPLIT extended to intercommunicators.
   Example(Parallel client-server model). The following client code
   illustrates how clients on the left side of an intercommunicator could
   be assigned to a single server from a pool of servers on the right side
   of an intercommunicator.

        // Client code

        MPI_Comm  multiple_server_comm;
        MPI_Comm  single_server_comm;
        int       color, rank, num_servers;

        // Create intercommunicator with clients and servers:
        //   multiple_server_comm 
        ...

	// Find out the number of servers available
        MPI_Comm_remote_size ( multiple_server_comm, &num_servers );

        // Determine my color 
        MPI_Comm_rank ( multiple_server_comm, &rank );
        color = rank % num_servers;

        // Split the intercommunicator
        MPI_Comm_split ( multiple_server_comm, color, rank,
                         &single_server_comm );

   The following is the corresponding server code:

        // Server code 
        MPI_Comm  multiple_client_comm;
        MPI_Comm  single_server_comm;
        int       rank;

        // Create intercommunicator with clients and servers:
        //   multiple_client_comm 
        ...

        // Split the intercommunicator for a single server per group
        //   of clients
        MPI_Comm_rank ( multiple_client_comm, &rank );
        MPI_Comm_split ( multiple_client_comm, rank, 0,
                         &single_server_comm );


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

  1. Use MPI_Allgather to get the color and key from each process
  2. Count the number of processes with the same color; create a
     communicator with that many processes.  If this process has
     MPI_UNDEFINED as the color, create a process with a single member.
  3. Use key to order the ranks
  4. Set the VCRs using the ordered key values

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

   MPI_Comm_free

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
    MPI_Intercomm_create( scomm, 0, MPI_COMM_WORLD, 1-color, 52, &comm);
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
                    printf( "Wrong tag = %d\n", status.MPI_TAG);
		    fflush(stdout);
                }
                if (status.MPI_SOURCE != i) {
                    errs++;
                    printf( "Wrong source = %d\n", status.MPI_SOURCE);
		    fflush(stdout);
                }
                for (k=0; k<10; k++) {
                    if (b[k] != (i + 10 * j) * size + k) {
                        errs++;
                        printf( "received b[%d] = %d from %d tag %d\n", k, b[k], i, 27+j);
			fflush(stdout);
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

