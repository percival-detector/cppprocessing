/*
 * test_HDF5_loader.cpp
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"

#include "percival_HDF5_loader.h"

#include <boost/test/unit_test.hpp>
#include <H5Cpp.h>
#include <string>

#include <iostream>

#ifndef H5_NO_NAMESPACE
    using namespace H5;
#endif

//defining parameters for test fixtures
const int TEST_FRAME_HEIGHT 	= 	32;
const int TEST_FRAME_WIDTH		=	512;
const int RANK 					= 	2;
const std::string HDF5_FILE_NAME			= 	"./data/test_HDF5.h5";
const std::string HDF5_INT_DATASET_NAME		= 	"int_sample_frame";
const std::string HDF5_DOUBLE_DATASET_NAME	= 	"double_sample_frame";

class fixture_HDF5{
public:
	percival_frame<int> buffer_frame_int;
	//percival_frame<double> buffer_frame_double;
	//percival_frame<char> buffer_frame_char;

	static int int_test_data[];
	//static double double_test_data[];

	fixture_HDF5(){
		int i = 0;
		BOOST_REQUIRE_NO_THROW(
		std::cout << "initialising" << std::endl;
		std::cout << i ++ << std::endl; //0
//		for(int i = 0; i < TEST_FRAME_HEIGHT; i++){
//			for(int j = 0; j < TEST_FRAME_WIDTH; j++){
//				*(int_test_data + i * TEST_FRAME_WIDTH + j) = 1;	//2^15	//todo change range to the full range of data
//				*(double_test_data + i * TEST_FRAME_WIDTH + j) = 13253.1415;	//2^15
//			}
//		}
		std::cout << i ++ << std::endl; //1
		//Creating test_HDF5.h5
		const H5std_string FILE_NAME(HDF5_FILE_NAME);
		const H5std_string DATA_SET_NAME1(HDF5_INT_DATASET_NAME);
		//const H5std_string DATA_SET_NAME2(HDF5_DOUBLE_DATASET_NAME);
		//TODO FIX THIS BUGGGGGGGG!!!!!
		H5File write_file(FILE_NAME,H5F_ACC_TRUNC);//<<<<<<<<<===============================================================================================


		std::cout << i ++ << std::endl; //2
		//creating a dataspace object
		hsize_t dims[RANK];
		dims[0] = TEST_FRAME_HEIGHT;
		dims[1] = TEST_FRAME_WIDTH ;
		DataSpace int_dataspace(RANK,dims);
		//DataSpace double_dataspace(RANK,dims);

		//creating a dataset
		DataSet dataset_int = write_file.createDataSet(DATA_SET_NAME1, PredType::STD_U32LE, int_dataspace);
		//DataSet dataset_double = write_file.createDataSet(DATA_SET_NAME2,PredType::IEEE_F64LE, double_dataspace);

		//Write to data set
		dataset_int.write(int_test_data, PredType::NATIVE_INT);
		//dataset_double.write(double_test_data, PredType::IEEE_F64LE);

		//close the file
		dataset_int.close();
		//dataset_double.close();
		write_file.close();

		buffer_frame_int.set_frame_size(1,1);
		//buffer_frame_double.set_frame_size(1,1);
		//buffer_frame_char.set_frame_size(1,1);

		std::cout << "finished initialising" << std::endl;
		);
	}
	~fixture_HDF5() { BOOST_TEST_MESSAGE( "teardown fixture" ); }
};

int fixture_HDF5::int_test_data[TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH] = {1};
//double fixture_HDF5::double_test_data[TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH] = {1.234};

BOOST_FIXTURE_TEST_SUITE(percival_HDF5_loader_test,fixture_HDF5)

BOOST_AUTO_TEST_CASE ( should_throw_exception_if_input_path_does_not_exist ){
	std::cout << "test one started" << std::endl;
	BOOST_REQUIRE_THROW(percival_HDF5_loader("./data/-NonexistentPath.h5","dataset",buffer_frame_int),FileIException);	//exception provided by H5Cpp.h
	std::cout << "test one finished" << std::endl;
}

BOOST_AUTO_TEST_CASE ( should_throw_exception_if_input_file_is_not_a_HDF5_file ){
//	BOOST_REQUIRE_EQUAL(1,1);
	//TODO: create a .txt file
	std::cout << "test two started" << std::endl;
	BOOST_REQUIRE_THROW(percival_HDF5_loader("./data/NotAHDF5File.txt","dataset",buffer_frame_int),FileIException);	//exception provided by H5Cpp.h
	std::cout << "test two finished" << std::endl;
}
//
//BOOST_AUTO_TEST_CASE ( should_throw_exception_if_dataset_does_not_exist ){
//
//	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME,"-NonexistentDataSet",buffer_frame_int),DataSetIException);	//TODO: modify the type of template function
//	std::cout << "test two finished" << std::endl;
//}// nonexistent groups are also tested
//
//BOOST_AUTO_TEST_CASE ( should_throw_if_data_type_is_neither_int_nor_32_bit_float ){
//	//Creating test_HDF5.h5
//	const std::string & CHAR_FRAME = "char_frame";
//
//	const H5std_string FILE_NAME(HDF5_FILE_NAME);
//	const H5std_string DATA_SET_NAME_CHAR(CHAR_FRAME);
//	H5File write_file(FILE_NAME,H5F_ACC_TRUNC);
//
//	//creating a dataspace object
//	hsize_t dims[RANK];
//	dims[0] = 1; dims[1] = 1;
//	DataSpace char_dataspace(RANK,dims);
//
//	//creating a dataset
//	DataSet dataset_char = write_file.createDataSet(DATA_SET_NAME_CHAR, PredType::NATIVE_CHAR, char_dataspace);
//	//creating data buffer
//	char char_test_data[] = {'a'};
//	//Write to data set
//	dataset_char.write(char_test_data, PredType::NATIVE_CHAR);
//
//	//close the file
//	dataset_char.close();
//	write_file.close();
//
//	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME,CHAR_FRAME, buffer_frame_char),FileIException);	//TODO: write an exception class for this
//
//	std::cout << "test three finished" << std::endl;
//}	//TODO:buffer_frame_char in class fixture HDF5
//
//BOOST_AUTO_TEST_CASE ( should_generate_output_of_the_same_size_as_input  ){
//	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATASET_NAME, buffer_frame_int));
//	BOOST_REQUIRE_EQUAL(buffer_frame_int.width, TEST_FRAME_WIDTH);
//	BOOST_REQUIRE_EQUAL(buffer_frame_int.height, TEST_FRAME_HEIGHT);
//	std::cout << "test four finished" << std::endl;
//}
//
//BOOST_AUTO_TEST_CASE ( should_throw_exception_if_input_percival_frame_is_of_wrong_type_float_vs_int ){
//	//should throw exception for wrong types -> guarantteed by language
//	//should not throw exception for correct types
//	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATASET_NAME, buffer_frame_int));
//	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_DOUBLE_DATASET_NAME, buffer_frame_double));
//	std::cout << "test five finished" << std::endl;
//}
//
//BOOST_AUTO_TEST_CASE ( should_preserve_data_integrity_int ){
//	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATASET_NAME, buffer_frame_int));
//	//four corners
//	BOOST_REQUIRE_EQUAL(*(buffer_frame_int.data), *(int_test_data));
//	BOOST_REQUIRE_EQUAL(*(buffer_frame_int.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), *(int_test_data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1));	//possibly memory access violation
//	std::cout << "test six finished" << std::endl;
//}
//
//BOOST_AUTO_TEST_CASE ( should_preserve_data_integrity_double ){
//	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_DOUBLE_DATASET_NAME, buffer_frame_double));
//	//four corners
//	BOOST_REQUIRE_CLOSE(*(buffer_frame_double.data), *(int_test_data), 0.00001);
//	BOOST_REQUIRE_CLOSE(*(buffer_frame_double.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), *(int_test_data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), 0.00001);	//possibly memory access violation
//	std::cout << "test seven finished" << std::endl;
//}

BOOST_AUTO_TEST_SUITE_END()

//TODO:should avoid opened files

//
//Unit_test guidelines:
//	1. should take in
//		- path to HDF5 files
//			throw an exception if path does not exist, use default H5Cpp exceptions
//		- pointer to a plain percival_frame buffer
//			should throw an exception if type mismatch, can use language facilities
//		- possibly other options for file.read()
//
//		- a property struct detailing the metadata of the HDF5 file
//			language facilities
//
//	2. should write to the plain image buffer
//		- integrity of the data should be preserved
//			write_data -> HDF5 -> HDF5 -> read_data -> data read by loader
//				compare
//
//	3. should write to the property struct
//		- should preserve data integrity
//			same procedure as in 2
//
//	4. should close HDF5 file properly
//		default H5Cpp exceptions
//
//	5. should automatically check HDF5 image size and set input percival_frame to be of the same size
//		check input and output size
//
