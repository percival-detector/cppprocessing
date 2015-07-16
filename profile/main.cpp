/*
 * main.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_ADC_decode.h"
#include "percival_frame.h"
#include "percival_processing.h"

#include "H5Cpp.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>

using namespace H5;

int* random_array_generator(int height, int width, int* arrayptr){
	srand (time(NULL));
	for(int i = 0; i < width * height; i ++){
		*(arrayptr + i) = rand() % 32768;
	}
	return arrayptr;
}

void write_to_a_file(){
		int width, height;
		width = 10;
		height = 20;
		int array[width * height];
		int* arrayptr = &array[0];
		arrayptr = random_array_generator(height, width, arrayptr);

		//displaying result
		for(int i = 0; i < width * height; i ++){
				std::cout << i << " "<< *(arrayptr + i) << std::endl;
		}

		//creating a file
		const H5std_string FILE_NAME("rand_numbers.h5");
		const H5std_string DATA_SET_NAME("FirstSetOfNumbers");
		H5File write_file(FILE_NAME,H5F_ACC_TRUNC);						//HDF5 object with a constructor

		//creating a dataspace object
		const int RANK = 2;
		hsize_t dims[RANK];
		dims[0] = width;
		dims[1] = height;
		DataSpace dataspace(RANK,dims);

		//creating a dataset
		DataSet dataset = write_file.createDataSet(DATA_SET_NAME, PredType::STD_U32LE, dataspace);

		//Write to data set
		dataset.write(arrayptr, PredType::NATIVE_INT);

		//close the file
		write_file.close();

		//open the file
		H5File read_file(FILE_NAME, H5F_ACC_RDWR);

		//open the dataset
		DataSet read_data_set = read_file.openDataSet(DATA_SET_NAME);

		//creating a buffer for the dataset
		int read_array[width * height];

		//reading in the dataset
		dataset.read( read_array, PredType::NATIVE_INT, dataspace );

		//displaying result
		for(int i = 0; i < width * height; i ++){
					std::cout << i <<  " "<< *(arrayptr + i) <<" "<< *(read_array + i) << std::endl;
		}

		read_data_set.close();
		read_file.close();
}

const int TEST_FRAME_HEIGHT 	= 	32;
const int TEST_FRAME_WIDTH		=	512;
const int RANK 					= 	2;
const std::string HDF5_FILE_NAME			= 	"test.h5";
const std::string HDF5_INT_DATASET_NAME		= 	"int_sample_frame";
const std::string HDF5_DOUBLE_DATASET_NAME	= 	"double_sample_frame";

void write_to_another_file(){
		int* int_test_data = new int[TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH];
		double* double_test_data = new double[TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH];
		int i = 0;
		std::cout << "initialising" << std::endl;
		std::cout << i ++ << std::endl; //0
		for(int i = 0; i < TEST_FRAME_HEIGHT; i++){
			for(int j = 0; j < TEST_FRAME_WIDTH; j++){
				*(int_test_data + i * TEST_FRAME_WIDTH + j) = 1;	//2^15
				*(double_test_data + i * TEST_FRAME_WIDTH + j) = 13253.1415;	//2^15
			}
		}
		std::cout << i ++ << std::endl; //1
		//Creating test_HDF5.h5
		const H5std_string FILE_NAME(HDF5_FILE_NAME);
		const H5std_string DATA_SET_NAME1(HDF5_INT_DATASET_NAME);
		const H5std_string DATA_SET_NAME2(HDF5_DOUBLE_DATASET_NAME);

		H5File write_file(FILE_NAME,H5F_ACC_TRUNC);//<<<<<<<<<===============================================================================================


		std::cout << i ++ << std::endl; //2
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

		std::cout << "finished initialising" << std::endl;
}
int main (void)
{
	percival_frame<int> buffer_frame_int;
	write_to_another_file();
	try{
	percival_HDF5_loader("./data/-NonexistentPath.h5","dataset",buffer_frame_int);
	}catch(FileIException & e){

	};
	write_to_another_file();
	percival_HDF5_loader("./data/NotAHDF5File.txt","dataset",buffer_frame_int);

	std::cout << "Done!" << std::endl;
    return 0;  // successfully terminated
}


