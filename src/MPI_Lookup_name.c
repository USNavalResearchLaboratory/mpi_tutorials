/* 
MPI_Lookup_name

   Lookup a port given a service name
int MPI_Lookup_name(
  char *service_name,
  MPI_Info info,
  char *port_name
);

int MPI_Lookup_name(
  wchar_t *service_name,
  MPI_Info info,
  wchar_t *port_name
);

Parameters

   service_name
          [in] a service name (string)

   info
          [in] implementation-specific information (handle)

   port_name
          [out] a port name (string)

Remarks

   This function retrieves a port_name published by MPI_PUBLISH_NAME with
   service_name. If service_name has not been published, it raises an
   error in the error class MPI_ERR_NAME. The application must supply a
   port_name buffer large enough to hold the largest possible port name.

   If an implementation allows multiple entries with the same service_name
   within the same scope, a particular port_name is chosen in a way
   determined by the implementation.

   If the info argument was used with MPI_PUBLISH_NAME to tell the
   implementation how to publish names, a similar info argument may be
   required for MPI_LOOKUP_NAME.
   If the service_name is found, MPI copies the associated value into
   port_name. The maximum size string that may be supplied by the system
   is MPI_MAX_PORT_NAME.

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

   MPI_ERR_INFO
          Invalid Info

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
#include <string.h>

int main( int argc, char *argv[] )
{
    int errs = 0;
    char port_name[MPI_MAX_PORT_NAME],
port_name_out[MPI_MAX_PORT_NAME];
    char serv_name[256];
    int merr, mclass;
    char errmsg[MPI_MAX_ERROR_STRING];
    int msglen;
    int rank;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Note that according to the MPI standard, port_name must have
been created by MPI_Open_port. For this example we'll use a fake name.
*/
    strcpy( port_name, "otherhost:122" );
    strcpy( serv_name, "MyTest" );
    MPI_Comm_set_errhandler( MPI_COMM_WORLD, MPI_ERRORS_RETURN );
    if (rank == 0)
    {
        merr = MPI_Publish_name( serv_name, MPI_INFO_NULL, port_name );
        if (merr) {
            errs++;
            MPI_Error_string( merr, errmsg, &msglen );
            printf( "Error in Publish_name: \"%s\"\n", errmsg
);fflush(stdout);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        merr = MPI_Unpublish_name( serv_name, MPI_INFO_NULL, port_name
);
        if (merr) {
            errs++;
            MPI_Error_string( merr, errmsg, &msglen );
            printf( "Error in Unpublish name: \"%s\"\n", errmsg
);fflush(stdout);
        }
    }
    else
    {
        MPI_Barrier(MPI_COMM_WORLD);
        merr = MPI_Lookup_name( serv_name, MPI_INFO_NULL, port_name_out
);
        if (merr) {
            errs++;
            MPI_Error_string( merr, errmsg, &msglen );
            printf( "Error in Lookup name: \"%s\"\n", errmsg
);fflush(stdout);
        }
        else {
            if (strcmp( port_name, port_name_out )) {
                errs++;
                printf( "Lookup name returned the wrong value (%s)\n",
port_name_out );fflush(stdout);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

