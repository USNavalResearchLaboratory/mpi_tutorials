/* 
MPI_Free_mem

   Free memory allocated with MPI_Alloc_mem
int MPI_Free_mem(
  void *base
);

Parameters

   base
          [in] initial address of memory segment allocated by
          MPI_ALLOC_MEM (choice)

Remarks

   In some systems, message-passing and remote-memory-access ( RMA)
   operations run faster when accessing specially allocated memory (e.g.,
   memory that is shared by the other processes in the communicating group
   on an SMP). MPI provides a mechanism for allocating and freeing such
   special memory. The use of such memory for message passing or RMA is
   not mandatory, and this memory can be used without restrictions as any
   other dynamically allocated memory. However, implementations may
   restrict the use of the MPI_WIN_LOCK and MPI_WIN_UNLOCK functions to
   windows allocated in such memory.

   The function MPI_FREE_MEM may return an error code of class
   MPI_ERR_BASE to indicate an invalid base argument.

   Rationale.

   The C and C++ bindings of MPI_ALLOC_MEM and MPI_FREE_MEM are similar to
   the bindings for the malloc and free C library calls: a call to
   MPI_Alloc_mem(..., &base) should be paired with a call to
   MPI_Free_mem(base) (one less level of indirection). Both arguments are
   declared to be of same type void* so as to facilitate type casting. The
   Fortran binding is consistent with the C and C++ bindings: the Fortran
   MPI_ALLOC_MEM call returns in baseptr the (integer valued) address of
   the allocated memory. The base argument of MPI_FREE_MEM is a choice
   argument, which passes (a reference to) the variable stored at that
   location.

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm


#include "mpi.h"
#include <stdio.h>
int main(int argc, char *argv[])
{
    int *ptr;
    MPI_Init(&argc, &argv);
    MPI_Alloc_mem(100*sizeof(int), MPI_INFO_NULL, &ptr);
    MPI_Free_mem(ptr);
    MPI_Finalize();
    return 0;
}

