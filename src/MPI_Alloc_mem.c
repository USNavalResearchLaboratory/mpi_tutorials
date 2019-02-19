/* 
MPI_Alloc_mem

   Allocate memory for message passing and RMA (Remote Memory Access)
int MPI_Alloc_mem(
  MPI_Aint size,
  MPI_Info info,
  void *baseptr
);

Parameters

   size
          [in] size of memory segment in bytes (nonnegative integer)

   info
          [in] info argument (handle)

   baseptr
          [out] pointer to a pointer to the beginning of the memory
          segment allocated

Remarks

   Using this routine from Fortran requires that the Fortran compiler
   accept a common pointer extension. See Section 4.11 (Memory Allocation)
   in the MPI-2 standard for more information and examples.

   Also note that while baseptr is a void * type, this is simply to allow
   easy use of any pointer object for this parameter. In fact, this
   argument is really a void ** type, that is, a pointer to a pointer.

   In some systems, message-passing and remote-memory-access ( RMA)
   operations run faster when accessing specially allocated memory (e.g.,
   memory that is shared by the other processes in the communicating group
   on an SMP). MPI provides a mechanism for allocating and freeing such
   special memory. The use of such memory for message passing or RMA is
   not mandatory, and this memory can be used without restrictions as any
   other dynamically allocated memory. However, implementations may
   restrict the use of the MPI_WIN_LOCK and MPI_WIN_UNLOCK functions to
   windows allocated in such memory

   The info argument can be used to provide directives that control the
   desired location of the allocated memory. Such a directive does not
   affect the semantics of the call. Valid info values are
   implementation-dependent; a null directive value of info =
   MPI_INFO_NULL is always valid.

   The function MPI_ALLOC_MEM may return an error code of class
   MPI_ERR_NO_MEM to indicate it failed because memory is exhausted.

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

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

   MPI_ERR_NO_MEM
          Insufficient memory available for allocation by MPI_Alloc_mem

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
int main( int argc, char *argv[] )
{
    int errs = 0, err;
    int j, count;
    char *ap;
    MPI_Init( &argc, &argv );
    MPI_Errhandler_set( MPI_COMM_WORLD, MPI_ERRORS_RETURN );
    for (count=1; count < 128000; count *= 2)
    {
        err = MPI_Alloc_mem( count, MPI_INFO_NULL, &ap );
        if (err) {
            int errclass;
            /* An error of MPI_ERR_NO_MEM is allowed */
            MPI_Error_class( err, &errclass );
            if (errclass != MPI_ERR_NO_MEM) {
                errs++;
                printf("MPI_Alloc_mem failed.\n");
            }
        }
        else {
            /* Access all of this memory */
            for (j=0; j<count; j++) {
                ap[j] = (char)(j & 0x7f);
            }
            MPI_Free_mem( ap );
        }
    }
    MPI_Finalize();
    return 0;
}

