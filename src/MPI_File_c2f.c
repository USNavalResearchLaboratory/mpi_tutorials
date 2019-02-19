/* 
MPI_File_c2f

   Translates a C file handle to a Fortran file handle
MPI_Fint MPI_File_c2f(
  MPI_File mpi_fh
);

Parameters

   mpi_fh
          [in] C file handle (handle)

Return Value

   Fortran file handle (integer)

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
			MPI_MODE_DELETE_ON_CLOSE | MPI_MODE_CREATE,
			MPI_INFO_NULL, &cFile );
    if (rc) {
        printf( "Unable to open file \"temp\"\n" );
	fflush(stdout);
    }
    else {
        handleA = MPI_File_c2f( cFile );
        handleB = MPI_File_c2f( cFile );
        if (handleA != handleB) {
            printf( "MPI_File_c2f does not give the same handle twice on the same MPI_File\n" );
	    fflush(stdout);
        }
    }
    
    MPI_File_close( &cFile );

    MPI_Finalize();
    return 0;
}

