/* 
MPI_Comm_get_attr

   Retrieves attribute value by key
int MPI_Comm_get_attr(
  MPI_Comm comm,
  int comm_keyval,
  void *attribute_val,
  int *flag
);

Parameters

   comm
          [in] communicator to which attribute is attached (handle)

   comm_keyval
          [in] key value (integer)

   attribute_val
          [out] attribute value, unless flag = false

   flag
          [out] true if an attribute value was extracted; false if no
          attribute is associated with the key

Remarks

   Retrieves attribute value by key. The call is erroneous if there is no
   key with value keyval. On the other hand, the call is correct if the
   key value exists, but no attribute is attached on comm for that key; in
   such case, the call returns flag = false. In particular
   MPI_KEYVAL_INVALID is an erroneous key value.

   Advice to users.

   The call to MPI_Attr_put passes in attribute_val the value of the
   attribute; the call to MPI_Attr_get passes in attribute_val the address
   of the the location where the attribute value is to be returned. Thus,
   if the attribute value itself is a pointer of type void*, the the
   actual attribute_val parameter to MPI_Attr_put will be of type void*
   and the actual attribute_val parameter to MPI_Attr_put will be of type
   void**. ( End of advice to users.)
   Rationale.

   The use of a formal parameter attribute_val or type void* (rather than
   void**) avoids the messy type casting that would be needed if the
   attribute value is declared with a type other than void*.
   This function replaces MPI_ATTR_GET, whose use is deprecated. The C
   binding is identical. The Fortran binding differs in that attribute_val
   is an address-sized integer.

   Attributes must be extracted from the same language as they were
   inserted in with MPI_Comm_set_attr. The notes for C and Fortran below
   explain why.

Notes for C

   Even though the attr_value arguement is declared as void *, it is
   really the address of a void pointer. See the rationale in the standard
   for more details.

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

int main( int argc, char **argv)
{
    void *v;
    int flag;
    int vval;
    int rank, size;
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_TAG_UB, &v, &flag );
    if (!flag) {
        fprintf( stderr, "Could not get TAG_UB\n" );
	fflush(stderr);
    }
    else {
        vval = *(int*)v;
        if (vval < 32767) {
            fprintf( stderr, "Got too-small value (%d) for TAG_UB\n", vval );
	    fflush(stderr);
        }
    }
    
    MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_HOST, &v, &flag );
    if (!flag) {
        fprintf( stderr, "Could not get HOST\n" );
	fflush(stderr);
    }
    else {
        vval = *(int*)v;
        if ((vval < 0 || vval >= size) && vval != MPI_PROC_NULL) {
            fprintf( stderr, "Got invalid value %d for HOST\n", vval);
	    fflush(stderr);
        }
    }
    MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_IO, &v, &flag );
    if (!flag) {
        fprintf( stderr, "Could not get IO\n" );
	fflush(stderr);
    }
    else {
        vval = *(int*)v;
        if ((vval < 0 || vval >= size) && vval != MPI_ANY_SOURCE && vval != MPI_PROC_NULL) {
            fprintf( stderr, "Got invalid value %d for IO\n", vval);
	    fflush(stderr);
        }
    }
    MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_WTIME_IS_GLOBAL, &v, &flag);
    if (flag) {
        /* Wtime need not be set */
        vval = *(int*)v;
        if (vval < 0 || vval > 1) {
            fprintf( stderr, "Invalid value for WTIME_IS_GLOBAL (got %d)\n", vval );
	    fflush(stderr);
        }
    }
    MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_APPNUM, &v, &flag );
    /* appnum need not be set */
    if (flag) {
        vval = *(int *)v;
        if (vval < 0) {
            fprintf( stderr, "MPI_APPNUM is defined as %d but must be nonnegative\n", vval );
	    fflush(stderr);
        }
    }
    MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_UNIVERSE_SIZE, &v, &flag );
    /* MPI_UNIVERSE_SIZE need not be set */
    if (flag) {
        /* But if it is set, it must be at least the size of comm_world */
        vval = *(int *)v;
        if (vval < size) {
            fprintf( stderr, "MPI_UNIVERSE_SIZE = %d, less than comm world (%d)\n", vval, size );
	    fflush(stderr);
        }
    }
    MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_LASTUSEDCODE, &v, &flag );
    /* Last used code must be defined and >= MPI_ERR_LASTCODE */
    if (flag) {
        vval = *(int*)v;
        if (vval < MPI_ERR_LASTCODE) {
            fprintf( stderr, "MPI_LASTUSEDCODE points to an integer (%d) smaller than MPI_ERR_LASTCODE (%d)\n",
		     vval, MPI_ERR_LASTCODE);
	    fflush(stderr);
        }
    }
    else {
        fprintf( stderr, "MPI_LASTUSECODE is not defined\n");
	fflush(stderr);
    }
    
    MPI_Finalize();
    return 0;
}

