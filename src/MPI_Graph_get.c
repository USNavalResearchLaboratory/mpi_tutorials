/* 
MPI_Graph_get

   Retrieves graph topology information associated with a communicator
int MPI_Graph_get(
  MPI_Comm comm,
  int maxindex,
  int maxedges,
  int *index,
  int *edges
);

Parameters

   comm
          [in] communicator with graph structure (handle)

   maxindex
          [in] length of vector index in the calling program (integer)

   maxedges
          [in] length of vector edges in the calling program (integer)

   index
          [out] array of integers containing the graph structure (for
          details see the definition of MPI_GRAPH_CREATE)

   edges
          [out] array of integers containing the graph structure

Remarks

   Functions MPI_GRAPHDIMS_GET and MPI_GRAPH_GET retrieve the
   graph-topology information that was associated with a communicator by
   MPI_GRAPH_CREATE.

   The information provided by MPI_GRAPHDIMS_GET can be used to dimension
   the vectors index and edges correctly for calls to MPI_GRAPH_GET.

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

