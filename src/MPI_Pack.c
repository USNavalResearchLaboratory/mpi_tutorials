/* 
MPI_Pack

   Packs a datatype into contiguous memory
int MPI_Pack(
  void *inbuf,
  int incount,
  MPI_Datatype datatype,
  void *outbuf,
  int outcount,
  int *position,
  MPI_Comm comm
);

Parameters

   inbuf
          [in] input buffer

   incount
          [in] input count

   datatype
          [in] datatype

   outbuf
          [in] output buffer

   outcount
          [in] output count

   position
          [in] position

   comm
          [in] communicator

Remarks

   Packs the message in the send buffer specified by inbuf, incount,
   datatype into the buffer space specified by outbuf and outcount. The
   input buffer can be any communication buffer allowed in MPI_SEND. The
   output buffer is a contiguous storage area containing outsize bytes,
   starting at the address outbuf (length is counted in bytes, not
   elements, as if it were a communication buffer for a message of type
   MPI_PACKED).

   The input value of position is the first location in the output buffer
   to be used for packing. position is incremented by the size of the
   packed message, and the output value of position is the first location
   in the output buffer following the locations occupied by the packed
   message. The comm argument is the communicator that will be
   subsequently used for sending the packed message.

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

int main(int argc, char *argv[])
{
    int rank, size;
    int i;
    char c[100];
    char buffer[110];
    int position = 0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2)
    {
        printf("Please run with 2 processes.\n");
	fflush(stdout);
        MPI_Finalize();
        return 1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
    {
        for (i=0; i<100; i++)
            c[i] = i;
        i = 123;
	
        MPI_Pack(&i, 1, MPI_INT, buffer, 110, &position, MPI_COMM_WORLD);
        MPI_Pack(c, 100, MPI_CHAR, buffer, 110, &position, MPI_COMM_WORLD);
        MPI_Send(buffer, position, MPI_PACKED, 1, 100, MPI_COMM_WORLD);
    }
    if (rank == 1)
    {
        MPI_Recv(buffer, 110, MPI_PACKED, 0, 100, MPI_COMM_WORLD, &status);
        MPI_Unpack(buffer, 110, &position, &i, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, 110, &position, c, 100, MPI_CHAR, MPI_COMM_WORLD);
        printf("i=%d\nc[0] = %d\n...\nc[99] = %d\n", i, (int)c[0], (int)c[99]);
	fflush(stdout);
    }
    
    MPI_Finalize();
    return 0;
}

