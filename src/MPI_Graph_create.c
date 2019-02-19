/* 
MPI_Graph_create

   Makes a new communicator to which topology information has been
   attached
int MPI_Graph_create(
  MPI_Comm comm_old,
  int nnodes,
  int *index,
  int *edges,
  int reorder,
  MPI_Comm *comm_graph
);

Parameters

   comm_old
          [in] input communicator without topology (handle)

   nnodes
          [in] number of nodes in graph (integer)

   index
          [in] array of integers describing node degrees (see below)

   edges
          [in] array of integers describing graph edges (see below)

   reorder
          [in] ranking may be reordered (true) or not (false) (logical)

   comm_graph
          [out] communicator with graph topology added (handle)

Remarks



   MPI_GRAPH_CREATE returns a handle to a new communicator to which the
   graph topology information is attached. If reorder = false then the
   rank of each process in the new group is identical to its rank in the
   old group. Otherwise, the function may reorder the processes. If the
   size, nnodes, of the graph is smaller than the size of the group of
   comm, then some processes are returned MPI_COMM_NULL, in analogy to
   MPI_CART_CREATE and MPI_COMM_SPLIT. The call is erroneous if it
   specifies a graph that is larger than the group size of the input
   communicator.

   The three parameters nnodes, index and edges define the graph
   structure. nnodes is the number of nodes of the graph. The nodes are
   numbered from 0 to nnodes-1. The ith entry of array index stores the
   total number of neighbors of the first i graph nodes. The lists of
   neighbors of nodes 0, 1, ..., nnodes-1 are stored in consecutive
   locations in array edges. The array edges is a flattened representation
   of the edge lists. The total number of entries in index is nnodes and
   the total number of entries in edges is equal to the number of graph
   edges.

   Each process must provide a description of the entire graph, not just
   the neigbors of the calling process.

   The definitions of the arguments nnodes, index, and edges are
   illustrated with the following simple example.

   Example

   Assume there are four processes 0, 1, 2, 3 with the following adjacency
   matrix:

   [img171.gif]


   Then, the input arguments are:

   [img172.gif]


   Thus, in C, index[0] is the degree of node zero, and index[i] -
   index[i-1] is the degree of node i, i=1, ..., nnodes-1; the list of
   neighbors of node zero is stored in edges[j], for [img173.gif] and the
   list of neighbors of node i, [img174.gif] , is stored in edges[j],
   [img175.gif] .

   In Fortran, index(1) is the degree of node zero, and index(i+1) -
   index(i) is the degree of node i, i=1, ..., nnodes-1; the list of
   neighbors of node zero is stored in edges(j), for [img176.gif] and the
   list of neighbors of node i, [img177.gif] , is stored in edges(j),
   [img178.gif] .


Algorithm

   We ignore the reorder info currently.

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
#include <stdlib.h>

int main( int argc, char *argv[] )
{
    int errs = 0, i, k;
        int dims[2], periods[2], wsize;
    int outdims[2], outperiods[2], outcoords[2];
    int topo_type;
    int *index, *edges, *outindex, *outedges;
    MPI_Comm comm1, comm2;
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &wsize );
    /* Create a cartesian topology, get its characteristics, then
        dup it and check that the new communicator has the same
        properties */
    dims[0] = dims[1] = 0;
    MPI_Dims_create( wsize, 2, dims );
    periods[0] = periods[1] = 0;
    MPI_Cart_create( MPI_COMM_WORLD, 2, dims, periods, 0, &comm1 );
    MPI_Comm_dup( comm1, &comm2 );
    MPI_Topo_test( comm2, &topo_type );
    if (topo_type != MPI_CART) {
        errs++;
        printf( "Topo type of duped cart was not cart\n");
	fflush(stdout);
    }
    else {
        MPI_Cart_get( comm2, 2, outdims, outperiods, outcoords );
        for (i=0; i<2; i++) {
            if (outdims[i] != dims[i]) {
                errs++;
                printf( "%d = outdims[%d] != dims[%d] = %d\n",
			outdims[i], i, i, dims[i] );
		fflush(stdout);
            }
            if (outperiods[i] != periods[i]) {
                errs++;
                printf( "%d = outperiods[%d] != periods[%d] = %d\n",
			outperiods[i], i, i, periods[i] );
		fflush(stdout);
            }
        }
    }
    MPI_Comm_free( &comm2 );
    MPI_Comm_free( &comm1 );
    /* Now do the same with a graph topology */
    if (wsize >= 3) {
        index = (int*)malloc(wsize * sizeof(int) );
        edges = (int*)malloc(wsize * 2 * sizeof(int) );
        if (!index || !edges) {
            printf( "Unable to allocate %d words for index or edges\n", 3 * wsize );
	    fflush(stdout);
            MPI_Abort( MPI_COMM_WORLD, 1 );
        }
        index[0] = 2;
        for (i=1; i<wsize; i++) {
            index[i] = 2 + index[i-1];
        }
        k=0;
        for (i=0; i<wsize; i++) {
            edges[k++] = (i-1+wsize) % wsize;
            edges[k++] = (i+1) % wsize;
        }
        MPI_Graph_create( MPI_COMM_WORLD, wsize, index, edges, 0, &comm1 );
        MPI_Comm_dup( comm1, &comm2 );
        MPI_Topo_test( comm2, &topo_type );
        if (topo_type != MPI_GRAPH) {
            errs++;
            printf( "Topo type of duped graph was not graph\n");
	    fflush(stdout);
        }
        else {
            int nnodes, nedges;
            MPI_Graphdims_get( comm2, &nnodes, &nedges );
            if (nnodes != wsize) {
                errs++;
                printf( "Nnodes = %d, should be %d\n", nnodes, wsize);
		fflush(stdout);
            }
            if (nedges != 2*wsize) {
                errs++;
                printf( "Nedges = %d, should be %d\n", nedges, 2*wsize);
		fflush(stdout);
            }
            outindex = (int*)malloc(wsize * sizeof(int) );
            outedges = (int*)malloc(wsize * 2 * sizeof(int) );
            if (!outindex || !outedges) {
                printf( "Unable to allocate %d words for outindex or outedges\n",
			3 * wsize );
		fflush(stdout);
                MPI_Abort( MPI_COMM_WORLD, 1 );
            }
            MPI_Graph_get( comm2, wsize, 2*wsize, outindex, outedges );
            for (i=0; i<wsize; i++) {
                if (index[i] != outindex[i]) {
                    printf( "%d = index[%d] != outindex[%d] = %d\n",
			    index[i], i, i, outindex[i] );
		    fflush(stdout);
                    errs++;
                }
            }
            for (i=0; i<2*wsize; i++) {
                if (edges[i] != outedges[i]) {
                    printf( "%d = edges[%d] != outedges[%d] = %d\n",
			    edges[i], i, i, outedges[i] );
		    fflush(stdout);
                    errs++;
                }
            }
            free( outindex );
            free( outedges );
        }
        free( index );
        free( edges );
        MPI_Comm_free( &comm2 );
        MPI_Comm_free( &comm1 );
    }
    
    MPI_Finalize();
    return 0;
}

