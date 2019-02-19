/* 
MPI_Type_create_hindexed

   Create a datatype for an indexed datatype with displacements in bytes
int MPI_Type_create_hindexed(
  int count,
  int blocklengths[],
  MPI_Aint displacements[],
  MPI_Datatype oldtype,
  MPI_Datatype *newtype
);

Parameters

   count
          [in] number of blocks --- also number of entries in
          displacements and blocklengths (integer)

   blocklengths
          [in] number of elements in each block (array of nonnegative
          integers)

   displacements
          [in] byte displacement of each block (array of integer)

   oldtype
          [in] old datatype (handle)

   newtype
          [out] new datatype (handle)

Remarks

   The function MPI_TYPE_CREATE_HINDEXED is identical to
   MPI_TYPE_CREATE_INDEXED, except that block displacements in
   array_of_displacements are specified in bytes, rather than in multiples
   of the oldtype extent.

   Assume that oldtype has type map,

   [img61.gif]

   with extent ex. Let B be the array_of_blocklength argument and D be the
   array_of_displacements argument. The newly created datatype has a type
   map with [img62.gif] entries:

   [img63.gif]


   [img64.gif]


   [img65.gif]


   [img66.gif]


   [img67.gif]


   [img68.gif]

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

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

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
    MPI_Aint displacement[3] = { 0, 7*sizeof(int), 18*sizeof(int) };
    int buffer[21];
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
    MPI_Type_create_hindexed(3, blocklen, displacement, type2, &type);
    MPI_Type_commit(&type);
    if (rank == 0)
    {
        for (i=0; i<21; i++)
            buffer[i] = i;
        MPI_Send(buffer, 1, type, 1, 123, MPI_COMM_WORLD);
    }
    if (rank == 1)
    {
        for (i=0; i<21; i++)
            buffer[i] = -1;
        MPI_Recv(buffer, 1, type, 0, 123, MPI_COMM_WORLD, &status);
        for (i=0; i<21; i++)
            printf("buffer[%d] = %d\n", i, buffer[i]);
        fflush(stdout);
    }
    MPI_Finalize();
    return 0;
}

