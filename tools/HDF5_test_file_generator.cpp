/*
 * main.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "H5Cpp.h"

using namespace H5;

const int TEST_FRAME_HEIGHT 	= 	32;
const int TEST_FRAME_WIDTH		=	512;
const int RANK 					= 	2;
const std::string HDF5_FILE_NAME			= 	"./data/test_HDF5.h5";
const std::string HDF5_INT_DATASET_NAME		= 	"int_sample_frame";
const std::string HDF5_DOUBLE_DATASET_NAME	= 	"double_sample_frame";

void write_to_another_file(){
	int* int_test_data = new int[TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH];
	double* double_test_data = new double[TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH];

	for(int i = 0; i < TEST_FRAME_HEIGHT; i++){
		for(int j = 0; j < TEST_FRAME_WIDTH; j++){
			*(int_test_data + i * TEST_FRAME_WIDTH + j) = (i + j) % 32768;
			*(double_test_data + i * TEST_FRAME_WIDTH + j) = (i + j) % 32768 * 1.0;
		}
	}


	/*		four corners of int_test_data
	 * 		0	......	511
	 * 		.			.
	 * 		.			.
	 * 		31	......	542
	 */

	/*		four corners of double_test_data
	 * 		0.0		......	511.0
	 * 		.				.
	 * 		.				.
	 * 		31.0	......	542.0
	 */

	//Creating test_HDF5.h5
	const H5std_string FILE_NAME(HDF5_FILE_NAME);
	const H5std_string DATA_SET_NAME1(HDF5_INT_DATASET_NAME);
	const H5std_string DATA_SET_NAME2(HDF5_DOUBLE_DATASET_NAME);

	H5File write_file(FILE_NAME,H5F_ACC_TRUNC);


	//creating a dataspace object
	hsize_t dims[RANK];
	dims[0] = TEST_FRAME_HEIGHT;
	dims[1] = TEST_FRAME_WIDTH ;
	DataSpace int_dataspace(RANK,dims);
	DataSpace double_dataspace(RANK,dims);

	//creating a dataset
	DataSet dataset_int = write_file.createDataSet(DATA_SET_NAME1, PredType::STD_U32LE, int_dataspace);
	DataSet dataset_double = write_file.createDataSet(DATA_SET_NAME2,PredType::IEEE_F64LE, double_dataspace);

	//Write to data set
	dataset_int.write(int_test_data, PredType::NATIVE_INT);
	dataset_double.write(double_test_data, PredType::IEEE_F64LE);

	//close the file
	dataset_int.close();
	dataset_double.close();
	write_file.close();

}

#include <iostream>
#include <fstream>

int main (void)
{
	try{
		write_to_another_file();
		std::ofstream myfile;
		myfile.open ("./data/NotAHDF5File.txt");
		myfile << "This file is used as a dummy file to check the behaviour of C++ HDF5 loader when the user attempts to use a non-HDF5 file.";
		myfile.close();


	}catch(FileIException & e){
		return -1;
	}
	return 0;
}
//TODO: write the try catch block properly
