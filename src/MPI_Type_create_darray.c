/* 
MPI_Type_create_darray

   Create a datatype representing a distributed array
int MPI_Type_create_darray(
  int size,
  int rank,
  int ndims,
  int array_of_gsizes[],
  int array_of_distribs[],
  int array_of_dargs[],
  int array_of_psizes[],
  int order,
  MPI_Datatype oldtype,
  MPI_Datatype *newtype
);

Parameters

   size
          [in] size of process group (positive integer)

   rank
          [in] rank in process group (nonnegative integer)

   ndims
          [in] number of array dimensions as well as process grid
          dimensions (positive integer)

   array_of_gsizes
          [in] number of elements of type oldtype in each dimension of
          global array (array of positive integers)

   array_of_distribs
          [in] distribution of array in each dimension (array of state)

   array_of_dargs
          [in] distribution argument in each dimension (array of positive
          integers)

   array_of_psizes
          [in] size of process grid in each dimension (array of positive
          integers)

   order
          [in] array storage order flag (state)

   oldtype
          [in] old datatype (handle)

   newtype
          [out] new datatype (handle)

Remarks

   MPI_TYPE_CREATE_DARRAY can be used to generate the datatypes
   corresponding to the distribution of an ndims-dimensional array of
   oldtype elements onto an ndims-dimensional grid of logical processes.
   Unused dimensions of array_of_psizes should be set to 1. For a call to
   MPI_TYPE_CREATE_DARRAY to be correct, the equation prod[i=0]^ndims-1
   array_of_psizes[i] = size must be satisfied. The ordering of processes
   in the process grid is assumed to be row-major, as in the case of
   virtual Cartesian process topologies in MPI-1.

   Advice to users.

   For both Fortran and C arrays, the ordering of processes in the process
   grid is assumed to be row-major. This is consistent with the ordering
   used in virtual Cartesian process topologies in MPI-1. To create such
   virtual process topologies, or to find the coordinates of a process in
   the process grid, etc., users may use the corresponding functions
   provided in MPI-1. ( End of advice to users.)

   Each dimension of the array can be distributed in one of three ways:
     * MPI_DISTRIBUTE_BLOCK - Block distribution
     * MPI_DISTRIBUTE_CYCLIC - Cyclic distribution
     * MPI_DISTRIBUTE_NONE - Dimension not distributed.

   The constant MPI_DISTRIBUTE_DFLT_DARG specifies a default distribution
   argument. The distribution argument for a dimension that is not
   distributed is ignored. For any dimension i in which the distribution
   is MPI_DISTRIBUTE_BLOCK, it erroneous to specify array_of_dargs[i] *
   array_of_psizes[i] < array_of_gsizes[i].

   For example, the HPF layout ARRAY(CYCLIC(15)) corresponds to
   MPI_DISTRIBUTE_CYCLIC with a distribution argument of 15, and the HPF
   layout ARRAY(BLOCK) corresponds to MPI_DISTRIBUTE_BLOCK with a
   distribution argument of MPI_DISTRIBUTE_DFLT_DARG.

   The order argument is used as in MPI_TYPE_CREATE_SUBARRAY to specify
   the storage order. Therefore, arrays described by this type constructor
   may be stored in Fortran (column-major) or C (row-major) order. Valid
   values for order are MPI_ORDER_FORTRAN and MPI_ORDER_C.

   This routine creates a new MPI datatype with a typemap defined in terms
   of a function called "cyclic()" (see below).

   Without loss of generality, it suffices to define the typemap for the
   MPI_DISTRIBUTE_CYCLIC case where MPI_DISTRIBUTE_DFLT_DARG is not used.

   MPI_DISTRIBUTE_BLOCK and MPI_DISTRIBUTE_NONE can be reduced to the
   MPI_DISTRIBUTE_CYCLIC case for dimension i as follows.

   MPI_DISTRIBUTE_BLOCK with array_of_dargs[i] equal to
   MPI_DISTRIBUTE_DFLT_DARG is equivalent to MPI_DISTRIBUTE_CYCLIC with
   array_of_dargs[i] set to

   (mpiargarray_of_gsizes[i] + mpiargarray_of_psizes[i] - 1) /
   mpiargarray_of_psizes[i].

   If array_of_dargs[i] is not MPI_DISTRIBUTE_DFLT_DARG, then
   MPI_DISTRIBUTE_BLOCK and MPI_DISTRIBUTE_CYCLIC are equivalent.

   MPI_DISTRIBUTE_NONE is equivalent to MPI_DISTRIBUTE_CYCLIC with
   array_of_dargs[i] set to array_of_gsizes[i].

   Finally, MPI_DISTRIBUTE_CYCLIC with array_of_dargs[i] equal to
   MPI_DISTRIBUTE_DFLT_DARG is equivalent to MPI_DISTRIBUTE_CYCLIC with
   array_of_dargs[i] set to 1.

   For MPI_ORDER_FORTRAN, an ndims-dimensional distributed array (
   newtype) is defined by the following code fragment:


    oldtype[0] = oldtype;
    for ( i = 0; i < ndims; i++ ) {
       oldtype[i+1] = cyclic(array_of_dargs[i],
                             array_of_gsizes[i],
                             r[i],
                             array_of_psizes[i],
                             oldtype[i]);
    }
    newtype = oldtype[ndims];

   For MPI_ORDER_C, the code is:


    oldtype[0] = oldtype;
    for ( i = 0; i < ndims; i++ ) {
       oldtype[i + 1] = cyclic(array_of_dargs[ndims - i - 1],
                               array_of_gsizes[ndims - i - 1],
                               r[ndims - i - 1],
                               array_of_psizes[ndims - i - 1],
                               oldtype[i]);
    }
    newtype = oldtype[ndims];


   where r[i] is the position of the process (with rank rank) in the
   process grid at dimension i. The values of r[i] are given by the
   following code fragment:


        t_rank = rank;
        t_size = 1;
        for (i = 0; i < ndims; i++)
                t_size *= array_of_psizes[i];
        for (i = 0; i < ndims; i++) {
            t_size = t_size / array_of_psizes[i];
            r[i] = t_rank / t_size;
            t_rank = t_rank % t_size;
        }

   Let the typemap of oldtype have the form:
   {(type[0],disp[0]),(type[1],disp[1]),...,(type[n-1],disp[n-1])} where
   type[i] is a predefined MPI datatype, and let ex be the extent of
   oldtype.

   Given the above, the function cyclic() is defined as follows:

   [img6.gif]

   where count is defined by this code fragment:

        nblocks = (gsize + (darg - 1)) / darg;
        count = nblocks / psize;
        left_over = nblocks - count * psize;
        if (r < left_over)
            count = count + 1;

   Here, nblocks is the number of blocks that must be distributed among
   the processors. Finally, darg[last] is defined by this code fragment:

        if ((num_in_last_cyclic = gsize % (psize * darg)) == 0)
             darg_last = darg;
        else
             darg_last = num_in_last_cyclic - darg * r;
             if (darg_last > darg)
                    darg_last = darg;
             if (darg_last <= 0)
                    darg_last = darg;

   Example Consider generating the filetypes corresponding to the HPF
   distribution:

      <oldtype> FILEARRAY(100, 200, 300)
!HPF$ PROCESSORS PROCESSES(2, 3)
!HPF$ DISTRIBUTE FILEARRAY(CYCLIC(10), *, BLOCK) ONTO PROCESSES

   This can be achieved by the following Fortran code, assuming there will
   be six processes attached to the run:

    ndims = 3
    array_of_gsizes(1) = 100
    array_of_distribs(1) = MPI_DISTRIBUTE_CYCLIC
    array_of_dargs(1) = 10
    array_of_gsizes(2) = 200
    array_of_distribs(2) = MPI_DISTRIBUTE_NONE
    array_of_dargs(2) = 0
    array_of_gsizes(3) = 300
    array_of_distribs(3) = MPI_DISTRIBUTE_BLOCK
    array_of_dargs(3) = MPI_DISTRIBUTE_DFLT_ARG
    array_of_psizes(1) = 2
    array_of_psizes(2) = 1
    array_of_psizes(3) = 3
    call MPI_COMM_SIZE(MPI_COMM_WORLD, size, ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
    call MPI_TYPE_CREATE_DARRAY(size, rank, ndims, array_of_gsizes, &
         array_of_distribs, array_of_dargs, array_of_psizes,        &
         MPI_ORDER_FORTRAN, oldtype, newtype, ierr)

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
   call statement. All MPI objects (e.g., MPI_Datatype, MPI_Comm) are of
   type INTEGER in Fortran.

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
#include <stdlib.h>
#include <string.h>

/* tests */
int darray_2d_c_test1(void);
int darray_4d_c_test1(void);

