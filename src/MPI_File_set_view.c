/* 
MPI_File_set_view

   Sets the file view
int MPI_File_set_view(
  MPI_File mpi_fh,
  MPI_Offset disp,
  MPI_Datatype etype,
  MPI_Datatype filetype,
  char *datarep,
  MPI_Info info
);

int MPI_File_set_view(
  MPI_File mpi_fh,
  MPI_Offset disp,
  MPI_Datatype etype,
  MPI_Datatype filetype,
  wchar_t *datarep,
  MPI_Info info
);

Parameters

   mpi_fh
          [in] file handle (handle)

   disp
          [in] displacement (nonnegative integer)

   etype
          [in] elementary datatype (handle)

   filetype
          [in] filetype (handle)

   datarep
          [in] data representation (string)

   info
          [in] info object (handle)

Remarks

   The MPI_FILE_SET_VIEW routine changes the process's view of the data in
   the file. The start of the view is set to disp; the type of data is set
   to etype; the distribution of data to processes is set to filetype; and
   the representation of data in the file is set to datarep. In addition,
   MPI_FILE_SET_VIEW resets the individual file pointers and the shared
   file pointer to zero. MPI_FILE_SET_VIEW is collective; the values for
   datarep and the extents of etype in the file data representation must
   be identical on all processes in the group; values for disp, filetype,
   and info may vary. The datatypes passed in etype and filetype must be
   committed.

   The etype always specifies the data layout in the file. If etype is a
   portable datatype, the extent of etype is computed by scaling any
   displacements in the datatype to match the file data representation. If
   etype is not a portable datatype, no scaling is done when computing the
   extent of etype. The user must be careful when using nonportable etypes
   in heterogeneous environments.

   If MPI_MODE_SEQUENTIAL mode was specified when the file was opened, the
   special displacement MPI_DISPLACEMENT_CURRENT must be passed in disp.
   This sets the displacement to the current position of the shared file
   pointer.

   Rationale.

   For some sequential files, such as those corresponding to magnetic
   tapes or streaming network connections, the displacement may not be
   meaningful. MPI_DISPLACEMENT_CURRENT allows the view to be changed for
   these types of files. ( End of rationale.)
   The disp displacement argument specifies the position (absolute offset
   in bytes from the beginning of the file) where the view begins.

   Advice to users.

   disp can be used to skip headers or when the file includes a sequence
   of data segments that are to be accessed in different patterns (see
   Figure 15 ). Separate views, each using a different displacement and
   filetype, can be used to access each segment.


                                [io-disp.gif]


   Figure 15:

   [ ]Displacements ( End of advice to users.)
   An etype ( elementary datatype) is the unit of data access and
   positioning. It can be any MPI predefined or derived datatype. Derived
   etypes can be constructed by using any of the MPI datatype constructor
   routines, provided all resulting typemap displacements are nonnegative
   and monotonically nondecreasing. Data access is performed in etype
   units, reading or writing whole data items of type etype. Offsets are
   expressed as a count of etypes; file pointers point to the beginning of
   etypes.

   Advice to users.

   In order to ensure interoperability in a heterogeneous environment,
   additional restrictions must be observed when constructing the etype. (
   End of advice to users.)
   A filetype is either a single etype or a derived MPI datatype
   constructed from multiple instances of the same etype. In addition, the
   extent of any hole in the filetype must be a multiple of the etype's
   extent. These displacements are not required to be distinct, but they
   cannot be negative, and they must be monotonically nondecreasing.

   If the file is opened for writing, neither the etype nor the filetype
   is permitted to contain overlapping regions. This restriction is
   equivalent to the "datatype used in a receive cannot specify
   overlapping regions" restriction for communication. Note that filetypes
   from different processes may still overlap each other.

   If filetype has holes in it, then the data in the holes is inaccessible
   to the calling process. However, the disp, etype and filetype arguments
   can be changed via future calls to MPI_FILE_SET_VIEW to access a
   different part of the file.

   It is erroneous to use absolute addresses in the construction of the
   etype and filetype.

   The info argument is used to provide information regarding file access
   patterns and file system specifics to direct optimization. The constant
   MPI_INFO_NULL refers to the null info and can be used when no info
   needs to be specified.

   The datarep argument is a string that specifies the representation of
   data in the file. See the file interoperability section for details and
   a discussion of valid values.

   The user is responsible for ensuring that all nonblocking requests and
   split collective operations on fh have been completed before calling
   MPI_FILE_SET_VIEW---otherwise, the call to MPI_FILE_SET_VIEW is
   erroneous.

*/
 
// Copyright 2009 Deino Software. All rights reserved.
// Source: http://mpi.deino.net/mpi_functions/index.htm

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

/* Test set_view with DISPLACEMENT_CURRENT */
int main( int argc, char *argv[] )
{
    int errs = 0, err;
    int size, rank, *buf;
    MPI_Offset offset;
    MPI_File fh;
    MPI_Comm comm;
    MPI_Status status;
    MPI_Init( &argc, &argv );
    /* This test reads a header then sets the view to every "size" int,
        using set view and current displacement. The file is first written
        using a combination of collective and ordered writes */
    comm = MPI_COMM_WORLD;
    err = MPI_File_open( comm, "test.ord", MPI_MODE_WRONLY |
			 MPI_MODE_CREATE, MPI_INFO_NULL, &fh );
    if (err)
    {
        MPI_Abort(MPI_COMM_WORLD, 911);
    }
    MPI_Comm_size( comm, &size );
    MPI_Comm_rank( comm, &rank );
    buf = (int *)malloc( size * sizeof(int) );
    buf[0] = size;
    err = MPI_File_write_all( fh, buf, 1, MPI_INT, &status );
    if (err) { errs++; }
    err = MPI_File_get_position( fh, &offset );
    if (err) { errs++; }
    err = MPI_File_seek_shared( fh, offset, MPI_SEEK_SET );
    if (err) { errs++; }
    buf[0] = rank;
    err = MPI_File_write_ordered( fh, buf, 1, MPI_INT, &status );
    if (err) { errs++; }
    err = MPI_File_close( &fh );
    if (err) { errs++; }
    /* Reopen the file as sequential */
    err = MPI_File_open( comm, "test.ord", MPI_MODE_RDONLY |
			 MPI_MODE_SEQUENTIAL | MPI_MODE_DELETE_ON_CLOSE,
			 MPI_INFO_NULL, &fh );
    if (err)
    {
        MPI_Abort(MPI_COMM_WORLD, 911);
    }
    if (rank == 0) {
        err = MPI_File_read_shared( fh, buf, 1, MPI_INT, &status );
        if (err) { errs++; }
        if (buf[0] != size) {
            errs++;
            fprintf(stderr, "Unexpected value for the header = %d, should be %d\n",
		    buf[0], size );
	    fflush(stderr);
        }
    }
    MPI_Barrier( comm );
    /* All processes must provide the same file view for
       MODE_SEQUENTIAL */
    err = MPI_File_set_view( fh, MPI_DISPLACEMENT_CURRENT, MPI_INT,
			     MPI_INT, "native", MPI_INFO_NULL );
    if (err) { errs++; }
    buf[0] = -1;
    err = MPI_File_read_ordered( fh, buf, 1, MPI_INT, &status );
    if (err) { errs++; }
    if (buf[0] != rank) {
        errs++;
        fprintf( stderr, "%d: buf[0] = %d\n", rank, buf[0] );
	fflush(stderr);
    }
    free( buf );
    err = MPI_File_close( &fh );
    if (err) { errs++; }

    MPI_Finalize();
    return errs;
}

