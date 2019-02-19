/* 
MPI_File_iread_at

   Nonblocking read using explict offset
int MPI_File_read_at(
  MPI_File mpi_fh,
  MPI_Offset offset,
  void *buf,
  int count,
  MPI_Datatype datatype,
  MPI_Request *request
);

Parameters

   mpi_fh
          [in] file handle (handle)

   offset
          [in] file offset (nonnegative integer)

   buf
          [out] initial address of buffer (choice)

   count
          [in] number of elements in buffer (nonnegative integer)

   datatype
          [in] datatype of each buffer element (handle)

   request
          [out] request object (handle)

Remarks

   MPI_FILE_READ_AT starts a non-blocking read of a file beginning at the
   position specified by offset.

   If MPI_MODE_SEQUENTIAL mode was specified when the file was opened, it
   is erroneous to call this routine.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* tests noncontiguous reads/writes using nonblocking I/O */
#define SIZE 5000

int main(int argc, char **argv)
{
    int *buf, i, mynod, nprocs, len, b[3];
    int errs=0;
    MPI_Aint d[3];
    MPI_File fh;
    MPI_Status status;
    char *filename;
    MPI_Datatype typevec, newtype, t[3];
    MPI_Request req;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &mynod);
    if (nprocs != 2) {
        fprintf(stderr, "Run this program on two processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* process 0 takes the file name as a command-line argument and
       broadcasts it to other processes */
    if (!mynod) {
        i = 1;
        while ((i < argc) && strcmp("-fname", *argv)) {
            i++;
            argv++;
        }
        if (i >= argc) {
            len = 8;
            filename = (char *)malloc(len + 10);
            strcpy( filename, "testfile" );
        }
        else {
            argv++;
            len = (int)strlen(*argv);
            filename = (char *) malloc(len+1);
            strcpy(filename, *argv);
        }
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(filename, len+1, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
    else {
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
        filename = (char *) malloc(len+1);
        MPI_Bcast(filename, len+1, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    buf = (int *) malloc(SIZE*sizeof(int));
    MPI_Type_vector(SIZE/2, 1, 2, MPI_INT, &typevec);
    b[0] = b[1] = b[2] = 1;
    d[0] = 0;
    d[1] = mynod*sizeof(int);
    d[2] = SIZE*sizeof(int);
    t[0] = MPI_LB;
    t[1] = typevec;
    t[2] = MPI_UB;
    MPI_Type_struct(3, b, d, t, &newtype);
    MPI_Type_commit(&newtype);
    MPI_Type_free(&typevec);

    if (mynod == 0) {
        fprintf(stderr, "\ntesting noncontiguous in memory, noncontiguous"
		"in file using nonblocking I/O\n");
	fflush(stderr);
        MPI_File_delete(filename, MPI_INFO_NULL);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE |
		  MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    MPI_File_set_view(fh, 0, MPI_INT, newtype, "native", MPI_INFO_NULL);
    for (i=0; i<SIZE; i++) buf[i] = i + mynod*SIZE;
    MPI_File_iwrite(fh, buf, 1, newtype, &req);
    MPI_Wait(&req, &status);

    MPI_Barrier(MPI_COMM_WORLD);

    for (i=0; i<SIZE; i++) buf[i] = -1;
    MPI_File_iread_at(fh, 0, buf, 1, newtype, &req);
    MPI_Wait(&req, &status);
    for (i=0; i<SIZE; i++) {
        if (!mynod) {
            if ((i%2) && (buf[i] != -1)) {
                errs++;
                fprintf(stderr, "Process %d: buf %d is %d, should be -1\n",
			mynod, i, buf[i]);
		fflush(stderr);
            }
            if (!(i%2) && (buf[i] != i)) {
                errs++;
                fprintf(stderr, "Process %d: buf %d is %d, should be %d\n",
			mynod, i, buf[i], i);
		fflush(stderr);
            }
        }
        else {
            if ((i%2) && (buf[i] != i + mynod*SIZE)) {
                errs++;
                fprintf(stderr, "Process %d: buf %d is %d, should be %d\n",
			mynod, i, buf[i], i + mynod*SIZE);
		fflush(stderr);
            }
            if (!(i%2) && (buf[i] != -1)) {
                errs++;
                fprintf(stderr, "Process %d: buf %d is %d, should be -1\n",
			mynod, i, buf[i]);
		fflush(stderr);
            }
        }
    }
    MPI_File_close(&fh);

    MPI_Barrier(MPI_COMM_WORLD);

    if (!mynod) {
        fprintf(stderr, "\ntesting noncontiguous in memory, contiguous "
		"in file using nonblocking I/O\n");
	fflush(stderr);
        MPI_File_delete(filename, MPI_INFO_NULL);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE |
		  MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    for (i=0; i<SIZE; i++) buf[i] = i + mynod*SIZE;
    MPI_File_iwrite_at(fh, mynod*(SIZE/2)*sizeof(int), buf, 1, newtype, &req);
    MPI_Wait(&req, &status);

    MPI_Barrier(MPI_COMM_WORLD);

    for (i=0; i<SIZE; i++) buf[i] = -1;
    MPI_File_iread_at(fh, mynod*(SIZE/2)*sizeof(int), buf, 1, newtype, &req);
    MPI_Wait(&req, &status);
    for (i=0; i<SIZE; i++) {
        if (!mynod) {
            if ((i%2) && (buf[i] != -1)) {
                errs++;
                fprintf(stderr, "Process %d: buf %d is %d, should be -1\n",
			mynod, i, buf[i]);
		fflush(stderr);
            }
            if (!(i%2) && (buf[i] != i)) {
                errs++;
                fprintf(stderr, "Process %d: buf %d is %d, should be %d\n",
			mynod, i, buf[i], i);
		fflush(stderr);
            }
        }
        else {
            if ((i%2) && (buf[i] != i + mynod*SIZE)) {
                errs++;
                fprintf(stderr, "Process %d: buf %d is %d, should be %d\n",
			mynod, i, buf[i], i + mynod*SIZE);
		fflush(stderr);
            }
            if (!(i%2) && (buf[i] != -1)) {
                errs++;
                fprintf(stderr, "Process %d: buf %d is %d, should be -1\n",
			mynod, i, buf[i]);
		fflush(stderr);
            }
        }
    }
    MPI_File_close(&fh);

    MPI_Barrier(MPI_COMM_WORLD);

    if (!mynod) {
        fprintf(stderr, "\ntesting contiguous in memory, noncontiguous "
		"in file using nonblocking I/O\n");
	fflush(stderr);
        MPI_File_delete(filename, MPI_INFO_NULL);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE |
		  MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    MPI_File_set_view(fh, 0, MPI_INT, newtype, "native", MPI_INFO_NULL);
    for (i=0; i<SIZE; i++) buf[i] = i + mynod*SIZE;
    MPI_File_iwrite(fh, buf, SIZE, MPI_INT, &req);
    MPI_Wait(&req, &status);

    MPI_Barrier(MPI_COMM_WORLD);

    for (i=0; i<SIZE; i++) buf[i] = -1;
    MPI_File_iread_at(fh, 0, buf, SIZE, MPI_INT, &req);
    MPI_Wait(&req, &status);
    for (i=0; i<SIZE; i++) {
        if (!mynod) {
            if (buf[i] != i) {
                errs++;
                fprintf(stderr, "Process %d: buf %d is %d, should be %d\n",
			mynod, i, buf[i], i);
            }
        }
        else {
            if (buf[i] != i + mynod*SIZE) {
                errs++;
                fprintf(stderr, "Process %d: buf %d is %d, should be %d\n",
			mynod, i, buf[i], i + mynod*SIZE);
            }
        }
    }
    MPI_File_close(&fh);

    MPI_Type_free(&newtype);
    free(buf);
    free(filename);

    MPI_Finalize();
    return errs;
}

