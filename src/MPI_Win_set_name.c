/* 
MPI_Win_set_name

   Set the print name for an MPI RMA window
int MPI_Win_set_name(
  MPI_Win win,
  char *win_name
);

int MPI_Win_set_name(
  MPI_Win win,
  wchar_t *win_name
);

Parameters

   win
          [in] window whose identifier is to be set (handle)

   win_name
          [in] the character string which is remembered as the name
          (string)

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

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[] )
{
    static char actbuf[1024];
    int errs = 0;
    MPI_Win win;
    int namelen;
    char name[MPI_MAX_OBJECT_NAME], nameout[MPI_MAX_OBJECT_NAME];
    MPI_Init( &argc, &argv );
    MPI_Win_create( actbuf, 1024, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win );
    strcpy( name, "win-0" );
    MPI_Win_set_name( win, name );
    nameout[0] = 0;
    MPI_Win_get_name( win, nameout, &namelen );
    if (strcmp( name, nameout )) {
        errs++;
        printf( "Unexpected name, was %s but should be %s\n", nameout, name );
    }
    MPI_Win_free(&win);
    MPI_Finalize();
    return errs;
}