/* helper functions */
static int parse_args(int argc, char **argv);
static int pack_and_unpack(char *typebuf, int count, MPI_Datatype
datatype, int typebufsz);

int main(int argc, char **argv)
{
    int err, errs = 0;

    MPI_Init(&argc, &argv);
    /* To improve reporting of problems about operations, 
       we change the error handler to errors return */
    MPI_Comm_set_errhandler( MPI_COMM_WORLD, MPI_ERRORS_RETURN );

    /* perform some tests */
    err = darray_2d_c_test1();
    if (err) {
      fprintf(stderr, "%d errors in 2d darray c test 1.\n",err);
      fflush(stderr);
    }
    errs += err;

    err = darray_4d_c_test1();
    if (err) {
      fprintf(stderr, "%d errors in 4d darray c test 1.\n", err);
      fflush(stderr);
    }
    errs += err;

    MPI_Finalize();
    return errs;
}

/* darray_2d_test1()
 *
 * Performs a sequence of tests building darrays with single-element
 * blocks, running through all the various positions that the element 
 * might come from.
 *
 * Returns the number of errors encountered.
 */
int darray_2d_c_test1(void)
{
    MPI_Datatype darray;
    int array[9]; /* initialized below */
    int array_size[2] = {3, 3};
    int array_distrib[2] = {MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_BLOCK};
    int array_dargs[2] = {MPI_DISTRIBUTE_DFLT_DARG, MPI_DISTRIBUTE_DFLT_DARG};
    int array_psizes[2] = {3, 3};
    int i, rank, err, errs = 0, sizeoftype;

    /* pretend we are each rank, one at a time */
    for (rank=0; rank < 9; rank++) {
        /* set up buffer */
        for (i=0; i < 9; i++) {
            array[i] = i;
        }
        /* set up type */
        err = MPI_Type_create_darray(9 /* size */, rank, 2 /* dims */, array_size,
				     array_distrib, array_dargs, array_psizes,
				     MPI_ORDER_C, MPI_INT, &darray);
        if (err != MPI_SUCCESS) {
            errs++;
            fprintf(stderr, "error in MPI_Type_create_darray call;" 
                    "aborting after %d errors\n", errs);
	    fflush(stderr);
            return errs;
        }
        MPI_Type_commit(&darray);
        MPI_Type_size(darray, &sizeoftype);
        if (sizeoftype != sizeof(int)) {
            errs++;
            fprintf(stderr, "size of type = %d; should be %d\n", sizeoftype,
		    (int) sizeof(int));
	    fflush(stderr);
            return errs;
        }
        err = pack_and_unpack((char *) array, 1, darray, 9*sizeof(int));
        for (i=0; i < 9; i++) {
            if ((i == rank) && (array[i] != rank)) {
                errs++;
                fprintf(stderr, "array[%d] = %d; should be %d\n", i, array[i], rank);
		fflush(stderr);
            }
            else if ((i != rank) && (array[i] != 0)) {
                errs++;
                fprintf(stderr, "array[%d] = %d; should be %d\n", i, array[i], 0);
		fflush(stderr);
            }
        }
        MPI_Type_free(&darray);
    }
    return errs;
}

