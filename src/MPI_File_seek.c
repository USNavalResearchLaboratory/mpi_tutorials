/* 
MPI_File_seek

   Updates the individual file pointer
int MPI_File_seek(
  MPI_File mpi_fh,
  MPI_Offset offset,
  int whence
);

Parameters

   mpi_fh
          [in] file handle (handle)

   offset
          [in] file offset (integer)

   whence
          [in] update mode (state)

Remarks

   MPI_FILE_SEEK updates the individual file pointer according to whence,
   which has the following possible values:
     * MPI_SEEK_SET: the pointer is set to offset
     * MPI_SEEK_CUR: the pointer is set to the current pointer position
       plus offset
     * MPI_SEEK_END: the pointer is set to the end of file plus offset

   The offset can be negative, which allows seeking backwards. It is
   erroneous to seek to a negative position in the view.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
/* Test reading and writing ordered output */
int main( int argc, char *argv[] )
{
    int errs = 0;
    int size, rank, i, *buf;
    MPI_File fh;
    MPI_Comm comm;
    MPI_Status status;

    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    MPI_File_open( comm, "test.ord", MPI_MODE_RDWR | MPI_MODE_CREATE |
MPI_MODE_DELETE_ON_CLOSE, MPI_INFO_NULL, &fh );

    MPI_Comm_size( comm, &size );
    MPI_Comm_rank( comm, &rank );
    buf = (int *)malloc( size * sizeof(int) );
    buf[0] = rank;
    MPI_File_write_ordered( fh, buf, 1, MPI_INT, &status );

    /* make sure all writes finish before we seek/read */
    MPI_Barrier(comm);
    /* Set the individual pointer to 0, since we want to use a read_all
*/
    MPI_File_seek( fh, 0, MPI_SEEK_SET );
    MPI_File_read_all( fh, buf, size, MPI_INT, &status );
    for (i=0; i<size; i++) {
        if (buf[i] != i) {
            errs++;
            fprintf( stderr, "%d: buf[%d] = %d\n", rank, i, buf[i] );
            fflush(stderr);
        }
    }

    MPI_File_seek_shared( fh, 0, MPI_SEEK_SET );
    for (i=0; i<size; i++) buf[i] = -1;
    MPI_File_read_ordered( fh, buf, 1, MPI_INT, &status );
    if (buf[0] != rank) {
        errs++;
        fprintf( stderr, "%d: buf[0] = %d\n", rank, buf[0] );
        fflush(stderr);
    }

    free( buf );
    MPI_File_close( &fh );

    MPI_Finalize();
    return errs;
}

