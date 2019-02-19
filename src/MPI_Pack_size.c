/* 
MPI_Pack_size

   Returns the upper bound on the amount of space needed to pack a message
int MPI_Pack_size(
  int incount,
  MPI_Datatype datatype,
  MPI_Comm comm,
  int *size
);

Parameters

   incount
          [in] count argument to packing call (integer)

   datatype
          [in] datatype argument to packing call (handle)

   comm
          [in] communicator argument to packing call (handle)

   size
          [out] upper bound on size of packed message, in bytes (integer)

Remarks

   The MPI standard document describes this in terms of MPI_Pack, but it
   applies to both MPI_Pack and MPI_Unpack. That is, the value size is the
   maximum that is needed by either MPI_Pack or MPI_Unpack.

   Rationale.

   The call returns an upper bound, rather than an exact bound, since the
   exact amount of space needed to pack the message may depend on the
   context (e.g., first message packed in a packing unit may take more
   space).

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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int a[100][100], b[100][100];
    MPI_Datatype row, xpose;
    MPI_Aint sizeofint;
    int err, errs = 0;
    int bufsize, position = 0;
    void *buffer;
    int i, j;
    /* Initialize a to some known values. */
    for(i = 0; i < 100; i++) {
        for(j = 0; j < 100; j++) {
            a[i][j] = i*1000+j;
            b[i][j] = -1;
        }
    }

    MPI_Init(&argc, &argv);

    MPI_Type_extent(MPI_INT, &sizeofint);

    /* Create datatypes. */
    MPI_Type_vector(100, 1, 100, MPI_INT, &row);
    MPI_Type_hvector(100, 1, sizeofint, row, &xpose);
    MPI_Type_commit(&xpose);

    /* Pack it. */
    MPI_Pack_size(1, xpose, MPI_COMM_WORLD, &bufsize);
    buffer = (char *) malloc((unsigned) bufsize);

    /* To improve reporting of problems about operations, we change the
error handler to errors return */
    MPI_Comm_set_errhandler( MPI_COMM_WORLD, MPI_ERRORS_RETURN );

    err = MPI_Pack(a, 1, xpose, buffer, bufsize, &position,
MPI_COMM_WORLD);

    /* Unpack the buffer into b. */
    position = 0;
    err = MPI_Unpack(buffer, bufsize, &position, b, 100*100, MPI_INT,
MPI_COMM_WORLD);

    for (i = 0; i < 100; i++) {
        for (j = 0; j < 100; j++) {
            if(b[i][j] != a[j][i]) {
                errs++;
                fprintf(stderr, "b[%d][%d] = %d, should be %d\n", i, j,
b[i][j], a[j][i]);fflush(stderr);
            }
        }
    }
    MPI_Type_free(&xpose);
    MPI_Type_free(&row);

    MPI_Finalize();
    return 0;
}