/* darray_4d_c_test1()
 *
 * Returns the number of errors encountered.
 */
int darray_4d_c_test1(void)
{
    MPI_Datatype darray;
    int array[72];
    int array_size[4] = {6, 3, 2, 2};
    int array_distrib[4] = { MPI_DISTRIBUTE_BLOCK,
                                   MPI_DISTRIBUTE_BLOCK,
                                   MPI_DISTRIBUTE_NONE,
                                   MPI_DISTRIBUTE_NONE };
    int array_dargs[4] = { MPI_DISTRIBUTE_DFLT_DARG,
                                   MPI_DISTRIBUTE_DFLT_DARG,
                                   MPI_DISTRIBUTE_DFLT_DARG,
                                   MPI_DISTRIBUTE_DFLT_DARG };
    int array_psizes[4] = {6, 3, 1, 1};
    int i, rank, err, errs = 0, sizeoftype;

    for (rank=0; rank < 18; rank++) {
        /* set up array */
        for (i=0; i < 72; i++) {
            array[i] = i;
        }

        /* set up type */
        err = MPI_Type_create_darray(18 /* size */, rank, 4 /* dims */,
                                     array_size, array_distrib, array_dargs,
				     array_psizes, MPI_ORDER_C, MPI_INT, &darray);
        if (err != MPI_SUCCESS) {
            errs++;
            fprintf(stderr, "error in MPI_Type_create_darray call;"
                    "aborting after %d errors\n", errs);
	    fflush(stderr);
            return errs;
        }
        MPI_Type_commit(&darray);
        /* verify the size of the type */
        MPI_Type_size(darray, &sizeoftype);
        if (sizeoftype != 4*sizeof(int)) {
            errs++;
            fprintf(stderr, "size of type = %d; should be %d\n",
		    sizeoftype, (int) (4*sizeof(int)));
	    fflush(stderr);
            return errs;
        }
        /* pack and unpack the type, zero'ing out all other values */
        err = pack_and_unpack((char *) array, 1, darray, 72*sizeof(int));
        for (i=0; i < 4*rank; i++) {
            if (array[i] != 0) {
                errs++;
                fprintf(stderr, "array[%d] = %d; should be %d\n", i, array[i], 0);
		fflush(stderr);
            }
        }
        for (i=4*rank; i < 4*rank + 4; i++) {
            if (array[i] != i) {
                errs++;
                fprintf(stderr, "array[%d] = %d; should be %d\n", i, array[i], i);
		fflush(stderr);
            }
        }
        for (i=4*rank+4; i < 72; i++) {
            if (array[i] != 0) {
                errs++;
                fprintf(stderr, "array[%d] = %d; should be %d\n", i, array[i], 0);
		fflush(stderr);
            }
        }
        MPI_Type_free(&darray);
    }
    return errs;
}

