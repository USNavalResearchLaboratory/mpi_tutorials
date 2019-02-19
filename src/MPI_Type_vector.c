/* 
MPI_Type_vector

   Creates a vector (strided) datatype
int MPI_Type_vector(
  int count,
  int blocklength,
  int stride,
  MPI_Datatype old_type,
  MPI_Datatype *newtype_p
);

Parameters

   count
          [in] number of blocks (nonnegative integer)

   blocklength
          [in] number of elements in each block (nonnegative integer)

   stride
          [in] number of elements between start of each block (integer)

   oldtype
          [in] old datatype (handle)

   newtype_p
          [out] new datatype (handle)

Remarks

   The function MPI_TYPE_VECTOR is a more general constructor that allows
   replication of a datatype into locations that consist of equally spaced
   blocks. Each block is obtained by concatenating the same number of
   copies of the old datatype. The spacing between blocks is a multiple of
   the extent of the old datatype.

   Example Assume, again, that oldtype has type map [img22.gif] with
   extent 16. A call to MPI_TYPE_VECTOR( 2, 3, 4, oldtype, newtype) will
   create the datatype with type map,

   [img23.gif]


   [img24.gif]

   That is, two blocks with three copies each of the old type, with a
   stride of 4 elements ( [img25.gif] bytes) between the blocks.

   Example A call to MPI_TYPE_VECTOR(3, 1, -2, oldtype, newtype) will
   create the datatype,

   [img26.gif]


   In general, assume that oldtype has type map,

   [img27.gif]

   with extent ex. Let bl be the blocklength. The newly created datatype
   has a type map with [img28.gif]

   entries:

   [img29.gif]


   [img30.gif]


   [img31.gif]


   [img32.gif]


   [img33.gif]


   [img34.gif]


   [img35.gif]


   [img36.gif]


   [img37.gif]

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int rank, size, i;
    MPI_Datatype type, type2;
    int buffer[24];
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
    MPI_Type_vector(3, 2, 3, type2, &type);
    MPI_Type_commit(&type);

    if (rank == 0)
    {
        for (i=0; i<24; i++)
            buffer[i] = i;
        MPI_Send(buffer, 1, type, 1, 123, MPI_COMM_WORLD);
    }

    if (rank == 1)
    {
        for (i=0; i<24; i++)
            buffer[i] = -1;
        MPI_Recv(buffer, 1, type, 0, 123, MPI_COMM_WORLD, &status);
        for (i=0; i<24; i++)
            printf("buffer[%d] = %d\n", i, buffer[i]);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}

