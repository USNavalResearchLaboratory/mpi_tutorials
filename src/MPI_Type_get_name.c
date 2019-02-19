/* 
MPI_Type_get_name

   Get the print name for a datatype
int MPI_Type_get_name(
  MPI_Datatype datatype,
  char *type_name,
  int *resultlen
);

int MPI_Type_get_name(
  MPI_Datatype datatype,
  wchar_t *type_name,
  int *resultlen
);

Parameters

   type
          [in] datatype whose name is to be returned (handle)

   type_name
          [out] the name previously stored on the datatype, or a empty
          string if no such name exists (string)

   resultlen
          [out] length of returned name (integer)

Remarks

   MPI_COMM_GET_NAME returns the last name which has previously been
   associated with the given communicator. The name may be set and got
   from any language. The same name will be returned independent of the
   language used. name should be allocated so that it can hold a resulting
   string of length MPI_MAX_OBJECT_NAME characters. MPI_COMM_GET_NAME
   returns a copy of the set name in name.

   If the user has not associated a name with a communicator, or an error
   occurs, MPI_COMM_GET_NAME will return an empty string (all spaces in
   Fortran, "" in C and C++). The three predefined communicators will have
   predefined names associated with them. Thus, the names of
   MPI_COMM_WORLD, MPI_COMM_SELF, and MPI_COMM_PARENT will have the
   default of MPI_COMM_WORLD, MPI_COMM_SELF, and MPI_COMM_PARENT. The fact
   that the system may have chosen to give a default name to a
   communicator does not prevent the user from setting a name on the same
   communicator; doing this removes the old name and assigns the new one.

   Rationale.

   We provide separate functions for setting and getting the name of a
   communicator, rather than simply providing a predefined attribute key
   for the following reasons:
     * It is not, in general, possible to store a string as an attribute
       from Fortran.

     * It is not easy to set up the delete function for a string attribute
       unless it is known to have been allocated from the heap.

     * To make the attribute key useful additional code to call strdup is
       necessary. If this is not standardized then users have to write it.
       This is extra unneeded work which we can easily eliminate.

     * The Fortran binding is not trivial to write (it will depend on
       details of the Fortran compilation system), and will not be
       portable. Therefore it should be in the library rather than in user
       code.

   ( End of rationale.)
   Advice to users.

   The above definition means that it is safe simply to print the string
   returned by MPI_COMM_GET_NAME, as it is always a valid string even if
   there was no name.

   Note that associating a name with a communicator has no effect on the
   semantics of an MPI program, and will (necessarily) increase the store
   requirement of the program, since the names must be saved. Therefore
   there is no requirement that users use these functions to associate
   names with communicators. However debugging and profiling MPI
   applications may be made easier if names are associated with
   communicators, since the debugger or profiler should then be able to
   present information in a less cryptic manner.

Thread and Interrupt Safety

   This routine is thread and interrupt safe only if no MPI routine that
   updates or frees the same MPI object may be called concurrently with
   this routine.

   The MPI standard defined a thread-safe interface but this does not mean
   that all routines may be called without any thread locks. For example,
   two threads must not attempt to change the contents of the same
   MPI_Info object concurrently. The user is responsible in this case for
   using some mechanism, such as thread locks, to ensure that only one
   thread at a time makes use of this routine.

   Null Handles The MPI 1.1 specification, in the section on opaque
   objects, explicitly disallows freeing a null communicator. The text
   from the standard is:
 A null handle argument is an erroneous IN argument in MPI calls, unless an
 exception is explicitly stated in the text that defines the function. Such
 exception is allowed for handles to request objects in Wait and Test calls
 (sections Communication Completion and Multiple Completions ). Otherwise, a
 null handle can only be passed to a function that allocates a new object and
 returns a reference to it in the handle.

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

   MPI_ERR_TYPE
          Invalid datatype argument. May be an uncommitted MPI_Datatype
          (see MPI_Type_commit).

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <string.h>

/* Create an array with all of the MPI names in it */
typedef struct mpi_names_t { MPI_Datatype dtype; const char *name; }
mpi_names_t;

/* The MPI standard specifies that the names must be the MPI names, not
   the related language names (e.g., MPI_CHAR, not char) */
