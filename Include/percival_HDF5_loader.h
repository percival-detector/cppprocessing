/*
 * percival_HDF5_loader.h
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_HDF5_LOADER_H_
#define INCLUDE_PERCIVAL_HDF5_LOADER_H_

#include "percival_processing.h"

#include "H5Cpp.h"
#include <string>
#include <iostream>

#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif


template<typename T>
void percival_HDF5_loader(
		const std::string & path_name,
		const std::string & data_set_name,
		percival_frame<T> & buffer_frame
){
	Exception::dontPrint();
	int i = 0;
	std::cout << i++ << std::endl; //0
	//should_throw_exception_if_input_path_does_not_exist
	H5File file;
	file.openFile(path_name, H5F_ACC_RDONLY);
	std::cout << i++ << std::endl; //1
	//should_throw_exception_if_dataset_does_not_exist
	DataSet read_data_set = file.openDataSet(data_set_name);
	std::cout << i++ << std::endl; //2

	DataSpace read_dataspace = read_data_set.getSpace();

	int rank = read_dataspace.getSimpleExtentNdims();
	std::cout << i++ << std::endl; //3
	if(rank != 2)
		throw DataSetIException{};

	std::cout << i++ << std::endl; //4
	hsize_t dims_out[2];
	int ndims = read_dataspace.getSimpleExtentDims( dims_out, NULL);

	int percival_frame_height = dims_out[0];
	int percival_frame_width  = dims_out[1];


	//should_generate_output_of_the_same_size_as_input
	buffer_frame.set_frame_size(percival_frame_height, percival_frame_width);

	//should_throw_if_data_type_is_neither_int_nor_32_bit_float
	H5T_class_t type_class = read_data_set.getTypeClass();

	if( (type_class == H5T_INTEGER)){
		//should_throw_exception_if_input_percival_frame_is_of_wrong_type_float_vs_int
		//todo: check if T is of type int
		std::cout << i++ << std::endl; //6
		IntType intype = read_data_set.getIntType();
		H5std_string order_string;
		H5T_order_t order = intype.getOrder( order_string );

//		if(order == H5T_ORDER_BE )
//			throw DataTypeIException{};
		//should_preserve_data_integrity_int
		read_data_set.read( buffer_frame.data, data_set_name, H5T_INTEGER, read_dataspace );
	}

//
//
//	else if((type_class == H5T_FLOAT)){
//
//		//should_throw_exception_if_input_percival_frame_is_of_wrong_type_float_vs_int
//		//todo check if T is float
//		FloatType floattype = read_data_set.getFloatType();
//		//todo check for 32/64 bits. 64 for the time being. 32 in real functions
//		//should_generate_output_of_the_same_size_as_input
//
//		std::cout << i++ << std::endl; //10
//		//should_preserve_data_integrity_double
//		read_data_set.read( buffer_frame.data, H5T_FLOAT, read_dataspace );
//	}
//	else{
//		std::cout << i++ << std::endl; //11
//		throw DataTypeIException{};
//
//	} //todo exception
//	std::cout << i++ << std::endl; //11
//	//todo: write exception for each of the throws above, pay particular attention to the message.
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
