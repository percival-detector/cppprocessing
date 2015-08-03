/*
 * percival_HDF5_loader.h
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_HDF5_WRITER_H_
#define INCLUDE_PERCIVAL_HDF5_WRITER_H_

#include "percival_processing.h"

#include "hdf5.h"
#include <typeinfo>
#include <iostream>

template<typename T>
void percival_HDF5_writer(
		percival_frame<T> & src_frame,
		const char * path_name,
		const char * data_set_name,
		bool overwrite_existing = 1,			//default 1
		bool print_error = 0					//default 0
){
	herr_t status;
	hid_t file_id, dataset_id, dataspace_id, datatype_id;
	hsize_t dims[2];
	if(!print_error)
	/* Turn off error handling */
		  H5Eset_auto(H5E_DEFAULT, NULL, NULL);


	/*
	 *
	 * open HDF5 files and datasets
	 *
	 */
	unsigned flags;
	if(overwrite_existing == 0)
		flags = H5F_ACC_EXCL;
	else flags = H5F_ACC_TRUNC;

	file_id = H5Fcreate(path_name, flags, H5P_DEFAULT, H5P_DEFAULT);

	if(file_id < 0){
		H5close();
		throw file_exception{"Unable to write to ", path_name};
	}
	/* Create the data space for the dataset. */
	dims[0] = src_frame.height;
	dims[1] = src_frame.width;

	dataspace_id = H5Screate_simple(2, dims, NULL);

	/* Create the dataset. */


	hid_t memtype_id;

	if(typeid(T) == typeid(short int))
		memtype_id	= H5T_STD_U16LE;
	else if(typeid(T) == typeid(int))
		memtype_id = H5T_STD_U32LE;
	else if(typeid(T) == typeid(float))
		memtype_id = H5T_IEEE_F32LE;
	else if(typeid(T) == typeid(double))
		memtype_id = H5T_IEEE_F64LE;
	else{
		H5close();
		throw datatype_exception{"Datatype is not supported."};
	}

	dataset_id = H5Dcreate(file_id, data_set_name, memtype_id, dataspace_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    status = H5Dwrite(dataset_id, memtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, src_frame.data);

    H5close();
}

#endif /* INCLUDE_PERCIVAL_HDF5_WRITER_H_ */
