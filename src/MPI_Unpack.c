/* 
MPI_Unpack

   Unpack a buffer according to a datatype into contiguous memory
int MPI_Unpack(
  void *inbuf,
  int insize,
  int *position,
  void *outbuf,
  int outcount,
  MPI_Datatype datatype,
  MPI_Comm comm
);

Parameters

   inbuf
          [in] input buffer start (choice)

   insize
          [in] size of input buffer, in bytes (integer)

   position
          [in] current position in bytes (integer)

   outbuf
          [out] output buffer start (choice)

   outcount
          [in] number of items to be unpacked (integer)

   datatype
          [in] datatype of each output data item (handle)

   comm
          [in] communicator for packed message (handle)

Remarks

   Unpacks a message into the receive buffer specified by outbuf,
   outcount, datatype from the buffer space specified by inbuf and insize.
   The output buffer can be any communication buffer allowed in MPI_RECV.
   The input buffer is a contiguous storage area containing insize bytes,
   starting at address inbuf. The input value of position is the first
   location in the input buffer occupied by the packed message. position
   is incremented by the size of the packed message, so that the output
   value of position is the first location in the input buffer after the
   locations occupied by the message that was unpacked. comm is the
   communicator used to receive the packed message.

   Advice to users.

   Note the difference between MPI_RECV and MPI_UNPACK: in MPI_RECV, the
   count argument specifies the maximum number of items that can be
   received. The actual number of items received is determined by the
   length of the incoming message. In MPI_UNPACK, the count argument
   specifies the actual number of items that are unpacked; the "size" of
   the corresponding message is the increment in position. The reason for
   this change is that the "incoming message size" is not predetermined
   since the user decides how much to unpack; nor is it easy to determine
   the "message size" from the number of items to be unpacked. In fact, in
   a heterogeneous system, this number may not be determined a priori. (
   End of advice to users.)
   To understand the behavior of pack and unpack, it is convenient to
   think of the data part of a message as being the sequence obtained by
   concatenating the successive values sent in that message. The pack
   operation stores this sequence in the buffer space, as if sending the
   message to that buffer. The unpack operation retrieves this sequence
   from buffer space, as if receiving a message from that buffer. (It is
   helpful to think of internal Fortran files or sscanf in C, for a
   similar function.)

   Several messages can be successively packed into one packing unit. This
   is effected by several successive related calls to MPI_PACK, where the
   first call provides position = 0, and each successive call inputs the
   value of position that was output by the previous call, and the same
   values for outbuf, outcount and comm. This packing unit now contains
   the equivalent information that would have been stored in a message by
   one send call with a send buffer that is the "concatenation" of the
   individual send buffers.

   A packing unit can be sent using type MPI_PACKED. Any point to point or
   collective communication function can be used to move the sequence of
   bytes that forms the packing unit from one process to another. This
   packing unit can now be received using any receive operation, with any
   datatype: the type matching rules are relaxed for messages sent with
   type MPI_PACKED.

   A message sent with any type (including MPI_PACKED) can be received
   using the type MPI_PACKED. Such a message can then be unpacked by calls
   to MPI_UNPACK.

   A packing unit (or a message created by a regular, "typed" send) can be
   unpacked into several successive messages. This is effected by several
   successive related calls to MPI_UNPACK, where the first call provides
   position = 0, and each successive call inputs the value of position
   that was output by the previous call, and the same values for inbuf,
   insize and comm.

   The concatenation of two packing units is not necessarily a packing
   unit; nor is a substring of a packing unit necessarily a packing unit.
   Thus, one cannot concatenate two packing units and then unpack the
   result as one packing unit; nor can one unpack a substring of a packing
   unit as a separate packing unit. Each packing unit, that was created by
   a related sequence of pack calls, or by a regular send, must be
   unpacked as a unit, by a sequence of related unpack calls.

   Rationale.

   The restriction on "atomic" packing and unpacking of packing units
   allows the implementation to add at the head of packing units
   additional information, such as a description of the sender
   architecture (to be used for type conversion, in a heterogeneous
   environment)

Thread and Interrupt Safety

   This routine is thread-safe. This means that this routine may be safely
   used by multiple threads without the need for any user-provided thread
   locks. However, the routine is not interrupt safe. Typically, this is
   due to the use of memory allocation routines such as malloc or other
   non-MPICH runtime routines that are themselves not interrupt-safe.

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

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

   MPI_ERR_COUNT
          Invalid count argument. Count arguments must be non-negative; a
          count of zero is often valid.

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

See Also

   MPI_Pack, MPI_Pack_size

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

