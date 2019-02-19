/* 
MPI_Comm_join

   Create a communicator by joining two processes connected by a socket.
int MPI_Comm_join(
  int fd,
  MPI_Comm *intercomm
);

Parameters

   fd
          [in] socket file descriptor

   intercomm
          [out] new intercommunicator (handle)

Remarks

   MPI_COMM_JOIN is intended for MPI implementations that exist in an
   environment supporting the Berkeley Socket interface [14,17].
   Implementations that exist in an environment not supporting Berkeley
   Sockets should provide the entry point for MPI_COMM_JOIN and should
   return MPI_COMM_NULL.

   This call creates an intercommunicator from the union of two MPI
   processes which are connected by a socket. MPI_COMM_JOIN should
   normally succeed if the local and remote processes have access to the
   same implementation-defined MPI communication universe.

   The socket must be quiescent before MPI_COMM_JOIN is called and after
   MPI_COMM_JOIN returns. More specifically, on entry to MPI_COMM_JOIN, a
   read on the socket will not read any data that was written to the
   socket before the remote process called MPI_COMM_JOIN.

   Advice to users. An MPI implementation may require a specific
   communication medium for MPI communication, such as a shared memory
   segment or a special switch. In this case, it may not be possible for
   two processes to successfully join even if there is a socket connecting
   them and they are using the same MPI implementation.

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

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
//#include <winsock2.h>   // Windows
#include <netdb.h>       // Linux
//#include <ws2tcpip.h> /* socklen_t */   // Windows
#include <sys/socket.h>  // Linux  ( socklen_t )
#include <string.h>

#define COUNT 1024

int main( int argc, char *argv[] )
{
    int sendbuf[COUNT], recvbuf[COUNT], i;
    int err=0, rank, nprocs, errs=0;
    MPI_Comm intercomm;
    int listenfd, connfd, port, namelen;
    struct sockaddr_in cliaddr, servaddr;
    struct hostent *h;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    socklen_t len, clilen;
    MPI_Init( &argc, &argv );
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (nprocs != 2) {
        printf("Run this program with 2 processes\n");
	fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    if (rank == 1) {
        /* server */
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd < 0) {
            printf("server cannot open socket\n");
	    fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD,1);
        }
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = 0;
        err = bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        if (err < 0) {
            errs++;
            printf("bind failed\n");fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD,1);
        }
        len = sizeof(servaddr);
        err = getsockname(listenfd, (struct sockaddr *) &servaddr, &len);
        if (err < 0) {
            errs++;
            printf("getsockname failed\n");
	    fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD,1);
        }
        port = ntohs(servaddr.sin_port);
        MPI_Get_processor_name(hostname, &namelen);
        MPI_Send(hostname, namelen+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&port, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        err = listen(listenfd, 5);
        if (err < 0) {
            errs++;
            printf("listen failed\n");fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
        if (connfd < 0) {
            printf("accept failed\n");
	    fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    else {
        /* client */
        MPI_Recv(hostname, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&port, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        h = gethostbyname(hostname);
        if (h == NULL) {
            printf("gethostbyname failed\n");
	    fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        servaddr.sin_family = h->h_addrtype;
        memcpy((char *) &servaddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
        servaddr.sin_port = htons(port);
        /* create socket */
        connfd = socket(AF_INET, SOCK_STREAM, 0);
        if (connfd < 0) {
            printf("client cannot open socket\n");
	    fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        /* connect to server */
        err = connect(connfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        if (err < 0) {
            errs++;
            printf("client cannot connect\n");
	    fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    /* To improve reporting of problems about operations, we change the
       error handler to errors return */
    MPI_Comm_set_errhandler( MPI_COMM_WORLD, MPI_ERRORS_RETURN );
    err = MPI_Comm_join(connfd, &intercomm);
    if (err)
    {
        errs++;
        printf("Error in MPI_Comm_join %d\n", err);
	fflush(stdout);
    }
    /* To improve reporting of problems about operations, we change the
       error handler to errors return */
    MPI_Comm_set_errhandler( intercomm, MPI_ERRORS_RETURN );
    for (i=0; i<COUNT; i++) {
        recvbuf[i] = -1;
        sendbuf[i] = i + COUNT*rank;
    }
    err = MPI_Sendrecv(sendbuf, COUNT, MPI_INT, 0, 0, recvbuf, COUNT, MPI_INT, 0, 0, intercomm, MPI_STATUS_IGNORE);
    if (err != MPI_SUCCESS) {
        errs++;
        printf( "Error in MPI_Sendrecv on new communicator\n");
	fflush(stdout);
    }
    for (i=0; i<COUNT; i++) {
        if (recvbuf[i] != ((rank+1)%2) * COUNT + i)
	  errs++;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    err = MPI_Comm_disconnect(&intercomm);
    if (err != MPI_SUCCESS) {
        errs++;
        printf( "Error in MPI_Comm_disconnect\n" );
	fflush(stdout);
    }
    
    MPI_Finalize();
    return errs;
}

