/* 
MPI_Comm_create_keyval

   Create a new attribute key
int MPI_Comm_create_keyval(
  MPI_Comm_copy_attr_function *comm_copy_attr_fn,
  MPI_Comm_delete_attr_function *comm_delete_attr_fn,
  int *comm_keyval,
  void *extra_state
);

Parameters

   comm_copy_attr_fn
          [in] Copy callback function for keyval

   comm_delete_attr_fn
          [in] Delete callback function for keyval

        comm_keyval
                [out] key value for future access (integer)

   extra_state
          [in] Extra state for callback functions

Remarks

   Key values are global (available for any and all communicators).

   Default copy and delete functions are available. These are

   MPI_COMM_NULL_COPY_FN
          empty copy function

   MPI_COMM_NULL_DELETE_FN
          empty delete function

   MPI_COMM_DUP_FN
          simple dup function

   There are subtle differences between C and Fortran that require that
   the copy_fn be written in the same language that MPI_Comm_create_keyval
   is called from. This should not be a problem for most users; only
   programers using both Fortran and C in the same program need to be sure
   that they follow this rule.

   This function replaces MPI_KEYVAL_CREATE, whose use is deprecated. The
   C binding is identical. The Fortran binding differs in that extra_state
   is an address-sized integer. Also, the copy and delete callback
   functions have Fortran bindings that are consistent with address-sized
   attributes.

   The argument comm_copy_attr_fn may be specified as
   MPI_COMM_NULL_COPY_FN or MPI_COMM_DUP_FN from either C, C++, or
   Fortran. MPI_COMM_NULL_COPY_FN is a function that does nothing other
   than returning flag = 0 and MPI_SUCCESS. MPI_COMM_DUP_FN is a
   simple-minded copy function that sets flag = 1, returns the value of
   attribute_val_in in attribute_val_out, and returns MPI_SUCCESS. These
   replace the MPI-1 predefined callbacks MPI_NULL_COPY_FN and MPI_DUP_FN,
   whose use is deprecated.

   The argument comm_delete_attr_fn may be specified as
   MPI_COMM_NULL_DELETE_FN from either C, C++, or Fortran.
   MPI_COMM_NULL_DELETE_FN is a function that does nothing, other than
   returning MPI_SUCCESS. MPI_COMM_NULL_DELETE_FN replaces
   MPI_NULL_DELETE_FN, whose use is deprecated.

   The C callback functions are:

   typedef int MPI_Comm_copy_attr_function(MPI_Comm oldcomm, int
   comm_keyval, void *extra_state, void *attribute_val_in, void
   *attribute_val_out, int *flag);

   and

   typedef int MPI_Comm_delete_attr_function(MPI_Comm comm, int
   comm_keyval, void *attribute_val, void *extra_state);

   which are the same as the MPI-1.1 calls but with a new name. The old
   names are deprecated.

   The Fortran callback functions are:

   SUBROUTINE COMM_COPY_ATTR_FN(OLDCOMM, COMM_KEYVAL, EXTRA_STATE,
   ATTRIBUTE_VAL_IN, ATTRIBUTE_VAL_OUT, FLAG, IERROR)
   INTEGER OLDCOMM, COMM_KEYVAL, IERROR
   INTEGER(KIND=MPI_ADDRESS_KIND) EXTRA_STATE, ATTRIBUTE_VAL_IN,
   ATTRIBUTE_VAL_OUT
   LOGICAL FLAG

   and

   SUBROUTINE COMM_DELETE_ATTR_FN(COMM, COMM_KEYVAL, ATTRIBUTE_VAL,
   EXTRA_STATE, IERROR)
   INTEGER COMM, COMM_KEYVAL, IERROR
   INTEGER(KIND=MPI_ADDRESS_KIND) ATTRIBUTE_VAL, EXTRA_STATE

   The C++ callbacks are:

   typedef int MPI::Comm::Copy_attr_function(const MPI::Comm& oldcomm, int
   comm_keyval, void* extra_state, void* attribute_val_in, void*
   attribute_val_out, bool& flag);

   and

   typedef int MPI::Comm::Delete_attr_function(MPI::Comm& comm, int
   comm_keyval, void* attribute_val, void* extra_state);

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

See Also

   MPI_Comm_free_keyval

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
            fprintf( stderr, "Atribute value for key %d not correct\n",i );
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

