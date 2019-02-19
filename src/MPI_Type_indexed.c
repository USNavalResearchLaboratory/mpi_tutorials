/* 
MPI_Type_indexed

   Creates an indexed datatype
int MPI_Type_indexed(
  int count,
  int blocklens[],
  int indices[],
  MPI_Datatype old_type,
  MPI_Datatype *newtype
);

Parameters

   count
          [in] number of blocks -- also number of entries in indices and
          blocklens

   blocklens
          [in] number of elements in each block (array of nonnegative
          integers)

   indices
          [in] displacement of each block in multiples of old_type (array
          of integers)

   old_type
          [in] old datatype (handle)

   newtype
          [out] new datatype (handle)

Remarks

   The function MPI_TYPE_INDEXED allows replication of an old datatype
   into a sequence of blocks (each block is a concatenation of the old
   datatype), where each block can contain a different number of copies
   and have a different displacement. All block displacements are
   multiples of the old type extent.

   Example:

   Let oldtype have type map [img49.gif] with extent 16. Let B = (3, 1)
   and let D = (4, 0). A call to MPI_TYPE_INDEXED(2, B, D, oldtype,
   newtype) returns a datatype with type map,

   [img50.gif]


   [img51.gif]

   That is, three copies of the old type starting at displacement 64, and
   one copy starting at displacement 0.

   In general, assume that oldtype has type map,

   [img52.gif]

   with extent ex. Let B be the array_of_blocklength argument and D be the
   array_of_displacements argument. The newly created datatype has
   [img53.gif] entries:

   [img54.gif]


   [img55.gif]


   [img56.gif]


   [img57.gif]


   [img58.gif]


   A call to MPI_TYPE_VECTOR(count, blocklength, stride, oldtype, newtype)
   is equivalent to a call to MPI_TYPE_INDEXED(count, B, D, oldtype,
   newtype) where

   [img59.gif]

   and

   [img60.gif]

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

   The indices are displacements, and are based on a zero origin. A common
   error is to do something like to following
    integer a(100)
    integer blens(10), indices(10)
    do i=1,10
         blens(i)   = 1
10       indices(i) = 1 + (i-1)*10
    call MPI_TYPE_INDEXED(10,blens,indices,MPI_INTEGER,newtype,ierr)
    call MPI_TYPE_COMMIT(newtype,ierr)
    call MPI_SEND(a,1,newtype,...)

   expecting this to send a(1),a(11),... because the indices have values
   1,11,.... Because these are displacements from the beginning of a, it
   actually sends a(1+1),a(1+11),....

   If you wish to consider the displacements as indices into a Fortran
   array, consider declaring the Fortran array with a zero origin
    integer a(0:99)

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

   MPI_ERR_COUNT
          Invalid count argument. Count arguments must be non-negative; a
          count of zero is often valid.

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

   MPI_ERR_INTERN
          This error is returned when some part of the MPICH
          implementation is unable to acquire memory.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
int main(int argc, char *argv[])
{
    int rank, size, i;
    MPI_Datatype type, type2;
    int blocklen[3] = { 2, 3, 1 };
    int displacement[3] = { 0, 3, 8 };
    int buffer[27];
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size < 2)
    {
        printf("Please run with 2 processes.\n");
        MPI_Finalize();
        return 1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Type_contiguous(3, MPI_INT, &type2);
    MPI_Type_commit(&type2);
    MPI_Type_indexed(3, blocklen, displacement, type2, &type);
    MPI_Type_commit(&type);
    if (rank == 0)
    {
        for (i=0; i<27; i++)
            buffer[i] = i;
        MPI_Send(buffer, 1, type, 1, 123, MPI_COMM_WORLD);
    }
    if (rank == 1)
    {
        for (i=0; i<27; i++)
            buffer[i] = -1;
        MPI_Recv(buffer, 1, type, 0, 123, MPI_COMM_WORLD, &status);
        for (i=0; i<27; i++)
            printf("buffer[%d] = %d\n", i, buffer[i]);
        fflush(stdout);
    }
    MPI_Finalize();
    return 0;
}

