/*
 * percival_HDF5_loader.h
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_HDF5_WRITER_H_
#define INCLUDE_PERCIVAL_HDF5_WRITER_H_

#include "percival_processing.h"
#include "percival_exceptions.h"
#include "hdf5.h"
#include <typeinfo>
#include <iostream>

template<typename T>
void percival_HDF5_writer(
		percival_frame_mem<T> & src_frame,
		const char * path_name,
		const char * data_set_name,
		bool overwrite_existing = 1,			//default 1,  overwrite existing datasets only todo is this a good choice? or prefer overwriting file annd datasets?
		bool print_error = 0					//default 0
){
	herr_t status;
	hid_t file_id, dataset_id, dataspace_id, datatype_id;
	hsize_t dims[2];
	if(!print_error)
	/* Turn off error handling */
		  H5Eset_auto2(H5E_DEFAULT, NULL, NULL);


	/*
	 *
	 * open HDF5 files and datasets
	 *
	 */
	unsigned flags;
//	if(overwrite_existing == 0)
		flags = H5F_ACC_EXCL;
//	else flags = H5F_ACC_TRUNC;

	file_id = H5Fcreate(path_name, flags, H5P_DEFAULT, H5P_DEFAULT);

	if(file_id < 0){
		file_id = H5Fopen(path_name, H5F_ACC_RDWR, H5P_DEFAULT);
		if(file_id < 0){
			H5close();
			throw file_exception("Unable to write to ", path_name);
		}
	}
	/* Create the data space for the dataset. */
	dims[0] = src_frame.height;
	dims[1] = src_frame.width;

	dataspace_id = H5Screate_simple(2, dims, NULL);

	/* Create the dataset. */


	hid_t memtype_id;

	hid_t native_type = H5Tget_native_type(datatype_id, H5T_DIR_ASCEND);

	if( H5Tequal( native_type, H5T_NATIVE_INT16) && (typeid(T) == typeid(short int)) )
			memtype_id = H5T_NATIVE_INT16;

	else if( H5Tequal(native_type, H5T_NATIVE_UINT16) && (typeid(T) == typeid(short unsigned int)) )
			memtype_id = H5T_NATIVE_UINT16;

	else if( H5Tequal( native_type, H5T_NATIVE_INT32) && (typeid(T) == typeid(int)) )
			memtype_id = H5T_NATIVE_INT32;

	else if( H5Tequal( native_type , H5T_NATIVE_UINT32) && (typeid(T) == typeid(unsigned int)) )
			memtype_id = H5T_NATIVE_UINT32;



	else if( H5Tequal( native_type , H5T_NATIVE_FLOAT) && (typeid(T) == typeid(float)) )
			memtype_id = H5T_NATIVE_FLOAT;

	else if( H5Tequal( native_type , H5T_NATIVE_DOUBLE) && (typeid(T) == typeid(double)) )
			memtype_id = H5T_NATIVE_DOUBLE;

	else{
			throw datatype_exception("Invalid input datatype or wrong destination datatype.");
			H5close();
	}

	dataset_id = H5Dcreate2(file_id, data_set_name, memtype_id, dataspace_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	if(dataset_id<0 && overwrite_existing == 1){
		dataset_id = H5Dopen2(file_id, data_set_name, H5P_DEFAULT);
		if(dataset_id<0)
		throw file_exception("Failed to create dataset: ", data_set_name);
	}
    status = H5Dwrite(dataset_id, memtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, src_frame.data);

    H5close();
}

#endif /* INCLUDE_PERCIVAL_HDF5_WRITER_H_ */
