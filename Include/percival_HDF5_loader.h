/*
 * percival_HDF5_loader.h
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_HDF5_LOADER_H_
#define INCLUDE_PERCIVAL_HDF5_LOADER_H_

#include "percival_processing.h"
#include "percival_exceptions.h"

#include "hdf5.h"
#include <exception>
#include <typeinfo>

template<typename T>
void transpose_percival_frame(percival_frame<T> & original_frame){
	//rather expensive operations

	percival_frame<T> temp_frame;
	int original_width = original_frame.width;
	int original_height = original_frame.height;

	int new_width	= original_height;
	int new_height	= original_width;

	temp_frame.set_frame_size(new_height, new_width);

	for(int i = 0; i < original_width; i ++){
		for(int j = 0; j < original_height; j ++)
			*(temp_frame.data + j + i * original_height) = *(original_frame.data + i + j * original_width);
	}

	original_frame.set_frame_size(new_height, new_width);

	for(int i = 0; i < new_width; i ++){
		for(int j = 0; j < new_height; j ++)
			*(original_frame.data + i + j * new_width) = *(temp_frame.data + i + j * new_width);
	}
}

template<typename T>
void percival_HDF5_loader(
		const char * path_name,
		const char * data_set_name,
		percival_frame<T> & buffer_frame,
		bool transposed = 0,
		bool print_error = 0			//default 0
){
	herr_t status;
	hid_t file_id, dataset_id, dataspace_id, datatype_id;

	if(!print_error)
	/* Turn off error handling */
	  H5Eset_auto2(H5E_DEFAULT, NULL, NULL);

	/*
	 *
	 * open HDF5 files and datasets
	 *
	 */
	file_id = H5Fopen(path_name,H5F_ACC_RDONLY, H5P_DEFAULT);
	if(file_id<0){
		H5close();
		throw file_exception(path_name, " does not exist or is not an HDF5 file. Failed to open.");
	}
	//should_throw_exception_if_dataset_does_not_exist
	dataset_id = H5Dopen2(file_id, data_set_name, H5P_DEFAULT);
	if(dataset_id<0){
		H5close();
		throw file_exception(data_set_name, " dataset does not exist. Failed to open.");
	}

	dataspace_id = H5Dget_space(dataset_id);
	if(dataspace_id<0){
		H5close();
		throw file_exception(path_name, " Fail to get dataspace.");
	}
	/*
	 *
	 *
	 * examining data properties, dimension, type, size, order etc
	 *
	 */
	int rank;
	rank = H5Sget_simple_extent_ndims(dataspace_id);
	if(rank != 2){
		H5close();
		throw dataspace_exception("Dimension != 2.");
	}
	hsize_t* current_dims, *maximum_dims;

	current_dims = new hsize_t[rank];
	maximum_dims = new hsize_t[rank];

	//Alternative C syntax
	//current_dims = (hsize_t*)malloc(rank*sizeof(hsize_t));
	//maximum_dims = (hsize_t*)malloc(rank*sizeof(hsize_t));

	H5Sget_simple_extent_dims(dataspace_id, current_dims, maximum_dims);
	buffer_frame.set_frame_size(current_dims[0], current_dims[1]);
	//
	//
	delete [] current_dims;
	delete [] maximum_dims;

	datatype_id = H5Dget_type(dataset_id);

	if(H5Tget_order(datatype_id)==H5T_ORDER_BE){
		H5close();
		throw datatype_exception("Only little endian data is permitted.");
	}

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
			throw datatype_exception("percival_HDF5_loader: Invalid input datatype or wrong destination datatype.");
			H5close();
	}


	//only little endian, unsigned int, unsigned short int, single and double float are accepted

	status = H5Dread (dataset_id, memtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer_frame.data);	//todo change these four read statement into one

	if(status < 0){
		H5close();
		throw file_exception(path_name, " cannot be read.");
	}

	if(transposed == 1){
		transpose_percival_frame(buffer_frame);
	}
	H5close();
}

#endif /* INCLUDE_PERCIVAL_HDF5_LOADER_H_ */



//Specification
//	1. should take in
//		- path to HDF5 files
//		- pointer to a plain percival_frame buffer
//		- possibly other options for file.read()
//		- a property struct detailing the metadata of the HDF5 file
//	2. should write to the plain image buffer
//		- integrity of the data should be preserved
//	3. should write to the property struct
//		- should preserve data integrity
//	4. should close HDF5 file properly
//	5. should automatically check HDF5 image size and set input percival_frame to be of the same size