/******************************************************************/
/* pack_and_unpack()
 *
 * Perform packing and unpacking of a buffer for the purposes of
checking
 * to see if we are processing a type correctly. Zeros the buffer
between
 * these two operations, so the data described by the type should be in
 * place upon return but all other regions of the buffer should be
zero.
 *
 * Parameters:
 * typebuf - pointer to buffer described by datatype and count that
 * will be packed and then unpacked into
 * count, datatype - description of typebuf
 * typebufsz - size of typebuf; used specifically to zero the buffer
 * between the pack and unpack steps
 *
 */
static int pack_and_unpack(char *typebuf, int count, MPI_Datatype
datatype, int typebufsz)
{
    char *packbuf;
    int err, errs = 0, pack_size, type_size, position;

    err = MPI_Type_size(datatype, &type_size);
    if (err != MPI_SUCCESS) {
        errs++;
        fprintf(stderr, "error in MPI_Type_size call; aborting after %d errors\n", errs);
	fflush(stderr);
        return errs;
    }
    type_size *= count;
    err = MPI_Pack_size(count, datatype, MPI_COMM_SELF, &pack_size);
    if (err != MPI_SUCCESS) {
        errs++;
        fprintf(stderr, "error in MPI_Pack_size call; aborting after %d errors\n", errs);
	fflush(stderr);
        return errs;
    }
    packbuf = (char *) malloc(pack_size);
    if (packbuf == NULL) {
        errs++;
        fprintf(stderr, "error in malloc call; aborting after %d errors\n", errs);
	fflush(stderr);
        return errs;
    }
    position = 0;
    err = MPI_Pack(typebuf, count, datatype, packbuf, type_size, &position, MPI_COMM_SELF);
    if (position != type_size) {
        errs++;
        fprintf(stderr, "position = %d; should be %d (pack)\n", position, type_size);
	fflush(stderr);
    }
    memset(typebuf, 0, typebufsz);
    position = 0;
    err = MPI_Unpack(packbuf, type_size, &position,typebuf,count,datatype, MPI_COMM_SELF);
    if (err != MPI_SUCCESS) {
        errs++;
        fprintf(stderr, "error in MPI_Unpack call; aborting after %d errors\n", errs);
	fflush(stderr);
        return errs;
    }
    free(packbuf);
    if (position != type_size) {
        errs++;
        fprintf(stderr, "position = %d; should be %d (unpack)\n", position, type_size);
	fflush(stderr);
    }
    return errs;
}

