/*
 * main.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "hdf5.h"
#include <string>
#include <iostream>

const int TEST_FRAME_HEIGHT 	= 	160;
const int TEST_FRAME_WIDTH		=	210;
const int RANK 					= 	2;
const std::string HDF5_FILE_NAME			= 	"./data/test_HDF5.h5";
const std::string HDF5_INT_DATA_SET_NAME	= 	"int_sample_frame";
const std::string HDF5_DOUBLE_DATA_SET_NAME	= 	"double_sample_frame";
const std::string HDF5_CHAR_DATA_SET_NAME 	= 	"char_frame";
const std::string HDF5_three_dimension_DATA_SET_NAME 	= 	"three_dimension_data";
const std::string HDF5_BE_DATA_SET_NAME 	= 	"BE_data";

void write_to_test_file(){
	int* int_test_data = new int[TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH];
	double* double_test_data = new double[TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH];

	for(int i = 0; i < TEST_FRAME_HEIGHT; i++){
		for(int j = 0; j < TEST_FRAME_WIDTH; j++){
			*(int_test_data + i * TEST_FRAME_WIDTH + j) = (i + j) % 32768;
			*(double_test_data + i * TEST_FRAME_WIDTH + j) = (i + j) % 32768 * 1.0;
		}
	}
	char char_test_data[] = {'a'};
	int* three_dimension_test_data = new int[27];

	for(int i = 0; i < 27; i++){
		*(three_dimension_test_data + i) = i;
	}

	/*		four corners of int_test_data
	 * 		0	......	209
	 * 		.			.
	 * 		.			.
	 * 		159	......	249
	 */

	/*		four corners of double_test_data
	 * 		0.0		......	209.0
	 * 		.				.
	 * 		.				.
	 * 		159.0	......	249.0
	 */

	//Creating test_HDF5.h5
	herr_t status;
	hid_t file_id;
	hid_t dataset_int_id, dataset_double_id, dataset_char_id, dataset_three_dimension_id;
	hid_t dataspace_char_id, dataspace_two_dimension_id, dataspace_three_dimension_id;
	hsize_t two_dims[2], three_dims[3], one_element[2];

	/*
	 *
	 * open HDF5 files and datasets
	 *
	 */

	file_id = H5Fcreate(HDF5_FILE_NAME.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	if(file_id<0)
		std::cerr << "Unable to open file." << std::endl;

	/* Create the data space for the dataset. */
	one_element[0] = 1; one_element[1]=1;

	two_dims[0] = TEST_FRAME_HEIGHT;
	two_dims[1] = TEST_FRAME_WIDTH;

	three_dims[0] = 3; 	three_dims[1] = 3; 	three_dims[2] = 3;

	dataspace_char_id = H5Screate_simple(2, one_element, NULL);
	dataspace_two_dimension_id = H5Screate_simple(2, two_dims, NULL);
	dataspace_three_dimension_id = H5Screate_simple(3, three_dims, NULL);

	if(dataspace_two_dimension_id<0)
		std::cerr << "Unable to create dataspace_two_dimension." << std::endl;
	if(dataspace_three_dimension_id<0)
		std::cerr << "Unable to create dataspace_three_dimension." << std::endl;

	/* Create int dataset. */

	dataset_int_id = H5Dcreate(file_id, HDF5_INT_DATA_SET_NAME.c_str(), H5T_NATIVE_INT32, dataspace_two_dimension_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if(dataset_int_id<0)
		std::cerr << "Unable to create dataset_int." << std::endl;
	status = H5Dwrite(dataset_int_id, H5T_NATIVE_INT32, H5S_ALL, H5S_ALL, H5P_DEFAULT, int_test_data);
	if(status <0)
		std::cerr << "Unable to write dataset_int." << std::endl;

	/* Create double dataset. */

	dataset_double_id = H5Dcreate(file_id, HDF5_DOUBLE_DATA_SET_NAME.c_str(), H5T_NATIVE_DOUBLE, dataspace_two_dimension_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if(dataset_double_id<0)
		std::cerr << "Unable to create dataset_double." << std::endl;
	status = H5Dwrite(dataset_double_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, double_test_data);
	if(status<0)
		std::cerr << "Unable to write dataset_double." << std::endl;
	/* Create char dataset. */

	dataset_char_id = H5Dcreate(file_id, HDF5_CHAR_DATA_SET_NAME.c_str(), H5T_NATIVE_CHAR, dataspace_char_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if(dataset_char_id<0)
		std::cerr << "Unable to create dataset_char." << std::endl;
	status = H5Dwrite(dataset_char_id, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, char_test_data);
	if(status<0)
		std::cerr << "Unable to write dataset_char." << std::endl;
	/* Create three dimension dataset. */

	dataset_three_dimension_id = H5Dcreate(file_id, HDF5_three_dimension_DATA_SET_NAME.c_str(), H5T_NATIVE_INT32, dataspace_three_dimension_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if(dataset_three_dimension_id<0)
		std::cerr << "Unable to create dataset_three_dimension." << std::endl;
	status = H5Dwrite(dataset_three_dimension_id, H5T_NATIVE_INT32, H5S_ALL, H5S_ALL, H5P_DEFAULT, three_dimension_test_data);
	if(status<0)
		std::cerr << "Unable to write dataset_three_dimension." << std::endl;
	H5close();
}

#include <iostream>
#include <fstream>

int main (void)
{

	write_to_test_file();


	std::ofstream myfile;
	myfile.open ("./data/NotAHDF5File.txt");
	myfile << "This file is used as a dummy file to check the behaviour of C++ HDF5 loader when the user attempts to use a non-HDF5 file.";
	myfile.close();

	return 0;
}
//TODO: write the try catch block properly
