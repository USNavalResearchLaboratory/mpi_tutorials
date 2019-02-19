/* 
MPI_Graph_map

   Maps process to graph topology information
int MPI_Graph_map(
  MPI_Comm comm_old,
  int nnodes,
  int *index,
  int *edges,
  int *newrank
);

Parameters

   comm
          [in] input communicator (handle)

   nnodes
          [in] number of graph nodes (integer)

   index
          [in] integer array specifying the graph structure, see
          MPI_GRAPH_CREATE

   edges
          [in] integer array specifying the graph structure

   newrank
          [out] reordered rank of the calling process; MPI_UNDEFINED if
          the calling process does not belong to graph (integer)

Remarks


Thread and Interrupt Safety

   This routine is both thread- and interrupt-safe. This means that this
   routine may safely be used by multiple threads and from within a signal
   handler.

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

   MPI_ERR_TOPOLOGY
          Invalid topology. Either there is no topology associated with
          this communicator, or it is not the correct type (e.g., MPI_CART
          when expecting MPI_GRAPH).

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    int errs = 0;
    int newrank, merr, rank;
    int index[2], edges[2];

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    /* Graph map where there are no nodes for this process */
    MPI_Comm_set_errhandler( MPI_COMM_WORLD, MPI_ERRORS_RETURN );
    /* Here is a singleton graph, containing only the root process */
    index[0] = 0;
    edges[0] = 0;
    merr = MPI_Graph_map( MPI_COMM_WORLD, 1, index, edges, &newrank );
    if (merr) {
        errs++;
        printf( "Graph map returned an error\n" );
	fflush(stdout);
    }
    if (rank != 0 && newrank != MPI_UNDEFINED) {
        errs++;
        printf( "Graph map with no local nodes did not return MPI_UNDEFINED\n" );
	fflush(stdout);
    }

    MPI_Finalize();
    return errs;
}

