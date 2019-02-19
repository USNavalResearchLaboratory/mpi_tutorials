/* 
MPI_Info_delete

   Deletes a (key,value) pair from info
int MPI_Info_delete(
  MPI_Info info,
  char *key
);

int MPI_Info_delete(
  MPI_Info info,
  wchar_t *key
);

Parameters

   info
          [in] info object (handle)

   key
          [in] key (string)

Remarks

   MPI_INFO_DELETE deletes a (key,value) pair from info. If key is not
   defined in info, the call raises an error of class MPI_ERR_INFO_NOKEY.

Thread and Interrupt Safety

   The user is responsible for ensuring that multiple threads do not try
   to update the same MPI object from different threads. This routine
   should not be used from within a signal handler.

   The MPI standard defined a thread-safe interface but this does not mean
   that all routines may be called without any thread locks. For example,
   two threads must not attempt to change the contents of the same
   MPI_Info object concurrently. The user is responsible in this case for
   using some mechanism, such as thread locks, to ensure that only one
   thread at a time makes use of this routine.

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
#include <stdlib.h>
#include <string.h>

#define NKEYS 3
int main( int argc, char *argv[] )
{
    int errs = 0;
    MPI_Info info;
    char *keys[NKEYS] = { "file", "soft", "host" };
    char *values[NKEYS] = { "runfile.txt", "2:1000:4,3:1000:7", "myhost.myorg.org" };
    char value[MPI_MAX_INFO_VAL];
    int i, flag, nkeys;

    MPI_Init( &argc, &argv );

    MPI_Info_create( &info );
    for (i=0; i<NKEYS; i++) {
        MPI_Info_set( info, keys[i], values[i] );
    }

    /* Check that all values are present */
    for (i=0; i<NKEYS; i++) {
        MPI_Info_get( info, keys[i], MPI_MAX_INFO_VAL, value, &flag );
        if (!flag) {
            errs++;
            printf( "No value for key %s\n", keys[i] );fflush(stdout);
        }
        if (strcmp( value, values[i] )) {
            errs++;
            printf( "Incorrect value for key %s, got %s expected %s\n",
		    keys[i], value, values[i] );
	    fflush(stdout);
        }
    }

    /* Now, change one value and remove another, then check again */
    MPI_Info_delete( info, keys[NKEYS-1] );
    MPI_Info_get_nkeys( info, &nkeys );
    if (nkeys != NKEYS - 1) {
        errs++;
        printf( "Deleting a key did not change the number of keys\n");
	fflush(stdout);
    }

    values[0] = "backfile.txt";
    MPI_Info_set( info, keys[0], values[0] );
    for (i=0; i<NKEYS-1; i++) {
        MPI_Info_get( info, keys[i], MPI_MAX_INFO_VAL, value, &flag );
        if (!flag) {
            errs++;
            printf( "(after reset) No value for key %s\n", keys[i]);
	    fflush(stdout);
        }
        if (strcmp( value, values[i] )) {
            errs++;
            printf( "(after reset) Incorrect value for key %s, got %s expected %s\n",
		    keys[i], value, values[i] );
	    fflush(stdout);
        }
    }

    MPI_Info_free( &info );
    if (info != MPI_INFO_NULL) {
        errs++;
        printf( "MPI_Info_free should set info to MPI_INFO_NULL\n");
	fflush(stdout);
    }

    MPI_Finalize();
    return errs;
}
