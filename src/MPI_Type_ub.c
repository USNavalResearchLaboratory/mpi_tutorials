/* 
MPI_Type_ub

   Returns the upper bound of a datatype
int MPI_Type_ub(
  MPI_Datatype datatype,
  MPI_Aint *displacement
);

Parameters

   datatype
          [in] datatype (handle)

   displacement
          [out] displacement of upper bound from origin, in bytes
          (integer)

Deprecated Function

   The MPI-2 standard deprecated a number of routines because MPI-2
   provides better versions. This routine is one of those that was
   deprecated. The routine may continue to be used, but new code should
   use the replacement routine. The replacement for this routine is
   MPI_Type_get_extent

Thread and Interrupt Safety

   This routine is both thread- and interrupt-safe. This means that this
   routine may safely be used by multiple threads and from within a signal
   handler.

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
#include <string.h>

int main(int argc, char **argv)
{
    int mpi_err, errs = 0, size;
    MPI_Aint lb, ub, extent;
    MPI_Datatype type;
    struct { float a; int b; } foo;
    MPI_Init(&argc, &argv);

    type = MPI_INT;
    MPI_Type_size(type, &size);
    if (size != sizeof(int)) {
        fprintf(stderr, "MPI_Type_size of MPI_INT incorrect size (%d); "
		"should be %d.\n", size, (int) sizeof(int));
	fflush(stderr);
        errs++;
    }

    MPI_Type_get_extent(type, &lb, &extent);
    if (extent != sizeof(int)) {
        fprintf(stderr, "MPI_Type_get_extent of MPI_INT returned "
		"incorrect extent (%d); should be %d.\n",
		(int) extent, (int) sizeof(int));
	fflush(stderr);
        errs++;
    }
    if (lb != 0) {
        fprintf(stderr, "MPI_Type_get_extent of MPI_INT returned incorrect "
		"lb (%d); should be 0.\n", (int) lb);
	fflush(stderr);
        errs++;
    }

    MPI_Type_ub(type, &ub);
    if (ub != extent - lb) {
        fprintf(stderr, "MPI_Type_ub of MPI_INT returned incorrect ub "
		"(%d); should be %d.\n", (int) ub, (int) (extent - lb));
	fflush(stderr);
        errs++;
    }

    type = MPI_FLOAT_INT;
    MPI_Type_size(type, &size);
    if (size != sizeof(float) + sizeof(int)) {
        fprintf(stderr, "MPI_Type_size of MPI_FLOAT_INT returned "
		"incorrect size (%d); should be %d.\n", size,
		(int) (sizeof(float) + sizeof(int)));
	fflush(stderr);
        errs++;
    }

    MPI_Type_get_extent(type, &lb, &extent);
    if (extent != sizeof(foo)) {
        fprintf(stderr, "MPI_Type_get_extent of MPI_FLOAT_INT returned "
		"incorrect extent (%d); should be %d.\n",
		(int) extent, (int) sizeof(foo));
	fflush(stderr);
        errs++;
    }
    if (lb != 0) {
        fprintf(stderr, "MPI_Type_get_extent of MPI_FLOAT_INT returned "
		"incorrect lb (%d); should be 0.\n", (int) lb);
	fflush(stderr);
        errs++;
    }

    MPI_Type_ub(type, &ub);
    if (ub != extent - lb) {
        fprintf(stderr, "MPI_Type_ub of MPI_FLOAT_INT returned incorrect "
		"ub (%d); should be %d.\n", (int) ub, (int) (extent -lb));
	fflush(stderr);
        errs++;
    }

    MPI_Finalize();
    return errs;
}

