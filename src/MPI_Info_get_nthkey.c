/* 
MPI_Info_get_nthkey

   Returns the nth defined key in info
int MPI_Info_get_nthkey(
  MPI_Info info,
  int n,
  char *key
);

int MPI_Info_get_nthkey(
  MPI_Info info,
  int n,
  wchar_t *key
);

Parameters

   info
          [in] info object (handle)

   n
          [in] key number (integer)

   key
          [out] key (string). The maximum number of characters is
          MPI_MAX_INFO_KEY.

Remarks

   This function returns the nth defined key in info. Keys are numbered 0
   ... N-1 where N is the value returned by MPI_INFO_GET_NKEYS. All keys
   between 0 and N-1 are guaranteed to be defined. The number of a given
   key does not change as long as info is not modified with MPI_INFO_SET
   or MPI_INFO_DELETE.

Thread and Interrupt Safety

   This routine is thread and interrupt safe only if no MPI routine that
   updates or frees the same MPI_Info object may be called concurrently
   with this routine.

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

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[] )
{
    int errs = 0;
    MPI_Info info1, infodup;
    int nkeys, nkeysdup, i, vallen, flag, flagdup;
    char key[MPI_MAX_INFO_KEY], keydup[MPI_MAX_INFO_KEY];
    char value[MPI_MAX_INFO_VAL], valdup[MPI_MAX_INFO_VAL];

    MPI_Init( &argc, &argv );

    MPI_Info_create( &info1 );
    MPI_Info_set( info1, "host", "myhost.myorg.org" );
    MPI_Info_set( info1, "file", "runfile.txt" );
    MPI_Info_set( info1, "soft", "2:1000:4,3:1000:7" );

    MPI_Info_dup( info1, &infodup );

    MPI_Info_get_nkeys( infodup, &nkeysdup );
    MPI_Info_get_nkeys( info1, &nkeys );
    if (nkeys != nkeysdup) {
        errs++;
        printf( "Dup'ed info has a different number of keys; is %d should be %d\n",
		nkeysdup, nkeys );
	fflush(stdout);
    }
    vallen = MPI_MAX_INFO_VAL;
    for (i=0; i<nkeys; i++) {
        /* MPI requires that the keys are in the same order after the dup */
        MPI_Info_get_nthkey( info1, i, key );
        MPI_Info_get_nthkey( infodup, i, keydup );
        if (strcmp(key, keydup)) {
            errs++;
            printf( "keys do not match: %s should be %s\n", keydup, key);
	    fflush(stdout);
        }

        vallen = MPI_MAX_INFO_VAL;
        MPI_Info_get( info1, key, vallen, value, &flag );
        MPI_Info_get( infodup, keydup, vallen, valdup, &flagdup );
        if (!flag || !flagdup) {
            errs++;
            printf( "Info get failed for key %s\n", key);
	    fflush(stdout);
        }
        else if (strcmp( value, valdup )) {
            errs++;
            printf( "Info values for key %s not the same after dup\n", key);
	    fflush(stdout);
        }
    }

    /* Change info and check that infodup does NOT have the new value
       (ensure that lazy dups are still duped) */
    MPI_Info_set( info1, "path", "/a:/b:/c/d" );

    MPI_Info_get( infodup, "path", vallen, value, &flag );
    if (flag) {
        errs++;
        printf( "inserting path into info changed infodup\n");
	fflush(stdout);
    }
    
    MPI_Info_free( &info1 );
    MPI_Info_free( &infodup );

    MPI_Finalize();
    return errs;
}
