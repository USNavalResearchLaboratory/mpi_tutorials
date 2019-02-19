/* 
MPI_Win_get_group

   Get the MPI Group of the window object
int MPI_Win_get_group(
  MPI_Win win,
  MPI_Group *group
);

Parameters

   win
          [in] window object (handle)

   group
          [out] group of processes which share access to the window
          (handle)

Remarks

   MPI_WIN_GET_GROUP returns a duplicate of the group of the communicator
   used to create the window. associated with win. The group is returned
   in group.

   The group is a duplicate of the group from the communicator used to
   create the MPI window, and should be freed with MPI_Group_free when it
   is no longer needed. This group can be used to form the group of
   neighbors for the routines MPI_Win_post and MPI_Win_start.

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

   MPI_ERR_WIN
          Invalid MPI window object

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

/* Test of Win_get_group */

int main( int argc, char *argv[] )
{
    int errs = 0;
    int result;
    int buf[10];
    MPI_Win   win;
    MPI_Group group, wingroup;
    int minsize = 2;
    MPI_Comm      comm;
    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    MPI_Win_create( buf, sizeof(int) * 10, sizeof(int), MPI_INFO_NULL,comm, &win );
    MPI_Win_get_group( win, &wingroup );
    MPI_Comm_group( comm, &group );
    MPI_Group_compare( group, wingroup, &result );
    if (result != MPI_IDENT) {
        errs++;
        fprintf( stderr, "Group returned by Win_get_group not the same as the input group\n" );
	fflush(stderr);
    }
    MPI_Group_free( &wingroup );
    MPI_Group_free( &group );
    MPI_Win_free( &win );
    MPI_Finalize();
    return errs;
}

