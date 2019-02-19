/* 
MPI_Comm_set_attr

   Stores attribute value associated with a key
int MPI_Comm_set_attr(
  MPI_Comm comm,
  int comm_keyval,
  void *attribute_val
);

Parameters

   comm
          [in] communicator to which attribute will be attached (handle)

   keyval
          [in] key value, as returned by MPI_Comm_create_keyval (integer)

   attribute_val
          [in] attribute value

Remarks

   This function stores the stipulated attribute value attribute_val for
   subsequent retrieval by MPI_ATTR_GET. If the value is already present,
   then the outcome is as if MPI_ATTR_DELETEwas first called to delete the
   previous value (and the callback function delete_fn was executed), and
   a new value was next stored. The call is erroneous if there is no key
   with value keyval; in particular MPI_KEYVAL_INVALID is an erroneous key
   value. The call will fail if the delete_fn function returned an error
   code other than MPI_SUCCESS.

   Values of the permanent attributes MPI_TAG_UB, MPI_HOST, MPI_IO,
   MPI_WTIME_IS_GLOBAL, MPI_UNIVERSE_SIZE, MPI_LASTUSEDCODE, and
   MPI_APPNUM may not be changed.

   The type of the attribute value depends on whether C, C++, or Fortran
   is being used. In C and C++, an attribute value is a pointer (void *);
   in Fortran, it is an address-sized integer.

   If an attribute is already present, the delete function (specified when
   the corresponding keyval was created) will be called.

   This function replaces MPI_ATTR_PUT, whose use is deprecated. The C
   binding is identical. The Fortran binding differs in that attribute_val
   is an address-sized integer.

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

   MPI_ERR_COMM
          Invalid communicator. A common error is to use a null
          communicator in a call (not even allowed in MPI_Comm_rank).

   MPI_ERR_KEYVAL
          Invalid keyval

   MPI_ERR_ARG
          This error class is associated with an error code that indicates
          that an attempt was made to free one of the permanent keys.

See Also

   MPI_Comm_create_keyval, MPI_Comm_delete_attr

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

/* Test creating and inserting attributes in different orders to ensure
   that the list management code handles all cases. */
int checkAttrs( MPI_Comm comm, int n, int key[], int attrval[] );
int checkNoAttrs( MPI_Comm comm, int n, int key[] );

int main( int argc, char *argv[] )
{
    int errs = 0;
    int key[3], attrval[3];
    int i;
    MPI_Comm comm;
    MPI_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;
    /* Create key values */
    for (i=0; i<3; i++) {
        MPI_Comm_create_keyval( MPI_NULL_COPY_FN, MPI_NULL_DELETE_FN, &key[i], (void *)0 );
        attrval[i] = 1024 * i;
    }
    /* Insert attribute in several orders. Test after put with get,
       then delete, then confirm delete with get. */
    MPI_Comm_set_attr( comm, key[2], &attrval[2] );
    MPI_Comm_set_attr( comm, key[1], &attrval[1] );
    MPI_Comm_set_attr( comm, key[0], &attrval[0] );
    errs += checkAttrs( comm, 3, key, attrval );
    MPI_Comm_delete_attr( comm, key[0] );
    MPI_Comm_delete_attr( comm, key[1] );
    MPI_Comm_delete_attr( comm, key[2] );
    errs += checkNoAttrs( comm, 3, key );
    MPI_Comm_set_attr( comm, key[1], &attrval[1] );
    MPI_Comm_set_attr( comm, key[2], &attrval[2] );
    MPI_Comm_set_attr( comm, key[0], &attrval[0] );
    errs += checkAttrs( comm, 3, key, attrval );
    MPI_Comm_delete_attr( comm, key[2] );
    MPI_Comm_delete_attr( comm, key[1] );
    MPI_Comm_delete_attr( comm, key[0] );
    errs += checkNoAttrs( comm, 3, key );
    MPI_Comm_set_attr( comm, key[0], &attrval[0] );
    MPI_Comm_set_attr( comm, key[1], &attrval[1] );
    MPI_Comm_set_attr( comm, key[2], &attrval[2] );
    errs += checkAttrs( comm, 3, key, attrval );
    MPI_Comm_delete_attr( comm, key[1] );
    MPI_Comm_delete_attr( comm, key[2] );
    MPI_Comm_delete_attr( comm, key[0] );
    errs += checkNoAttrs( comm, 3, key );
    for (i=0; i<3; i++) {
        MPI_Comm_free_keyval( &key[i] );
    }
    
    MPI_Finalize();
    return 0;
}

int checkAttrs( MPI_Comm comm, int n, int key[], int attrval[] )
{
    int errs = 0;
    int i, flag, *val_p;
    for (i=0; i<n; i++) {
        MPI_Comm_get_attr( comm, key[i], &val_p, &flag );
        if (!flag) {
            errs++;
            fprintf( stderr, "Attribute for key %d not set\n", i);
	    fflush(stderr);
        }
        else if (val_p != &attrval[i]) {
            errs++;
            fprintf( stderr, "Atribute value for key %d not correct\n", i);
	    fflush(stderr);
        }
    }
    return errs;
}

int checkNoAttrs( MPI_Comm comm, int n, int key[] )
{
    int errs = 0;
    int i, flag, *val_p;
    for (i=0; i<n; i++) {
        MPI_Comm_get_attr( comm, key[i], &val_p, &flag );
        if (flag) {
            errs++;
            fprintf( stderr, "Attribute for key %d set but should be deleted\n", i );
	    fflush(stderr);
        }
    }
    return errs;
}
