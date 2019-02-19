/* 
MPI_File_write_all

   Collective write using individual file pointer
int MPI_File_write_all(
  MPI_File mpi_fh,
  void *buf,
  int count,
  MPI_Datatype datatype,
  MPI_Status *status
);

Parameters

   mpi_fh
          [in] file handle (handle)

   buf
          [in] initial address of buffer (choice)

   count
          [in] number of elements in buffer (nonnegative integer)

   datatype
          [in] datatype of each buffer element (handle)

   status
          [out] status object (Status)

Remarks

   MPI_FILE_WRITE_ALL is a collective version of the blocking
   MPI_FILE_WRITE interface.  MPI_FILE_WRITE writes a file using the
   individual file pointer.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

/* Test set_view with DISPLACEMENT_CURRENT */
int main( int argc, char *argv[] )
{
    int errs = 0, err;
    int size, rank, *buf;
    MPI_Offset offset;
    MPI_File fh;
    MPI_Comm comm;
    MPI_Status status;
    MPI_Init( &argc, &argv );
    /* This test reads a header then sets the view to every "size" int,
        using set view and current displacement. The file is first written
        using a combination of collective and ordered writes */
    comm = MPI_COMM_WORLD;
    err = MPI_File_open( comm, "test.ord", MPI_MODE_WRONLY |
			 MPI_MODE_CREATE, MPI_INFO_NULL, &fh );
    if (err)
    {
        MPI_Abort(MPI_COMM_WORLD, 911);
    }
    MPI_Comm_size( comm, &size );
    MPI_Comm_rank( comm, &rank );
    buf = (int *)malloc( size * sizeof(int) );
    buf[0] = size;
    err = MPI_File_write_all( fh, buf, 1, MPI_INT, &status );
    if (err) { errs++; }
    err = MPI_File_get_position( fh, &offset );
    if (err) { errs++; }
    err = MPI_File_seek_shared( fh, offset, MPI_SEEK_SET );
    if (err) { errs++; }
    buf[0] = rank;
    err = MPI_File_write_ordered( fh, buf, 1, MPI_INT, &status );
    if (err) { errs++; }
    err = MPI_File_close( &fh );
    if (err) { errs++; }
    /* Reopen the file as sequential */
    err = MPI_File_open( comm, "test.ord", MPI_MODE_RDONLY |
			 MPI_MODE_SEQUENTIAL | MPI_MODE_DELETE_ON_CLOSE,
			 MPI_INFO_NULL, &fh );
    if (err)
    {
        MPI_Abort(MPI_COMM_WORLD, 911);
    }
    if (rank == 0) {
        err = MPI_File_read_shared( fh, buf, 1, MPI_INT, &status );
        if (err) { errs++; }
        if (buf[0] != size) {
            errs++;
            fprintf(stderr, "Unexpected value for the header = %d, should be %d\n",
		    buf[0], size );
	    fflush(stderr);
        }
    }
    MPI_Barrier( comm );
    /* All processes must provide the same file view for MODE_SEQUENTIAL */
    err = MPI_File_set_view( fh, MPI_DISPLACEMENT_CURRENT, MPI_INT,
			     MPI_INT, "native", MPI_INFO_NULL );
    if (err) { errs++; }
    buf[0] = -1;
    err = MPI_File_read_ordered( fh, buf, 1, MPI_INT, &status );
    if (err) { errs++; }
    if (buf[0] != rank) {
        errs++;
        fprintf( stderr, "%d: buf[0] = %d\n", rank, buf[0] );
	fflush(stderr);
    }
    free( buf );
    err = MPI_File_close( &fh );
    if (err) { errs++; }

    MPI_Finalize();
    return errs;
}

