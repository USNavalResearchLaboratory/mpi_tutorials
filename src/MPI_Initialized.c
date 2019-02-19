/* 
MPI_Initialized

   Indicates whether MPI_Init has been called.
int MPI_Initialized(
  int *flag
);

Parameters

   flag
          [out] Flag is true if MPI_Init or MPI_Init_thread has been
          called and false otherwise.

Remarks

   This routine may be used to determine whether MPI_INIT has been called.
   It is the only MPI-1 routine that may be called before MPI_INIT is
   called.

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

int main( int argc, char *argv[] )
{
    int flag;
    int error;
    
    flag = 0;
    MPI_Initialized(&flag);
    if (flag)
    {
        printf("MPI_Initialized returned true before MPI_Init.\n");
        return -1;
    }

    MPI_Init(&argc, &argv);

    flag = 0;
    MPI_Initialized(&flag);
    if (!flag)
    {
        printf("MPI_Initialized returned false after MPI_Init.\n");
        fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, error);
        return -1;
    }

    MPI_Finalize();
    return 0;
}

