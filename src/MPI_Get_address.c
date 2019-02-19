/* 
MPI_Get_address

   Get the address of a location in memory
int MPI_Get_address(
  void *location,
  MPI_Aint *address
);

Parameters

   location
          [in] location in caller memory (choice)

   address
          [out] address of location (address)

Remarks

   This routine is provided for both the Fortran and C programmers. On
   many systems, the address returned by this routine will be the same as
   produced by the C & operator, but this is not required in C and may not
   be true of systems with word- rather than byte-oriented instructions or
   systems with segmented address spaces.

   This routine should be used instead of MPI_Address.

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

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    int errs = 0;
    int buf[10];
    MPI_Aint a1, a2;

    MPI_Init( &argc, &argv );
    MPI_Get_address( &buf[0], &a1 );
    MPI_Get_address( &buf[1], &a2 );

    if ((int)(a2-a1) != sizeof(int)) {
        errs++;
        printf( "Get address of two address did not return values the correct distance apart\n" );
	fflush(stdout);
    }
    
    MPI_Finalize();
    return 0;
}

