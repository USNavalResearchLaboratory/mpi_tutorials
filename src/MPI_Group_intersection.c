/* 
MPI_Group_intersection

   Produces a group as the intersection of two existing groups
int MPI_Group_intersection(
  MPI_Group group1,
  MPI_Group group2,
  MPI_Group *newgroup
);

Parameters

   group1
          [in] first group (handle)

   group2
          [in] second group (handle)

   newgroup
          [out] intersection group (handle)

Remarks

   The output group contains those processes that are in both group1 and
   group2.

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

   MPI_ERR_GROUP
          Null or invalid group passed to function.

   MPI_ERR_INTERN
          This error is returned when some part of the MPICH
          implementation is unable to acquire memory.

See Also

   MPI_Group_free

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

/*
Test the group routines
MPI_Group_compare
MPI_Group_excl
MPI_Group_intersection
MPI_Group_range_excl
MPI_Group_rank
MPI_Group_size
MPI_Group_translate_ranks
MPI_Group_union
*/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
    int errs=0, toterr;
    MPI_Group basegroup;
    MPI_Group g1, g2, g3, g4, g5, g6, g7, g8, g9, g10;
    MPI_Group g3a, g3b;
    MPI_Comm comm, newcomm, splitcomm, dupcomm;
    int i, grp_rank, rank, grp_size, size, result;
    int nranks, *ranks, *ranks_out;
    int range[1][3];
    int worldrank;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &worldrank );
    comm = MPI_COMM_WORLD;
    MPI_Comm_group( comm, &basegroup );
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );

    /* Get the basic information on this group */
    MPI_Group_rank( basegroup, &grp_rank );
    if (grp_rank != rank) {
        errs++;
        fprintf( stdout, "group rank %d != comm rank %d\n", grp_rank, rank );
	fflush(stdout);
    }
    MPI_Group_size( basegroup, &grp_size );
    if (grp_size != size) {
        errs++;
        fprintf( stdout, "group size %d != comm size %d\n", grp_size, size );
    }

    /* Form a new communicator with inverted ranking */
    MPI_Comm_split( comm, 0, size - rank, &newcomm );
    MPI_Comm_group( newcomm, &g1 );
    ranks = (int *)malloc( size * sizeof(int) );
    ranks_out = (int *)malloc( size * sizeof(int) );
    for (i=0; i<size; i++) ranks[i] = i;
    nranks = size;
    MPI_Group_translate_ranks( g1, nranks, ranks, basegroup, ranks_out);
    for (i=0; i<size; i++) {
        if (ranks_out[i] != (size - 1) - i) {
            errs++;
            fprintf( stdout, "Translate ranks got %d expected %d\n",
		     ranks_out[i], (size - 1) - i );
	    fflush(stdout);
        }
    }

    /* Check Compare */
    MPI_Group_compare( basegroup, g1, &result );
    if (result != MPI_SIMILAR) {
        errs++;
        fprintf( stdout, "Group compare should have been similar, was %d\n",
		 result );
	fflush(stdout);
    }
    MPI_Comm_dup( comm, &dupcomm );
    MPI_Comm_group( dupcomm, &g2 );
    MPI_Group_compare( basegroup, g2, &result );
    if (result != MPI_IDENT) {
        errs++;
        fprintf( stdout, "Group compare should have been ident, was %d\n",
		 result );
	fflush(stdout);
    }
    MPI_Comm_split( comm, rank < size/2, rank, &splitcomm );
    MPI_Comm_group( splitcomm, &g3 );
    MPI_Group_compare( basegroup, g3, &result );
    if (result != MPI_UNEQUAL) {
        errs++;
        fprintf( stdout, "Group compare should have been unequal, was %d\n",
		 result );
	fflush(stdout);
    }

    /* Build two groups that have this process and one other, but do
       not have the same processes */
    ranks[0] = rank;
    ranks[1] = (rank + 1) % size;
    MPI_Group_incl( basegroup, 2, ranks, &g3a );
    ranks[1] = (rank + size - 1) % size;
    MPI_Group_incl( basegroup, 2, ranks, &g3b );
    MPI_Group_compare( g3a, g3b, &result );
    if (result != MPI_UNEQUAL) {
        errs++;
        fprintf( stdout, "Group compare of equal sized but different"
		 "groups should have been unequal, was %d\n", result );
	fflush(stdout);
    }
    /* Build two new groups by excluding members; use Union to put them
       together again */
    /* Exclude 0 */
    for (i=0; i<size; i++) ranks[i] = i;
    MPI_Group_excl( basegroup, 1, ranks, &g4 );
    /* Exclude 1-(size-1) */
    MPI_Group_excl( basegroup, size-1, ranks+1, &g5 );
    MPI_Group_union( g5, g4, &g6 );
    MPI_Group_compare( basegroup, g6, &result );
    if (result != MPI_IDENT) {
        int usize;
        errs++;
        /* See ordering requirements on union */
        fprintf( stdout, "Group excl and union did not give ident groups\n" );
        fprintf( stdout, "[%d] result of compare was %d\n", rank, result );
        MPI_Group_size( g6, &usize );
        fprintf( stdout, "Size of union is %d, should be %d\n", usize, size );
        fflush(stdout);
    }
    MPI_Group_union( basegroup, g4, &g7 );
    MPI_Group_compare( basegroup, g7, &result );
    if (result != MPI_IDENT) {
        int usize;
        errs++;
        fprintf( stdout, "Group union of overlapping groups failed\n");
        fprintf( stdout, "[%d] result of compare was %d\n", rank, result );
        MPI_Group_size( g7, &usize );
        fprintf( stdout, "Size of union is %d, should be %d\n", usize, size );
        fflush(stdout);
    }

    /* Use range_excl instead of ranks */
    /* printf ("range excl\n" ); fflush( stdout ); */
    range[0][0] = 1;
    range[0][1] = size-1;
    range[0][2] = 1;
    MPI_Group_range_excl( basegroup, 1, range, &g8 );
    /* printf( "out of range excl\n" ); fflush( stdout ); */
    MPI_Group_compare( g5, g8, &result );
    /* printf( "out of compare\n" ); fflush( stdout ); */
    if (result != MPI_IDENT) {
        errs++;
        fprintf( stdout, "Group range excl did not give ident groups\n");
    }

    /* printf( "intersection\n" ); fflush( stdout ); */
    MPI_Group_intersection( basegroup, g4, &g9 );
    MPI_Group_compare( g9, g4, &result );
    if (result != MPI_IDENT) {
        errs++;
        fprintf( stdout, "Group intersection did not give ident groups\n" );
    }

    /* Exclude EVERYTHING and check against MPI_GROUP_EMPTY */
    /* printf( "range excl all\n" ); fflush( stdout ); */
    range[0][0] = 0;
    range[0][1] = size-1;
    range[0][2] = 1;
    MPI_Group_range_excl( basegroup, 1, range, &g10 );

    MPI_Group_compare( g10, MPI_GROUP_EMPTY, &result );

    if (result != MPI_IDENT) {
        errs++;
        fprintf( stdout, "MPI_GROUP_EMPTY didn't compare against empty group\n");
	fflush(stdout);
    }

    MPI_Group_free( &basegroup );
    MPI_Group_free( &g1 );
    MPI_Group_free( &g2 );
    MPI_Group_free( &g3 );
    MPI_Group_free( &g3a );
    MPI_Group_free( &g3b );
    MPI_Group_free( &g4 );
    MPI_Group_free( &g5 );
    MPI_Group_free( &g6 );
    MPI_Group_free( &g7 );
    MPI_Group_free( &g8 );
    MPI_Group_free( &g9 );
    MPI_Group_free( &g10 );
    MPI_Comm_free( &dupcomm );
    MPI_Comm_free( &splitcomm );
    MPI_Comm_free( &newcomm );

    MPI_Finalize();
    return errs;
}

