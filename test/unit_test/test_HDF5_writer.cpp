/*
 * test_HDF5_loader.cpp
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"

#include <boost/test/unit_test.hpp>
#include <string>

#include <iostream>

//defining parameters for test fixtures
const int TEST_FRAME_HEIGHT 	= 	32;
const int TEST_FRAME_WIDTH		=	512;
const char* HDF5_WRITE_FILE_NAME			= 	"./data/test_write_to_HDF5.h5";
const char* HDF5_WRITE_DATA_SET_NAME	= 	"short_int_sample_frame";

class fixture_write_to_HDF5{
	public:
		percival_frame<short int> short_int_src_frame;
		percival_frame<short int> short_int_read_frame;
};

BOOST_FIXTURE_TEST_SUITE(percival_HDF5_loader_test, fixture_write_to_HDF5)

//BOOST_AUTO_TEST_CASE ( should_throw_exception_if_destination_file_cannot_be_opened ){
//
//}

//Assuming that incoming percival_frames are 2D

BOOST_AUTO_TEST_CASE ( should_throw_if_src_is_of_unsupported_type  ){
	percival_frame<long long int> long_long_int_buffer_frame;		//long long int is not supported
	BOOST_REQUIRE_THROW( percival_HDF5_writer(long_long_int_buffer_frame, HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME ), datatype_exception);
}

BOOST_AUTO_TEST_CASE (when_given_a_filename_and_dataset_name_writer_should_create_the_file_and_data_set_with_the_right_names){
	BOOST_REQUIRE_NO_THROW(percival_HDF5_writer( short_int_src_frame, HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME));
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME, short_int_read_frame));
}

BOOST_AUTO_TEST_CASE ( should_preserve_data_integrity_when_writing_int ){
	short int test1 = 0;
	short int test2 = 542;
	short int test3 = 511;
	short int test4 = 31;

	short_int_src_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH );

	*(short_int_src_frame.data)													= test1;
	*(short_int_src_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1)		= test2;
	*(short_int_src_frame.data + 					 TEST_FRAME_WIDTH - 1)		= test3;
	*(short_int_src_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) )	= test4;

	BOOST_REQUIRE_NO_THROW(percival_HDF5_writer( short_int_src_frame, HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME));
	percival_HDF5_loader(HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME, short_int_read_frame);
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME, short_int_read_frame));
	//four corners
	BOOST_REQUIRE_EQUAL(*(short_int_read_frame.data), test1);												//upper left
	BOOST_REQUIRE_EQUAL(*(short_int_read_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), test2);	//Lower right
	BOOST_REQUIRE_EQUAL(*(short_int_read_frame.data + 					 TEST_FRAME_WIDTH - 1), test3);	//upper right
	BOOST_REQUIRE_EQUAL(*(short_int_read_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) ), test4);	//lower left

}
BOOST_AUTO_TEST_CASE ( should_preserve_data_integrity_when_writing_float ){

	percival_frame<float> float_src_frame;
	percival_frame<float> float_read_frame;

	float test1 = 0.0;
	float test2 = 542.0;
	float test3 = 511.0;
	float test4 = 31.0;

	float_src_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH );

	*(float_src_frame.data)													= test1;
	*(float_src_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1)		= test2;
	*(float_src_frame.data + 					 TEST_FRAME_WIDTH - 1)		= test3;
	*(float_src_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) )	= test4;
	BOOST_REQUIRE_NO_THROW(percival_HDF5_writer( float_src_frame, HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME));
	//percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATA_SET_NAME, int_buffer_frame);
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME, float_read_frame));
	//four corners
	BOOST_REQUIRE_EQUAL(*(float_read_frame.data), test1);												//upper left
	BOOST_REQUIRE_EQUAL(*(float_read_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), test2);	//Lower right
	BOOST_REQUIRE_EQUAL(*(float_read_frame.data + 					 TEST_FRAME_WIDTH - 1), test3);	//upper right
	BOOST_REQUIRE_EQUAL(*(float_read_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) ), test4);	//lower left

}
BOOST_AUTO_TEST_SUITE_END()



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
