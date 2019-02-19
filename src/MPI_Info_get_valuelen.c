/* 
MPI_Info_get_valuelen

   Retrieves the length of the value associated with a key
int MPI_Info_get_valuelen(
  MPI_Info info,
  char *key,
  int *valuelen,
  int *flag
);

int MPI_Info_get_valuelen(
  MPI_Info info,
  wchar_t *key,
  int *valuelen,
  int *flag
);

Parameters

   info
          [in] info object (handle)

   key
          [in] key (string)

   valuelen
          [out] length of value argument (integer)

   flag
          [out] true if key defined, false if not (boolean)

Remarks

   Retrieves the length of the value associated with key. If key is
   defined, valuelen is set to the length of its associated value and flag
   is set to true. If key is not defined, valuelen is not touched and flag
   is set to false. The length returned in C or C++ does not include the
   end-of-string character.

   If key is larger than MPI_MAX_INFO_KEY, the call is erroneous.

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

   MPI_ERR_INFO_KEY
          Invalid or null key string for info.

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <string.h>

#define NKEYS 3
int main( int argc, char *argv[] )
{
    int errs = 0;
    MPI_Info info;
    char *keys[NKEYS] = { "file", "soft", "host" };
    char *values[NKEYS] = { "runfile.txt", "2:1000:4,3:1000:7",
"myhost.myorg.org" };
    char value[MPI_MAX_INFO_VAL];
    int i, flag, vallen;

    MPI_Init( &argc, &argv );

    MPI_Info_create( &info );
    for (i=0; i<NKEYS; i++) {
        MPI_Info_set( info, keys[i], values[i] );
    }

    /* Check that all values are present */
    for (i=0; i<NKEYS; i++) {
        MPI_Info_get_valuelen( info, keys[i], &vallen, &flag );
        if (!flag) {
            errs++;
            printf( "get_valuelen failed for valid key %s\n", keys[i]
);fflush(stdout);
        }
        MPI_Info_get( info, keys[i], MPI_MAX_INFO_VAL, value, &flag );
        if (!flag) {
            errs++;
            printf( "No value for key %s\n", keys[i] );fflush(stdout);
        }
        if (strcmp( value, values[i] )) {
            errs++;
            printf( "Incorrect value for key %s\n", keys[i]
);fflush(stdout);
        }
        if (strlen(value) != vallen) {
            errs++;
            printf( "value_len returned %d but actual len is %d\n",
vallen, (int) strlen(value) );fflush(stdout);
        }
    }

    MPI_Info_free( &info );
    MPI_Finalize();
    return errs;
}

