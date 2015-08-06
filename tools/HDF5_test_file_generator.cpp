/*
 * main.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "H5Cpp.h"

using namespace H5;

const int TEST_FRAME_HEIGHT 	= 	160;
const int TEST_FRAME_WIDTH		=	210;
const int RANK 					= 	2;
const std::string HDF5_FILE_NAME			= 	"./data/test_HDF5.h5";
const std::string HDF5_INT_DATA_SET_NAME	= 	"int_sample_frame";
const std::string HDF5_DOUBLE_DATA_SET_NAME	= 	"double_sample_frame";
const std::string HDF5_CHAR_DATA_SET_NAME 	= 	"char_frame";
const std::string HDF5_three_dimension_DATA_SET_NAME 	= 	"three_dimension_data";
const std::string HDF5_BE_DATA_SET_NAME 	= 	"BE_data";

void write_to_another_file(){
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

	H5File write_file(HDF5_FILE_NAME,H5F_ACC_TRUNC);

	//creating a dataspace object for int and double
	hsize_t dims[RANK];
	dims[0] = TEST_FRAME_HEIGHT;	dims[1] = TEST_FRAME_WIDTH ;
	DataSpace int_dataspace(RANK,dims);
	DataSpace double_dataspace(RANK,dims);

	//creating a dataspace object for char and Big Endian data
	hsize_t char_dims[RANK];
	char_dims[0] = 1; char_dims[1] = 1;
	DataSpace char_dataspace(RANK,char_dims);
	DataSpace BE_dataspace(RANK,char_dims);

	//creating a dataspace object for char and Big Endian data
	hsize_t three_dimension_dims[3];
	three_dimension_dims[0] = 3; three_dimension_dims[1] = 3; three_dimension_dims[2] = 3;
	DataSpace three_dimension_dataspace(3,three_dimension_dims);

	//creating a dataset
	DataSet dataset_int = write_file.createDataSet(HDF5_INT_DATA_SET_NAME, PredType::NATIVE_INT, int_dataspace);
	DataSet dataset_double = write_file.createDataSet(HDF5_DOUBLE_DATA_SET_NAME,PredType::IEEE_F64LE, double_dataspace);
	DataSet dataset_char = write_file.createDataSet(HDF5_CHAR_DATA_SET_NAME, PredType::NATIVE_CHAR, char_dataspace);
	DataSet dataset_BE = write_file.createDataSet(HDF5_BE_DATA_SET_NAME, PredType::STD_U32BE, BE_dataspace);
	DataSet dataset_three_dimension = write_file.createDataSet(HDF5_three_dimension_DATA_SET_NAME, PredType::NATIVE_INT, three_dimension_dataspace);

	//Write to data set
	dataset_int.write(int_test_data, PredType::NATIVE_INT);
	dataset_double.write(double_test_data, PredType::IEEE_F64LE);
	dataset_char.write(char_test_data, PredType::NATIVE_CHAR);
	dataset_BE.write(BE_test_data, PredType::STD_U32BE);
	dataset_three_dimension.write(three_dimension_test_data, PredType::NATIVE_INT);

	//close the file
	dataset_int.close();
	dataset_double.close();
	dataset_char.close();
	dataset_BE.close();
	dataset_three_dimension.close();

	write_file.close();

}


#include <iostream>
#include <fstream>

int main (void)
{

		write_to_another_file();


		std::ofstream myfile;
		myfile.open ("./data/NotAHDF5File.txt");
		myfile << "This file is used as a dummy file to check the behaviour of C++ HDF5 loader when the user attempts to use a non-HDF5 file.";
		myfile.close();

	return 0;
}
//TODO: write the try catch block properly
