/* 
MPI_Finalized

   Indicates whether MPI_Finalize has been called.
int MPI_Finalized(
  int *flag
);

Parameters

   flag
          [out] Flag is true if MPI_Finalize has been called and false
          otherwise. (logical)

Remarks

   This routine returns true if MPI_FINALIZE has completed. It is legal to
   call MPI_FINALIZED before MPI_INIT and after MPI_FINALIZE.

   One of the goals of MPI was to allow for layered libraries. In order
   for a library to do this cleanly, it needs to know if MPI is active. In
   MPI-1 the function MPI_INITIALIZED was provided to tell if MPI had been
   initialized. The problem arises in knowing if MPI has been finalized.
   Once MPI has been finalized it is no longer active and cannot be
   restarted. A library needs to be able to determine this to act
   accordingly.

   Advice to users.

   MPI is "active" and it is thus safe to call MPI functions if MPI_INIT
   has completed and MPI_FINALIZE has not completed. If a library has no
   other way of knowing whether MPI is active or not, then it can use
   MPI_INITIALIZED and MPI_FINALIZED to determine this. For example, MPI
   is "active" in callback functions that are invoked during MPI_FINALIZE.

Thread and Interrupt Safety

   This routine is both thread- and interrupt-safe. This means that this
   routine may safely be used by multiple threads and from within a signal
   handler.

Notes for Fortran

   All MPI routines in Fortran (except for MPI_WTIME and MPI_WTICK) have
   an additional argument ierr at the end of the argument list. ierr is an
   integer and has the same meaning as the return value of the routine in
   C. In Fortran, MPI routines are subroutines, and are invoked with the
   call statement. All MPI objects (e.g., MPI_Datatype, MPI_Comm) are of
   type INTEGER in Fortran.

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

int main( int argc, char *argv[] )
{
    int flag;

    flag = 0;
    MPI_Finalized(&flag);
    if (flag)
    {
        printf("MPI_Finalized returned true before MPI_Init.\n");
        return -1;
    }

    MPI_Init(&argc, &argv);

    flag = 0;
    MPI_Finalized(&flag);
    if (flag)
    {
        printf("MPI_Finalized returned true before MPI_Finalize.\n");
        fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 1);
        return -1;
    }

    MPI_Finalize();

    flag = 0;
    MPI_Finalized(&flag);
    if (!flag)
    {
        printf("MPI_Finalized returned false after MPI_Finalize.\n");
        return -1;
    }
    return 0;
}

