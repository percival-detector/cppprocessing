/*
 * percival_HDF5_loader.h
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_HDF5_LOADER_H_
#define INCLUDE_PERCIVAL_HDF5_LOADER_H_

#include "percival_processing.h"

#include "hdf5.h"
#include <exception>
#include <iostream>

class file_exception : public std::exception{

};

class datatype_exception : public std::exception{

};

template<typename T>
void percival_HDF5_writer(
		const char * path_name,
		const char * data_set_name,
		percival_frame<T> & buffer_frame,
		bool print_error = 0
){
	hid_t error_stack;
	herr_t status;
	hid_t file_id, dataset_id, dataspace_id, datatype_id;
	int rank;


	if(print_error == 0){
		herr_t error_id;
		error_id = H5Eset_auto(error_stack, NULL, NULL);
	}


	file_id = H5Fopen(path_name,H5F_ACC_RDONLY, H5P_DEFAULT);
	if(file_id<0)
		throw file_exception{};

	//should_throw_exception_if_dataset_does_not_exist
	dataset_id = H5Dopen2(file_id, data_set_name, H5P_DEFAULT);
	if(dataset_id<0)
			throw file_exception{};

	dataspace_id = H5Dget_space(dataset_id);
	if(dataspace_id<0)
			throw file_exception{};

	rank = H5Sget_simple_extent_ndims(dataspace_id);
	if(rank != 2)
		throw datatype_exception{};

	hsize_t* current_dims, *maximum_dims;

	current_dims = new hsize_t[rank];
	maximum_dims = new hsize_t[rank];

			//Alternative C syntax
			//current_dims = (hsize_t*)malloc(rank*sizeof(hsize_t));
			//maximum_dims = (hsize_t*)malloc(rank*sizeof(hsize_t));

	H5Sget_simple_extent_dims(dataspace_id, current_dims, maximum_dims);



	buffer_frame.set_frame_size(current_dims[0], current_dims[1]);
//
//	//should_throw_if_data_type_is_neither_int_nor_32_bit_float
	delete [] current_dims;
	delete [] maximum_dims;

//==============================================================================================
	//this part is important to know
	datatype_id = H5Dget_type(dataset_id);
//	if(H5Tget_class(datatype_id)==H5T_INTEGER)
//		std::cout << H5Tget_class(datatype_id) << std::endl;
//==============================================================================================



	int size = H5Tget_size(datatype_id);

	if(size != sizeof(T))
		throw datatype_exception{};


	if(H5Tget_order(datatype_id)==H5T_ORDER_BE)
		throw datatype_exception{};

	switch(H5Tget_class(datatype_id)){
		case H5T_INTEGER:
			if(size == 2){
				status = H5Dread (dataset_id, H5T_STD_U16LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer_frame.data);
			}
			else if(size == 4){
				status = H5Dread (dataset_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer_frame.data);
			}
			else{
				throw datatype_exception{};
			}
			break;

		case H5T_FLOAT:
			if(size == 4){
				status = H5Dread (dataset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer_frame.data);
			}
			else if(size == 8){
				status = H5Dread (dataset_id, H5T_IEEE_F64LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer_frame.data);
			}
			else{
				throw datatype_exception{};
			}
			break;

		default:
			throw datatype_exception{};

	}

	status = H5Sclose(dataspace_id);
	status = H5Dclose(dataset_id);
	status = H5Fclose(file_id);
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