static mpi_names_t mpi_names[] = {
    { MPI_CHAR, "MPI_CHAR" },
    { MPI_SIGNED_CHAR, "MPI_SIGNED_CHAR" },
    { MPI_UNSIGNED_CHAR, "MPI_UNSIGNED_CHAR" },
    { MPI_BYTE, "MPI_BYTE" },
    { MPI_WCHAR, "MPI_WCHAR" },
    { MPI_SHORT, "MPI_SHORT" },
    { MPI_UNSIGNED_SHORT, "MPI_UNSIGNED_SHORT" },
    { MPI_INT, "MPI_INT" },
    { MPI_UNSIGNED, "MPI_UNSIGNED" },
    { MPI_LONG, "MPI_LONG" },
    { MPI_UNSIGNED_LONG, "MPI_UNSIGNED_LONG" },
    { MPI_FLOAT, "MPI_FLOAT" },
    { MPI_DOUBLE, "MPI_DOUBLE" },
    { MPI_LONG_DOUBLE, "MPI_LONG_DOUBLE" },
    { MPI_LONG_LONG, "MPI_LONG_LONG" },
    { MPI_UNSIGNED_LONG_LONG, "MPI_UNSIGNED_LONG_LONG" },
    { MPI_PACKED, "MPI_PACKED" },
    { MPI_LB, "MPI_LB" },
    { MPI_UB, "MPI_UB" },
    { MPI_FLOAT_INT, "MPI_FLOAT_INT" },
    { MPI_DOUBLE_INT, "MPI_DOUBLE_INT" },
    { MPI_LONG_INT, "MPI_LONG_INT" },
    { MPI_SHORT_INT, "MPI_SHORT_INT" },
    { MPI_2INT, "MPI_2INT" },
    { MPI_LONG_DOUBLE_INT, "MPI_LONG_DOUBLE_INT" },
    { MPI_COMPLEX, "MPI_COMPLEX" },
    { MPI_DOUBLE_COMPLEX, "MPI_DOUBLE_COMPLEX" },
    { MPI_LOGICAL, "MPI_LOGICAL" },
    { MPI_REAL, "MPI_REAL" },
    { MPI_DOUBLE_PRECISION, "MPI_DOUBLE_PRECISION" },
    { MPI_INTEGER, "MPI_INTEGER" },
    { MPI_2INTEGER, "MPI_2INTEGER" },
    { MPI_2COMPLEX, "MPI_2COMPLEX" },
    { MPI_2DOUBLE_COMPLEX, "MPI_2DOUBLE_COMPLEX" },
    { MPI_2REAL, "MPI_2REAL" },
    { MPI_2DOUBLE_PRECISION, "MPI_2DOUBLE_PRECISION" },
    { MPI_CHARACTER, "MPI_CHARACTER" },
    { MPI_REAL4, "MPI_REAL4" },
    { MPI_REAL8, "MPI_REAL8" },
    { MPI_REAL16, "MPI_REAL16" },
    { MPI_COMPLEX8, "MPI_COMPLEX8" },
    { MPI_COMPLEX16, "MPI_COMPLEX16" },
    { MPI_COMPLEX32, "MPI_COMPLEX32" },
    { MPI_INTEGER1, "MPI_INTEGER1" },
    { MPI_INTEGER2, "MPI_INTEGER2" },
    { MPI_INTEGER4, "MPI_INTEGER4" },
    { MPI_INTEGER8, "MPI_INTEGER8" },
    //{ MPI_INTEGER16, "MPI_INTEGER16" }, // won't compile with this one
    { 0, (char *)0 }, /* Sentinal used to indicate the last element */
};

int main( int argc, char **argv )
{
    char name[MPI_MAX_OBJECT_NAME];
    int namelen, i;
    int errs = 0;

    MPI_Init(0,0);
    /* Sample some datatypes */
    /* See 8.4, "Naming Objects" in MPI-2. The default name is the same
       as the datatype name */
    MPI_Type_get_name( MPI_DOUBLE, name, &namelen );
    if (strncmp( name, "MPI_DOUBLE", MPI_MAX_OBJECT_NAME )) {
        errs++;
        fprintf( stderr, "Expected MPI_DOUBLE but got :%s:\n", name);
	fflush(stderr);
    }
    MPI_Type_get_name( MPI_INT, name, &namelen );
    if (strncmp( name, "MPI_INT", MPI_MAX_OBJECT_NAME )) {
        errs++;
        fprintf( stderr, "Expected MPI_INT but got :%s:\n", name);
	fflush(stderr);
    }
    /* Now we try them ALL */
    for (i=0; mpi_names[i].name != 0; i++) {
        /* The size-specific types may be MPI_DATATYPE_NULL */
        if (mpi_names[i].dtype == MPI_DATATYPE_NULL) continue;
        name[0] = 0;
        MPI_Type_get_name( mpi_names[i].dtype, name, &namelen );
        if (strncmp( name, mpi_names[i].name, namelen )) {
            errs++;
            fprintf( stderr, "Expected %s but got %s\n", mpi_names[i].name, name );
	    fflush(stderr);
        }
    }
    
    /* Try resetting the name */
    MPI_Type_set_name( MPI_INT, "int" );
    name[0] = 0;
    MPI_Type_get_name( MPI_INT, name, &namelen );
    if (strncmp( name, "int", MPI_MAX_OBJECT_NAME )) {
        errs++;
        fprintf( stderr, "Expected int but got :%s:\n", name);
	fflush(stderr);
    }

    MPI_Finalize();
    return errs;
}

