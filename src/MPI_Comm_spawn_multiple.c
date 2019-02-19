/* 
MPI_Comm_spawn_multiple

   While MPI_COMM_SPAWN is sufficient for most cases, it does not allow
   the spawning of multiple binaries, or of the same binary with multiple
   sets of arguments. The following routine spawns multiple binaries or
   the same binary with multiple sets of arguments, establishing
   communication with them and placing them in the same MPI_COMM_WORLD.
int MPI_Comm_spawn_multiple(
  int count,
  char *array_of_commands[],
  char* *array_of_argv[],
  int array_of_maxprocs[],
  MPI_Info array_of_info[],
  int root,
  MPI_Comm comm,
  MPI_Comm *intercomm,
  int array_of_errcodes[]
);

int MPI_Comm_spawn_multiple(
  int count,
  wchar_t *array_of_commands[],
  wchar_t* *array_of_argv[],
  int array_of_maxprocs[],
  MPI_Info array_of_info[],
  int root,
  MPI_Comm comm,
  MPI_Comm *intercomm,
  int array_of_errcodes[]
);

Parameters

   count
          [in] number of commands (positive integer, significant to MPI
          only at root

   array_of_commands
          [in] programs to be executed (array of strings, significant only
          at root)

   array_of_argv
          [in] arguments for commands (array of array of strings,
          significant only at root)

   array_of_maxprocs
          [in] maximum number of processes to start for each command
          (array of integer, significant only at root)

   array_of_info
          [in] info objects telling the runtime system where and how to
          start processes (array of handles, significant only at root)

   root
          [in] rank of process in which previous arguments are examined
          (integer)

   comm
          [in] intracommunicator containing group of spawning processes
          (handle)

   intercomm
          [out] intercommunicator between original group and newly spawned
          group (handle)

   array_of_errcodes
          [out] one error code per process (array of integer)

Info argument

   DeinoMPI uses the info argument to specify additional control
   parameters to each spawn command.  Here are the supported keys:
     * path - search path to locate the executable. Use semicolons (;) to
       separate paths.  Example: c:\temp;c:\bin
     * host - host name to launch the processes on.  All count[i]
       processes will be launched on this host.
     * wdir - working directory
     * env - list of environment variables in the form: env=var env2=var2
       ...  Variables with spaces or = characters in them should be
       quoted.  The quote and escape characters need to be escaped within
       quoted strings.  For example: name="John Doe" random="He said,
       \"put it in c:\\temp\""
     * hosts - list of hosts where to deposit count[i] processes.  The
       form is: hostA[:n] hostB[:m] where :n is an optional way to deposit
       more than one process per host.  Example: hostA hostB:2 hostC.  If
       count = 5 the processes will be deposited as follows: hostA hostB
       hostB hostC hostA
     * machinefile - file to be read to create a host list.  The format of
       the file is one host per line with blank lines and lines beginning
       with # ignored.  Multiple processes per host can be specified by
       specifying the host name as follows: hostA:n.  Hosts are selected
       round robin from the list until count[i] number of processes are
       launched.
     * map - list of network drives to map before launching the
       processes.  The format is: drive:\\share;drive2:\\share2.  Example:
       z:\\myserver\home\userA;y:\\myserver\data
     * localonly - launch the count[i] processes only on the host that
       performs the spawn.  This is the host that executes mpiexec, not
       the host of the root process that called spawn.
     * priority - set the priority of the launched count[i] processes.
       Format: priority class[:thread priority].  You can specify the
       process priority and optionally the thread priority.  The classes
       are: 0,1,2,3,4 (idle, below, normal, above, high).  The thread
       priorities are: 0,1,2,3,4,5 (idle, lowest, below, normal, above,
       highest).  The default is 2:3
     * exitcodes - print the exit codes of the spawned processes when the
       group exits.  Currently this causes all processes from the spawn
       command to print their exit codes, not just the count[i] processes
       associated with the info argument.
     * log - shortcut to enable MPE logging of the spawned group.  If this
       key is specified in any info argument it must be specified in all
       of them since it is not permitted to log a subset of processes of
       MPI_COMM_WORLD.

Remarks

   MPI_COMM_SPAWN_MULTIPLE is identical to MPI_COMM_SPAWN except that
   there are multiple executable specifications. The first argument,
   count, gives the number of specifications. Each of the next four
   arguments are simply arrays of the corresponding arguments in
   MPI_COMM_SPAWN. For the Fortran version of array_of_argv, the element
   array_of_argv(i,j) is the jth argument to command number i.
   Rationale.

   This may seem backwards to Fortran programmers who are familiar with
   Fortran's column-major ordering. However, it is necessary to do it this
   way to allow MPI_COMM_SPAWN to sort out arguments. Note that the
   leading dimension of array_of_argv must be the same as count. ( End of
   rationale.)
   Advice to users.

   The argument count is interpreted by MPI only at the root, as is
   array_of_argv. Since the leading dimension of array_of_argv is count, a
   non-positive value of count at a non-root node could theoretically
   cause a runtime bounds check error, even though array_of_argv should be
   ignored by the subroutine. If this happens, you should explicitly
   supply a reasonable value of count on the non-root nodes. ( End of
   advice to users.)

   In any language, an application may use the constant MPI_ARGVS_NULL
   (which is likely to be (char ***)0 in C) to specify that no arguments
   should be passed to any commands. The effect of setting individual
   elements of array_of_argv to MPI_ARGV_NULL is not defined. To specify
   arguments for some commands but not others, the commands without
   arguments should have a corresponding argv whose first element is null
   ( (char *)0 in C and empty string in Fortran).

   All of the spawned processes have the same MPI_COMM_WORLD. Their ranks
   in MPI_COMM_WORLD correspond directly to the order in which the
   commands are specified in MPI_COMM_SPAWN_MULTIPLE. Assume that m[1]
   processes are generated by the first command, m[2] by the second, etc.
   The processes corresponding to the first command have ranks 0, 1, ...,
   m[1]-1. The processes in the second command have ranks m[1], m[1]+1,
   ..., m[1]+m[2]-1. The processes in the third have ranks m[1]+m[2],
   m[1]+m[2]+1, ..., m[1]+m[2]+m[3]-1, etc.

   Advice to users.

   Calling MPI_COMM_SPAWN multiple times would create many sets of
   children with different MPI_COMM_WORLDs whereas MPI_COMM_SPAWN_MULTIPLE
   creates children with a single MPI_COMM_WORLD, so the two methods are
   not completely equivalent. There are also two performance-related
   reasons why, if you need to spawn multiple executables, you may want to
   use MPI_COMM_SPAWN_MULTIPLE instead of calling MPI_COMM_SPAWN several
   times. First, spawning several things at once may be faster than
   spawning them sequentially. Second, in some implementations,
   communication between processes spawned at the same time may be faster
   than communication between processes spawned separately. ( End of
   advice to users.)
   The array_of_errcodes argument is 1-dimensional array of size
   [img9.gif] , where n[i] is the ith element of array_of_maxprocs.
   Command number i corresponds to the n[i] contiguous slots in this array
   from element [img10.gif] to [img11.gif] . Error codes are treated as
   for MPI_COMM_SPAWN.

   Examples of array_of_argv in C and Fortran

   To run the program "ocean" with arguments "-gridfile" and "ocean1.grd"
   and the program "atmos" with argument "atmos.grd" in C:

       char *array_of_commands[2] = {"ocean", "atmos"};
       char **array_of_argv[2];
       char *argv0[] = {"-gridfile", "ocean1.grd", (char *)0};
       char *argv1[] = {"atmos.grd", (char *)0};
       array_of_argv[0] = argv0;
       array_of_argv[1] = argv1;
       MPI_Comm_spawn_multiple(2, array_of_commands, array_of_argv, ...);

   Here's how you do it in Fortran:

       CHARACTER*25 commands(2), array_of_argv(2, 3)
       commands(1) = ' ocean '
       array_of_argv(1, 1) = ' -gridfile '
       array_of_argv(1, 2) = ' ocean1.grd'
       array_of_argv(1, 3) = ' '

       commands(2) = ' atmos '
       array_of_argv(2, 1) = ' atmos.grd '
       array_of_argv(2, 2) = ' '

       call MPI_COMM_SPAWN_MULTIPLE(2, commands, array_of_argv, ...)


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

   MPI_ERR_ARG
          Invalid argument. Some argument is invalid and is not identified
          by a specific error class (e.g., MPI_ERR_RANK).

   MPI_ERR_INFO
          Invalid Info

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )
{
    int np[2] = { 1, 1 };
    int errcodes[2];
    MPI_Comm parentcomm, intercomm;
    char *cmds[2] = { "spawn_example", "spawn_example" };
    MPI_Info infos[2] = { MPI_INFO_NULL, MPI_INFO_NULL };
    MPI_Init( &argc, &argv );
    MPI_Comm_get_parent( &parentcomm );
    if (parentcomm == MPI_COMM_NULL)
    {
        /* Create 2 more processes - this example must be called
           spawn_example.exe for this to work. */
        MPI_Comm_spawn_multiple( 2, cmds, MPI_ARGVS_NULL, np, infos, 0,
				 MPI_COMM_WORLD, &intercomm, errcodes );
        printf("I'm the parent.\n");
    }
    else
    {
        printf("I'm the spawned.\n");
    }
    
    fflush(stdout);

    MPI_Finalize();
    return 0;
}

