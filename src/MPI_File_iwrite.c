/* 
MPI_File_iwrite

   Nonblocking write using individual file pointer
int MPI_File_iwrite(
  MPI_File mpi_fh,
  void *buf,
  int count,
  MPI_Datatype datatype,
  MPI_Request *request
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

   request
          [out] request object (handle)

Remarks

   MPI_FILE_IWRITE is a nonblocking version of the MPI_FILE_WRITE
   interface. MPI_FILE_WRITE writes a file using the individual file
   pointer.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE (65536)

/* Uses asynchronous I/O. Each process writes to separate files and
   reads them back. The file name is taken as a command-line argument,
   and the process rank is appended to it.*/

int main(int argc, char **argv)
{
    int *buf, i, rank, nints, len;
    char *filename, *tmp;
    int errs=0;
    MPI_File fh;
    MPI_Status status;
    MPI_Request request;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    /* process 0 takes the file name as a command-line argument and 
       broadcasts it to other processes */
    if (rank == 0)
    {
        i = 1;
        while ((i < argc) && strcmp("-fname", *argv)) {
            i++;
            argv++;
        }
        if (i >= argc) {
            /* Use a default filename of testfile */
            len = 8;
            filename = (char *)malloc(len + 10);
            strcpy( filename, "testfile" );
        }
        else {
            argv++;
            len = (int)strlen(*argv);
            filename = (char *) malloc(len+10);
            strcpy(filename, *argv);
        }
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(filename, len+10, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
    else {
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
        filename = (char *) malloc(len+10);
        MPI_Bcast(filename, len+10, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
    buf = (int *) malloc(SIZE);
    nints = SIZE/sizeof(int);
    for (i=0; i<nints; i++) buf[i] = rank*100000 + i;
    /* each process opens a separate file called filename.'myrank' */
    tmp = (char *) malloc(len+10);
    strcpy(tmp, filename);
    sprintf(filename, "%s.%d", tmp, rank);
    MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_CREATE |
		  MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    MPI_File_set_view(fh, 0, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
    MPI_File_iwrite(fh, buf, nints, MPI_INT, &request);
    MPI_Wait( &request, &status );
    MPI_File_close(&fh);
    /* reopen the file and read the data back */
    for (i=0; i<nints; i++) buf[i] = 0;
    MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_CREATE |
		  MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    MPI_File_set_view(fh, 0, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
    MPI_File_iread(fh, buf, nints, MPI_INT, &request);
    MPI_Wait( &request, &status );
    MPI_File_close(&fh);
    /* check if the data read is correct */
    for (i=0; i<nints; i++) {
        if (buf[i] != (rank*100000 + i)) {
            errs++;
            fprintf(stderr, "Process %d: error, read %d, should be %d\n",
		    rank, buf[i], rank*100000+i);
	    fflush(stderr);
        }
    }
    
    free(buf);
    free(filename);
    free(tmp);

    MPI_Finalize();
    return errs;
}

