/*
 * main.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "hdf5.h"

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
	int BE_test_data[] = {20};
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
	hid_t dataset_int_id, dataset_double_id, dataset_char_id, dataset_BE_id, dataset_three_dimension_id;
	hid_t dataspace_two_dimension_id, dataspace_three_dimension_id;
	hsize_t two_dims[2], three_dims[3];

	/*
	 *
	 * open HDF5 files and datasets
	 *
	 */

	file_id = H5Fcreate(HDF5_FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	/* Create the data space for the dataset. */
	two_dims[0] = TEST_FRAME_HEIGHT;
	two_dims[1] = TEST_FRAME_WIDTH;

	three_dims[0] = 3; 	three_dims[1] = 3; 	three_dims[2] = 3;

	dataspace_two_dimension_id = H5Screate_simple(2, two_dims, NULL);
	dataspace_three_dimension_id = H5Screate_simple(3, three_dims, NULL);

	/* Create int dataset. */

	dataset_int_id = H5Dcreate(file_id, HDF5_INT_DATA_SET_NAME, H5T_NATIVE_INT32, dataspace_two_dimension_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite(dataset_int_id, H5T_NATIVE_INT32, H5S_ALL, H5S_ALL, H5P_DEFAULT, int_test_data);

	/* Create double dataset. */

	dataset_double_id = H5Dcreate(file_id, HDF5_DOUBLE_DATA_SET_NAME, H5T_NATIVE_DOUBLE, dataspace_two_dimension_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite(dataset_double_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, double_test_data);

	/* Create char dataset. */

	dataset_char_id = H5Dcreate(file_id, HDF5_CHAR_DATA_SET_NAME, H5T_NATIVE_CHAR, dataspace_two_dimension_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite(dataset_char_id, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, char_test_data);

	/* Create three dimension dataset. */

	dataset_int_id = H5Dcreate(file_id, HDF5_three_dimension_DATA_SET_NAME, H5T_NATIVE_INT32, dataspace_three_dimension_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite(dataset_three_dimension_id, H5T_NATIVE_INT32, H5S_ALL, H5S_ALL, H5P_DEFAULT, three_dimension_test_data);

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
