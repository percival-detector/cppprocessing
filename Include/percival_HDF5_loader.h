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
void percival_HDF5_loader( const std::string & path_name, const std::string & dataset_name, percival_frame<T> buffer_frame)
{

	//should_throw_exception_if_input_path_does_not_exist
		Exception::dontPrint();

		const H5std_string FILE_NAME( path_name );
		const H5std_string DATA_SET_NAME( dataset_name );
		//open the file
		int i = 0;
		std::cout << "in function loader" << i ++ << std::endl;
		H5File file;
		std::cout << "in function loader" << i ++ << std::endl;
		file.openFile(FILE_NAME, H5F_ACC_RDONLY);
		std::cout << "in function loader" << i ++ << std::endl;


	//should_throw_exception_if_dataset_does_not_exist
	//should_throw_if_data_type_is_neither_int_nor_32_bit_float
	//should_generate_output_of_the_same_size_as_input
	//should_throw_exception_if_input_percival_frame_is_of_wrong_type_float_vs_int
	//should_preserve_data_integrity_int
	//should_preserve_data_integrity_double
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
