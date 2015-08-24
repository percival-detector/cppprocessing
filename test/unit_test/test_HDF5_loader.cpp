/*
 * test_HDF5_loader.cpp
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"

#include "percival_HDF5_loader.h"

#include <boost/test/unit_test.hpp>
#include <string>

#include <iostream>

//defining parameters for test fixtures
const int TEST_FRAME_HEIGHT 	= 	160;
const int TEST_FRAME_WIDTH		=	210;
const int RANK 					= 	2;
const char* HDF5_FILE_NAME			= 	"./data/test_HDF5.h5";
const char* HDF5_INT_DATA_SET_NAME	= 	"int_sample_frame";
const char* HDF5_DOUBLE_DATA_SET_NAME	= 	"double_sample_frame";
const char* HDF5_CHAR_DATA_SET_NAME 	= 	"char_frame";
const char* HDF5_three_dimension_DATA_SET_NAME 	= 	"three_dimension_data";
const char* HDF5_BE_DATA_SET_NAME 	= 	"BE_data";

class fixture_HDF5{
	public:
		percival_frame_mem<int> int_buffer_frame;
		percival_frame_mem<double> double_buffer_frame;
};


BOOST_FIXTURE_TEST_SUITE(percival_HDF5_loader_test, fixture_HDF5)
//TODO: write exception classes for the loader
BOOST_AUTO_TEST_CASE ( should_throw_exception_if_input_path_does_not_exist ){
	BOOST_REQUIRE_THROW(percival_HDF5_loader("./data/-NonexistentPath.h5","dataset",int_buffer_frame),file_exception);
}

BOOST_AUTO_TEST_CASE ( should_throw_exception_if_input_file_is_not_a_HDF5_file ){
	BOOST_REQUIRE_THROW(percival_HDF5_loader("./data/NotAHDF5File.txt","dataset",int_buffer_frame),file_exception);
}

BOOST_AUTO_TEST_CASE ( should_throw_exception_if_dataset_does_not_exist ){
	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME,"NonexistentDataSet",int_buffer_frame), file_exception);

}// nonexistent groups are also tested

BOOST_AUTO_TEST_CASE ( should_throw_if_input_file_is_not_2D  ){
	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_three_dimension_DATA_SET_NAME, int_buffer_frame),dataspace_exception);
}
//
//BOOST_AUTO_TEST_CASE ( should_throw_if_input_file_is_not_little_endian  ){
//	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_BE_DATA_SET_NAME, int_buffer_frame),datatype_exception);
//}

BOOST_AUTO_TEST_CASE ( should_throw_if_buffer_is_of_wrong_type  ){
	percival_frame_mem<float> float_buffer_frame;
	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATA_SET_NAME, float_buffer_frame), datatype_exception);
}

BOOST_AUTO_TEST_CASE ( should_generate_output_of_the_same_size_as_input  ){
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATA_SET_NAME, int_buffer_frame));
	BOOST_REQUIRE_EQUAL(int_buffer_frame.width, TEST_FRAME_WIDTH);
	BOOST_REQUIRE_EQUAL(int_buffer_frame.height, TEST_FRAME_HEIGHT);

}

BOOST_AUTO_TEST_CASE ( should_throw_if_data_type_is_neither_int_nor_32_bit_float ){
	percival_frame_mem<char> char_buffer_frame;
	BOOST_REQUIRE_THROW(percival_HDF5_loader(HDF5_FILE_NAME,HDF5_CHAR_DATA_SET_NAME, char_buffer_frame),datatype_exception);	//TODO: write an exception class for this

}

BOOST_AUTO_TEST_CASE ( should_preserve_data_integrity_int ){
	//percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATA_SET_NAME, int_buffer_frame);
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATA_SET_NAME, int_buffer_frame));
	//four corners
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data), 0);												//upper left
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), 368);	//Lower right
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data + 					 TEST_FRAME_WIDTH - 1), 209);	//upper right
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) ), 159);	//lower left

}

BOOST_AUTO_TEST_CASE ( should_preserve_data_integrity_double ){
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_DOUBLE_DATA_SET_NAME, double_buffer_frame));
	//four corners
		/*		four corners of double_test_data
		 * 		0	......	209
		 * 		.			.
		 * 		.			.
		 * 		159	......	368
		 */
	BOOST_REQUIRE_CLOSE_FRACTION(*(double_buffer_frame.data), 0, 0.01);												//upper left
	BOOST_REQUIRE_CLOSE_FRACTION(*(double_buffer_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), 368, 0.01);	//Lower right
	BOOST_REQUIRE_CLOSE_FRACTION(*(double_buffer_frame.data + 					 TEST_FRAME_WIDTH - 1), 209, 0.01);	//upper right
	BOOST_REQUIRE_CLOSE_FRACTION(*(double_buffer_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) ), 159, 0.01);	//lower left

}

BOOST_AUTO_TEST_CASE ( should_preserve_data_integrity_after_transposition_option_enabled_int ){
	//percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATA_SET_NAME, int_buffer_frame);
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATA_SET_NAME, int_buffer_frame, true));
	//four corners
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data), 0);												//upper left
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), 368);	//Lower right
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data + 					 TEST_FRAME_HEIGHT - 1), 159);	//upper right
	BOOST_REQUIRE_EQUAL(*(int_buffer_frame.data + TEST_FRAME_HEIGHT * (TEST_FRAME_WIDTH - 1) ), 209);	//lower left

}

BOOST_AUTO_TEST_SUITE_END()

//TODO:should avoid opened files

//
//Unit_test guidelines:
//	1. should take in
//		- path to HDF5 files
//			throw an exception if path does not exist, use default H5Cpp exceptions
//		- pointer to a plain percival_frame_mem buffer
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
//	5. should automatically check HDF5 image size and set input percival_frame_mem to be of the same size
//		check input and output size
//
