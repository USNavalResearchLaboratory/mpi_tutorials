/* 
MPI_File_close

   Closes a file
int MPI_File_close(
  MPI_File *mpi_fh
);

Parameters

   fh
          [in] file handle (handle)

Remarks

   MPI_FILE_CLOSE first synchronizes file state (equivalent to performing
   an MPI_FILE_SYNC), then closes the file associated with fh. The file is
   deleted if it was opened with access mode MPI_MODE_DELETE_ON_CLOSE
   (equivalent to performing an MPI_FILE_DELETE). MPI_FILE_CLOSE is a
   collective routine.

   Advice to users.

   If the file is deleted on close, and there are other processes
   currently accessing the file, the status of the file and the behavior
   of future accesses by these processes are implementation dependent. (
   End of advice to users.)
   The user is responsible for ensuring that all outstanding nonblocking
   requests and split collective operations associated with fh made by a
   process have completed before that process calls MPI_FILE_CLOSE.

   The MPI_FILE_CLOSE routine deallocates the file handle object and sets
   fh to MPI_FILE_NULL.

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
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    rc = MPI_File_open( MPI_COMM_WORLD, "temp", MPI_MODE_RDWR |
MPI_MODE_DELETE_ON_CLOSE | MPI_MODE_CREATE, MPI_INFO_NULL, &cFile );
    if (rc) {
        printf( "Unable to open file \"temp\"\n" );fflush(stdout);
    }
    else {
        MPI_File_close( &cFile );
    }
    MPI_Finalize();
    return 0;
}

