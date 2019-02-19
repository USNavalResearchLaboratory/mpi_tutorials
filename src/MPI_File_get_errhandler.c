/* 
MPI_File_get_errhandler

   Get the error handler attached to a file
int MPI_File_get_errhandler(
  MPI_File file,
  MPI_Errhandler *errhandler
);

Parameters

   file
          [in] MPI file (handle)

   errhandler
          [out] handler currently associated with file (handle)

Remarks

   Retrieves the error handler currently associated with a file.

Thread and Interrupt Safety

   This routine is thread and interrupt safe only if no MPI routine that
   updates or frees the same MPI object may be called concurrently with
   this routine.

   The MPI standard defined a thread-safe interface but this does not mean
   that all routines may be called without any thread locks. For example,
   two threads must not attempt to change the contents of the same
   MPI_Info object concurrently. The user is responsible in this case for
   using some mechanism, such as thread locks, to ensure that only one
   thread at a time makes use of this routine.

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include "mpi.h"
#include <stdio.h>
int main( int argc, char *argv[] )
{
    MPI_Fint handleA, handleB;
    int rc;
    int errs = 0;
    int rank;
    MPI_File cFile;
    MPI_Errhandler eh;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    rc = MPI_File_open( MPI_COMM_WORLD, "temp", MPI_MODE_RDWR |
MPI_MODE_CREATE, MPI_INFO_NULL, &cFile );
    if (rc) {
        printf( "Unable to open file \"temp\"\n" );fflush(stdout);
    }
    else {
        rc = MPI_File_get_errhandler( cFile, &eh );
        rc = MPI_File_close( &cFile );
        if (rank == 0) {
            rc = MPI_File_delete( "temp", MPI_INFO_NULL );
            if (rc) {
                printf( "Unable to delete file \"temp\"\n"
);fflush(stdout);
            }
        }
    }
    MPI_Finalize();
    return 0;
}

