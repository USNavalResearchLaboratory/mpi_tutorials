/* 
MPI_Test_cancelled

   Tests to see if a request was cancelled
int MPI_Test_cancelled(
  MPI_Status *status,
  int *flag
);

Parameters

   status
          [in] status object (Status)

   flag
          [out] true if the request was cancelled, false otherwise
          (logical)

Remarks

   Returns flag = true if the communication associated with the status
   object was canceled successfully. In such a case, all other fields of
   status (such as count or tag) are undefined. Returns flag = false,
   otherwise. If a receive operation might be canceled then one should
   call MPI_TEST_CANCELLED first, to check whether the operation was
   canceled, before checking on the other fields of the return status.

   Advice to users.

   Cancel can be an expensive operation that should be used only
   exceptionally.

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

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
/* Test of various send cancel calls */
int main( int argc, char *argv[] )
{
    int errs = 0;
    int rank, size, source, dest;
    MPI_Comm comm;
    MPI_Status status;
    MPI_Request req;
    static int bufsizes[4] = { 1, 100, 10000, 1000000 };
    char *buf;
    int cs, flag, n;
    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );
    source = 0;
    dest = size - 1;
    for (cs=0; cs<4; cs++) {
        if (rank == 0) {
            n = bufsizes[cs];
            buf = (char *)malloc( n );
            if (!buf) {
                fprintf( stderr, "Unable to allocate %d bytes\n", n
);fflush(stderr);
                MPI_Abort( MPI_COMM_WORLD, 1 );
            }
            MPI_Isend( buf, n, MPI_CHAR, dest, cs+n+1, comm, &req );
            MPI_Cancel( &req );
            MPI_Wait( &req, &status );
            MPI_Test_cancelled( &status, &flag );
            if (!flag) {
                errs ++;
                printf( "Failed to cancel a Isend request\n" );
                fflush(stdout);
            }
            else
            {
                n = 0;
            }
            /* Send the size, zero for successfully cancelled */
            MPI_Send( &n, 1, MPI_INT, dest, 123, comm );
            /* Send the tag so the message can be received */
            n = cs+n+1;
            MPI_Send( &n, 1, MPI_INT, dest, 123, comm );
            free( buf );
        }
        else if (rank == dest)
        {
            int n, tag;
            char *btemp;
            MPI_Recv( &n, 1, MPI_INT, 0, 123, comm, &status );
            MPI_Recv( &tag, 1, MPI_INT, 0, 123, comm, &status );
            if (n > 0)
            {
                /* If the message was not cancelled, receive it here */
                btemp = (char*)malloc( n );
                if (!btemp)
                {
                    fprintf( stderr, "Unable to allocate %d bytes\n",
n);fflush(stderr);
                    MPI_Abort( MPI_COMM_WORLD, 1 );
                }
                MPI_Recv( btemp, n, MPI_CHAR, 0, tag, comm, &status );
                free(btemp);
            }
        }
        MPI_Barrier( comm );
    }
    MPI_Finalize();
    return 0;
}

