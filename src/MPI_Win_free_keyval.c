/* 
MPI_Win_free_keyval

   Frees an attribute key for MPI RMA windows
int MPI_Win_free_keyval(
  int *win_keyval
);

Parameters

   win_keyval
          [in] key value (integer)

Remarks

   Frees an attribute key for MPI RMA windows.

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

   MPI_ERR_WIN
          Invalid MPI window object

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

   MPI_ERR_KEYVAL
          Invalid keyval

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

/* Test creating and inserting attributes in different orders */

int checkAttrs( MPI_Win win, int n, int key[], int attrval[] );
int checkNoAttrs( MPI_Win win, int n, int key[] );

int main( int argc, char *argv[] )
{
    int errs = 0;
    int key[3], attrval[3];
    int i;
    int buf[1];
    MPI_Comm comm;
    MPI_Win  win;

    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    MPI_Win_create( buf, sizeof(int), sizeof(int), MPI_INFO_NULL, comm, &win );

    /* Create key values */
    for (i=0; i<3; i++) {
        MPI_Win_create_keyval( MPI_NULL_COPY_FN, MPI_NULL_DELETE_FN, &key[i], (void *)0 );
        attrval[i] = 1024 * i;
    }
    
    /* Insert attribute in several orders.  Test after put with get, then delete, then confirm delete with get. */
    MPI_Win_set_attr( win, key[2], &attrval[2] );
    MPI_Win_set_attr( win, key[1], &attrval[1] );
    MPI_Win_set_attr( win, key[0], &attrval[0] );
    errs += checkAttrs( win, 3, key, attrval );
    MPI_Win_delete_attr( win, key[0] );
    MPI_Win_delete_attr( win, key[1] );
    MPI_Win_delete_attr( win, key[2] );
    errs += checkNoAttrs( win, 3, key );
    MPI_Win_set_attr( win, key[1], &attrval[1] );
    MPI_Win_set_attr( win, key[2], &attrval[2] );
    MPI_Win_set_attr( win, key[0], &attrval[0] );
    errs += checkAttrs( win, 3, key, attrval );
    MPI_Win_delete_attr( win, key[2] );
    MPI_Win_delete_attr( win, key[1] );
    MPI_Win_delete_attr( win, key[0] );
    errs += checkNoAttrs( win, 3, key );
    MPI_Win_set_attr( win, key[0], &attrval[0] );
    MPI_Win_set_attr( win, key[1], &attrval[1] );
    MPI_Win_set_attr( win, key[2], &attrval[2] );
    errs += checkAttrs( win, 3, key, attrval );
    MPI_Win_delete_attr( win, key[1] );
    MPI_Win_delete_attr( win, key[2] );
    MPI_Win_delete_attr( win, key[0] );
    errs += checkNoAttrs( win, 3, key );

    for (i=0; i<3; i++) {
        MPI_Win_free_keyval( &key[i] );
    }

    MPI_Win_free( &win );
    MPI_Finalize();

    return errs;
}

int checkAttrs( MPI_Win win, int n, int key[], int attrval[] )
{
    int errs = 0;
    int i, flag, *val_p;
    for (i=0; i<n; i++) {
        MPI_Win_get_attr( win, key[i], &val_p, &flag );
        if (!flag) {
            errs++;
            fprintf( stderr, "Attribute for key %d not set\n", i );
	    fflush(stderr);
        }
        else if (val_p != &attrval[i]) {
            errs++;
            fprintf( stderr, "Atribute value for key %d not correct\n",i );
	    fflush(stderr);
        }
    }
    return errs;
}

int checkNoAttrs( MPI_Win win, int n, int key[] )
{
    int errs = 0;
    int i, flag, *val_p;
    for (i=0; i<n; i++) {
        MPI_Win_get_attr( win, key[i], &val_p, &flag );
        if (flag) {
            errs++;
            fprintf( stderr, "Attribute for key %d set but should be deleted\n", i );
	    fflush(stderr);
        }
    }
    return errs;
}

