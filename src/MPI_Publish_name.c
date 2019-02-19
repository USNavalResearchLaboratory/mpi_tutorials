/* 
MPI_Publish_name

   Publish a service name for use with MPI_Comm_connect
int MPI_Publish_name(
  char *service_name,
  MPI_Info info,
  char *port_name
);

int MPI_Publish_name(
  wchar_t *service_name,
  MPI_Info info,
  wchar_t *port_name
);

Parameters

   service_name
          [in] a service name to associate with the port (string)

   info
          [in] implementation-specific information (handle)

   port_name
          [in] a port name (string)

Remarks

   The maximum size string that may be supplied for port_name is
   MPI_MAX_PORT_NAME.

   This routine publishes the pair ( port_name, service_name) so that an
   application may retrieve a system-supplied port_name using a well-known
   service_name.

   The implementation must define the scope of a published service name,
   that is, the domain over which the service name is unique, and
   conversely, the domain over which the (port name, service name) pair
   may be retrieved. For instance, a service name may be unique to a job
   (where job is defined by a distributed operating system or batch
   scheduler), unique to a machine, or unique to a Kerberos realm. The
   scope may depend on the info argument to MPI_PUBLISH_NAME.

   MPI permits publishing more than one service_name for a single
   port_name. On the other hand, if service_name has already been
   published within the scope determined by info, the behavior of
   MPI_PUBLISH_NAME is undefined. An MPI implementation may, through a
   mechanism in the info argument to MPI_PUBLISH_NAME, provide a way to
   allow multiple servers with the same service in the same scope. In this
   case, an implementation-defined policy will determine which of several
   port names is returned by MPI_LOOKUP_NAME.

   Note that while service_name has a limited scope, determined by the
   implementation, port_name always has global scope within the
   communication universe used by the implementation (i.e., it is globally
   unique).

   port_name should be the name of a port established by MPI_OPEN_PORT and
   not yet deleted by MPI_CLOSE_PORT. If it is not, the result is
   undefined.

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

   MPI_ERR_INFO
          Invalid Info

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

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

