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
const std::string HDF5_INT_DATA_SET_NAME	= 	"int_sample_frame";
const std::string HDF5_DOUBLE_DATA_SET_NAME	= 	"double_sample_frame";
const std::string HDF5_CHAR_DATA_SET_NAME 	= 	"char_frame";
const std::string HDF5_three_dimension_DATA_SET_NAME 	= 	"three_dimension_data";
const std::string HDF5_BE_DATA_SET_NAME 	= 	"BE_data";

class fixture_HDF5{
	public:
		percival_frame<int> int_buffer_frame;
		percival_frame<double> double_buffer_frame;
};


BOOST_FIXTURE_TEST_SUITE(percival_HDF5_loader_test, fixture_HDF5)

BOOST_AUTO_TEST_CASE ( should_throw_exception_if_input_path_does_not_exist ){
	BOOST_REQUIRE_THROW(percival_HDF5_loader("./data/-NonexistentPath.h5","dataset",int_buffer_frame),FileIException);	//exception provided by H5Cpp.h
}

BOOST_AUTO_TEST_CASE ( should_throw_exception_if_input_file_is_not_a_HDF5_file ){
	BOOST_REQUIRE_THROW(percival_HDF5_loader("./data/NotAHDF5File.txt","dataset",int_buffer_frame),FileIException);	//exception provided by H5Cpp.h
}

BOOST_AUTO_TEST_CASE ( should_throw_exception_if_dataset_does_not_exist ){

	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME,"NonexistentDataSet",int_buffer_frame), FileIException);

}// nonexistent groups are also tested

BOOST_AUTO_TEST_CASE ( should_throw_if_input_file_is_not_2D  ){
	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_three_dimension_DATA_SET_NAME, int_buffer_frame),DataSpaceIException);
}

BOOST_AUTO_TEST_CASE ( should_throw_if_input_file_is_not_little_endian  ){
	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_BE_DATA_SET_NAME, int_buffer_frame),DataTypeIException);
}


BOOST_AUTO_TEST_CASE ( should_generate_output_of_the_same_size_as_input  ){


	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATA_SET_NAME, int_buffer_frame));
	BOOST_REQUIRE_EQUAL(int_buffer_frame.width, TEST_FRAME_WIDTH);
	BOOST_REQUIRE_EQUAL(int_buffer_frame.height, TEST_FRAME_HEIGHT);

}

BOOST_AUTO_TEST_CASE ( should_throw_if_data_type_is_neither_int_nor_32_bit_float ){
	percival_frame<char> char_buffer_frame;
	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME,HDF5_CHAR_DATA_SET_NAME, char_buffer_frame),DataTypeIException);	//TODO: write an exception class for this

}

BOOST_AUTO_TEST_CASE ( should_preserve_data_integrity_int ){
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATA_SET_NAME, int_buffer_frame));
	//four corners
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data), 0);												//upper left
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), 542);	//Lower right
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data + 					 TEST_FRAME_WIDTH - 1), 511);	//upper right
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) ), 31);	//lower left

}

BOOST_AUTO_TEST_CASE ( should_preserve_data_integrity_double ){
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_DOUBLE_DATA_SET_NAME, double_buffer_frame));
	//four corners
		/*		four corners of double_test_data
		 * 		0.0		......	511.0
		 * 		.				.
		 * 		.				.
		 * 		31.0	......	542.0
		 */
	BOOST_REQUIRE_CLOSE_FRACTION(*(double_buffer_frame.data), 0, 0.01);												//upper left
	BOOST_REQUIRE_CLOSE_FRACTION(*(double_buffer_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), 542, 0.01);	//Lower right
	BOOST_REQUIRE_CLOSE_FRACTION(*(double_buffer_frame.data + 					 TEST_FRAME_WIDTH - 1), 511, 0.01);	//upper right
	BOOST_REQUIRE_CLOSE_FRACTION(*(double_buffer_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) ), 31, 0.01);	//lower left

}

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
