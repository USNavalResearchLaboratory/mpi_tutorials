/* 
MPI_File_delete

   Deletes a file
int MPI_File_delete(
  char *filename,
  MPI_Info info
);

int MPI_File_delete(
  wchar_t *filename,
  MPI_Info info
);

Parameters

   filename
          [in] name of file to delete (string)

   info
          [in] info object (handle)

Remarks

   MPI_FILE_DELETE deletes the file identified by the file name filename.
   If the file does not exist, MPI_FILE_DELETE raises an error in the
   class MPI_ERR_NO_SUCH_FILE.

   The info argument can be used to provide information regarding file
   system specifics. The constant MPI_INFO_NULL refers to the null info,
   and can be used when no info needs to be specified.

   If a process currently has the file open, the behavior of any access to
   the file (as well as the behavior of any outstanding accesses) is
   implementation dependent. In addition, whether an open file is deleted
   or not is also implementation dependent. If the file is not deleted, an
   error in the class MPI_ERR_FILE_IN_USE or MPI_ERR_ACCESS will be
   raised. Errors are raised using the default error handler.

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
MPI_MODE_CREATE, MPI_INFO_NULL, &cFile );
    if (rc) {
        printf( "Unable to open file \"temp\"\n" );fflush(stdout);
    }
    else {
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

