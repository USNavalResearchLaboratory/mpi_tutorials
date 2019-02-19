/* 
MPI_Init_thread

   Initialize the MPI execution environment
int MPI_Init_thread(
  int *argc,
  char ***argv,
  int required,
  int *provided
);

int MPI_Init_thread(
  int *argc,
  wchar_t ***argv,
  int required,
  int *provided
);

Parameters

   argc
          [in] Pointer to the number of arguments

   argv
          [in] Pointer to the argument vector

   required
          [in] Level of desired thread support

   provided
          [out] Level of provided thread support

Command line arguments

   MPI specifies no command-line arguments but does allow an MPI
   implementation to make use of them. See MPI_INIT for a description of
   the command line arguments supported by MPI_INIT and MPI_INIT_THREAD.

Remarks

   Advice to users.

   In C and C++, the passing of argc and argv is optional. In C, this is
   accomplished by passing the appropriate null pointer. In C++, this is
   accomplished with two separate bindings to cover these two cases. ( End
   of advice to users.)

   This call initializes MPI in the same way that a call to MPI_INIT
   would. In addition, it initializes the thread environment. The argument
   required is used to specify the desired level of thread support. The
   possible values are listed in increasing order of thread support:

   MPI_THREAD_SINGLE
          Only one thread will execute.

   MPI_THREAD_FUNNELED
          The process may be multi-threaded, but only the main thread will
          make MPI calls (all MPI calls are funneled to the main thread).

   MPI_THREAD_SERIALIZED
          The process may be multi-threaded, and multiple threads may make
          MPI calls, but only one at a time: MPI calls are not made
          concurrently from two distinct threads (all MPI calls are
          serialized).

   MPI_THREAD_MULTIPLE
          Multiple threads may call MPI, with no restrictions.These values
          are monotonic; i.e., MPI_THREAD_SINGLE < MPI_THREAD_FUNNELED <
          MPI_THREAD_SERIALIZED < MPI_THREAD_MULTIPLE.

          Different processes in MPI_COMM_WORLD may require different
          levels of thread support.

          The call returns in provided information about the actual level
          of thread support that will be provided by MPI. It can be one of
          the four values listed above.

          The level(s) of thread support that can be provided by
          MPI_INIT_THREAD will depend on the implementation, and may
          depend on information provided by the user before the program
          started to execute (e.g., with arguments to mpiexec). If
          possible, the call will return provided = required. Failing
          this, the call will return the least supported level such that
          provided > required (thus providing a stronger level of support
          than required by the user). Finally, if the user requirement
          cannot be satisfied, then the call will return in provided the
          highest supported level.

          A thread compliant MPI implementation will be able to return
          provided
          = MPI_THREAD_MULTIPLE. Such an implementation may always return
          provided
          = MPI_THREAD_MULTIPLE, irrespective of the value of required. At
          the other extreme, an MPI library that is not thread compliant
          may always return provided = MPI_THREAD_SINGLE, irrespective of
          the value of required.

          A call to MPI_INIT has the same effect as a call to
          MPI_INIT_THREAD with a required = MPI_THREAD_SINGLE.

          Vendors may provide (implementation dependent) means to specify
          the level(s) of thread support available when the MPI program is
          started, e.g., with arguments to mpiexec. This will affect the
          outcome of calls to MPI_INIT and MPI_INIT_THREAD. Suppose, for
          example, that an MPI program has been started so that only
          MPI_THREAD_MULTIPLE is available. Then MPI_INIT_THREAD will
          return provided = MPI_THREAD_MULTIPLE, irrespective of the value
          of required; a call to MPI_INIT will also initialize the MPI
          thread support level to MPI_THREAD_MULTIPLE. Suppose, on the
          other hand, that an MPI program has been started so that all
          four levels of thread support are available. Then, a call to
          MPI_INIT_THREAD will return provided = required; on the other
          hand, a call to MPI_INIT will initialize the MPI thread support
          level to MPI_THREAD_SINGLE.

          Rationale.

          Various optimizations are possible when MPI code is executed
          single-threaded, or is executed on multiple threads, but not
          concurrently: mutual exclusion code may be omitted. Furthermore,
          if only one thread executes, then the MPI library can use
          library functions that are not thread safe, without risking
          conflicts with user threads. Also, the model of one
          communication thread, multiple computation threads fits well
          many applications. E.g., if the process code is a sequential
          Fortran/C/C++ program with MPI calls that has been parallelized
          by a compiler for execution on an SMP node, in a cluster of
          SMPs, then the process computation is multi-threaded, but MPI
          calls will likely execute on a single thread.

          The design accommodates a static specification of the thread
          support level, for environments that require static binding of
          libraries, and for compatibility for current multi-threaded MPI
          codes.

Notes for Fortran

   Note that the Fortran binding for this routine does not have the argc
   and argv arguments. (MPI_INIT_THREAD(required, provided, ierror))

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

   MPI_ERR_OTHER
          Other error; use MPI_Error_string to get more information about
          this error code.

See Also

   MPI_Init, MPI_Finalize

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    int errs = 0;
    int provided, flag, claimed;

    MPI_Init_thread( 0, 0, MPI_THREAD_MULTIPLE, &provided );

    MPI_Is_thread_main( &flag );
    if (!flag) {
        errs++;
        printf( "This thread called init_thread but Is_thread_main gave false\n" );
	fflush(stdout);
    }
    MPI_Query_thread( &claimed );
    if (claimed != provided) {
        errs++;
        printf( "Query thread gave thread level %d but Init_thread gave %d\n",
		claimed, provided );
	fflush(stdout);
    }

    MPI_Finalize();
    return errs;
}

