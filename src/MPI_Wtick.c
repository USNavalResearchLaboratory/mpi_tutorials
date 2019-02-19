/* 
MPI_Wtick

   Returns the resolution of MPI_Wtime
double MPI_Wtick( void );

Return value

   Time in seconds of resolution of MPI_Wtime.

Remarks

   MPI_WTICK returns the resolution of MPI_WTIME in seconds. That is, it
   returns, as a double precision value, the number of seconds between
   successive clock ticks. For example, if the clock is implemented by the
   hardware as a counter that is incremented every millisecond, the value
   returned by MPI_WTICK should be [img196.gif]

Notes for Fortran

   This is a function, declared as DOUBLE PRECISION MPI_WTICK() in
   Fortran.

See Also

   also: MPI_Wtime, MPI_Comm_get_attr, MPI_Attr_get

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    double tick;
    MPI_Init( 0, 0 );
    tick = MPI_Wtick();
    printf("A single MPI tick is %0.9f seconds\n", tick);
    fflush(stdout);

    MPI_Finalize( );
    return 0;
}

