/* 
MPI_Wtime

   Returns an elapsed time on the calling processor
double MPI_Wtime( void );

Return value

   Time in seconds since an arbitrary time in the past.

Remarks

   MPI_WTIME returns a floating-point number of seconds, representing
   elapsed wall-clock time since some time in the past.

   The "time in the past" is guaranteed not to change during the life of
   the process. The user is responsible for converting large numbers of
   seconds to other units if they are preferred.

   The times returned are local to the node that called them. There is no
   requirement that different nodes return "the same time."
   This is intended to be a high-resolution, elapsed (or wall) clock. See
   MPI_WTICK to determine the resolution of MPI_WTIME. If the attribute
   MPI_WTIME_IS_GLOBAL is defined and true, then the value is synchronized
   across all processes in MPI_COMM_WORLD.

Notes for Fortran

   This is a function, declared as DOUBLE PRECISION MPI_WTIME() in
   Fortran.

See Also

   also: MPI_Wtick, MPI_Comm_get_attr, MPI_Attr_get

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
//#include <windows.h>
#include <unistd.h>
#include <stdio.h>

int main( int argc, char *argv[] )
{
    double t1, t2;
    
    MPI_Init( 0, 0 );
    t1 = MPI_Wtime();
    //Sleep(1000);     // Windows
    sleep(1);          // *nix
    t2 = MPI_Wtime();

    printf("MPI_Wtime measured a 1 second sleep to be: %1.2f\n", t2-t1);
    fflush(stdout);

    MPI_Finalize( );
    return 0;
}

