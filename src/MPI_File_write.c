/* 
MPI_File_write

   Write using individual file pointer
int MPI_File_write(
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

   MPI_FILE_WRITE writes a file using the individual file pointer.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

/* Test reading and writing zero bytes (set status correctly) */
int main( int argc, char *argv[] )
{
    int errs = 0;
    int size, rank, i, *buf, count;
    MPI_File fh;
    MPI_Comm comm;
    MPI_Status status;

    MPI_Init( &argc, &argv );

    comm = MPI_COMM_WORLD;
    MPI_File_open( comm, "test.ord", MPI_MODE_RDWR | MPI_MODE_CREATE | MPI_MODE_DELETE_ON_CLOSE, MPI_INFO_NULL, &fh );
    MPI_Comm_size( comm, &size );
    MPI_Comm_rank( comm, &rank );
    buf = (int *)malloc( size * sizeof(int) );
    buf[0] = rank;

    /* Write to file */
    MPI_File_write_ordered( fh, buf, 1, MPI_INT, &status );
    MPI_Get_count( &status, MPI_INT, &count );
    if (count != 1) {
        errs++;
        fprintf( stderr, "Wrong count (%d) on write-ordered\n", count);
	fflush(stderr);
    }

    /* Set the individual pointer to 0, since we want to use a read_all */
    MPI_File_seek( fh, 0, MPI_SEEK_SET );

    /* Read nothing (check status) */
    memset( &status, 0xff, sizeof(MPI_Status) );
    MPI_File_read( fh, buf, 0, MPI_INT, &status );
    MPI_Get_count( &status, MPI_INT, &count );
    if (count != 0) {
        errs++;
        fprintf( stderr, "Count not zero (%d) on read\n", count);
	fflush(stderr);
    }

    /* Write nothing (check status) */
    memset( &status, 0xff, sizeof(MPI_Status) );
    MPI_File_write( fh, buf, 0, MPI_INT, &status );
    if (count != 0) {
        errs++;
        fprintf( stderr, "Count not zero (%d) on write\n", count);
	fflush(stderr);
    }

    /* Read shared nothing (check status) */
    MPI_File_seek_shared( fh, 0, MPI_SEEK_SET );
    /* Read nothing (check status) */
    memset( &status, 0xff, sizeof(MPI_Status) );
    MPI_File_read_shared( fh, buf, 0, MPI_INT, &status );
    MPI_Get_count( &status, MPI_INT, &count );
    if (count != 0) {
        errs++;
        fprintf( stderr, "Count not zero (%d) on read shared\n", count);
	fflush(stderr);
    }
    /* Write nothing (check status) */
    memset( &status, 0xff, sizeof(MPI_Status) );
    MPI_File_write_shared( fh, buf, 0, MPI_INT, &status );
    if (count != 0) {
        errs++;
        fprintf( stderr, "Count not zero (%d) on write\n", count);
	fflush(stderr);
    }

    MPI_Barrier( comm );

    MPI_File_seek_shared( fh, 0, MPI_SEEK_SET );
    for (i=0; i<size; i++) buf[i] = -1;
    MPI_File_read_ordered( fh, buf, 1, MPI_INT, &status );
    if (buf[0] != rank) {
        errs++;
        fprintf( stderr, "%d: buf = %d\n", rank, buf[0]);
	fflush(stderr);
    }

    free( buf );
    MPI_File_close( &fh );

    MPI_Finalize();
    return errs;
}

