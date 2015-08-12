/*
 * test_HDF5_loader.cpp
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"
#include "percival_HDF5_loader.h"
#include "percival_HDF5_writer.h"

#include <boost/test/unit_test.hpp>
#include <string>

#include <iostream>

//defining parameters for test fixtures
const int TEST_FRAME_HEIGHT 	= 	32;
const int TEST_FRAME_WIDTH		=	512;
const char* HDF5_WRITE_FILE_NAME			= 	"./data/test_write_to_HDF5.h5";
const char* HDF5_WRITE_DATA_SET_NAME	= 	"short_int_sample_frame";
const char* HDF5_WRITE_INT_DATA_SET_NAME = "int_sample_frame";
const char* HDF5_WRITE_FLOAT_DATA_SET_NAME= "float_sample_frame";

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
//
BOOST_AUTO_TEST_CASE ( should_throw_if_src_is_of_unsupported_type  ){
	percival_frame<long long int> long_long_int_buffer_frame;		//long long int is not supported
	BOOST_REQUIRE_THROW( percival_HDF5_writer(long_long_int_buffer_frame, HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME ), datatype_exception);
}

BOOST_AUTO_TEST_CASE (when_given_a_filename_and_dataset_name_writer_should_create_the_file_and_data_set_with_the_right_names){
	BOOST_REQUIRE_NO_THROW(percival_HDF5_writer( short_int_src_frame, HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME));
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME, short_int_read_frame));
}

BOOST_AUTO_TEST_CASE ( should_create_additional_data_set_if_the_fourth_argument_is_set_to_FALSE){
	percival_HDF5_writer( short_int_src_frame, HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME);
	percival_HDF5_writer( short_int_src_frame, HDF5_WRITE_FILE_NAME, "another_data_set", false);
	percival_HDF5_loader(HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME, short_int_read_frame);
	percival_HDF5_loader(HDF5_WRITE_FILE_NAME, "another_data_set", short_int_read_frame);

	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_WRITE_FILE_NAME, HDF5_WRITE_DATA_SET_NAME, short_int_read_frame));
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_WRITE_FILE_NAME, "another_data_set", short_int_read_frame));
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

	BOOST_REQUIRE_NO_THROW(percival_HDF5_writer( short_int_src_frame, HDF5_WRITE_FILE_NAME, HDF5_WRITE_INT_DATA_SET_NAME));
	percival_HDF5_loader(HDF5_WRITE_FILE_NAME, HDF5_WRITE_INT_DATA_SET_NAME, short_int_read_frame);
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_WRITE_FILE_NAME, HDF5_WRITE_INT_DATA_SET_NAME, short_int_read_frame));
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
	BOOST_REQUIRE_NO_THROW(percival_HDF5_writer( float_src_frame, HDF5_WRITE_FILE_NAME, HDF5_WRITE_FLOAT_DATA_SET_NAME));
	//percival_HDF5_loader(HDF5_FILE_NAME, HDF5_INT_DATA_SET_NAME, int_buffer_frame);
	BOOST_REQUIRE_NO_THROW(percival_HDF5_loader(HDF5_WRITE_FILE_NAME, HDF5_WRITE_FLOAT_DATA_SET_NAME, float_read_frame));
	//four corners
	BOOST_REQUIRE_EQUAL(*(float_read_frame.data), test1);												//upper left
	BOOST_REQUIRE_EQUAL(*(float_read_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), test2);	//Lower right
	BOOST_REQUIRE_EQUAL(*(float_read_frame.data + 					 TEST_FRAME_WIDTH - 1), test3);	//upper right
	BOOST_REQUIRE_EQUAL(*(float_read_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) ), test4);	//lower left

}
BOOST_AUTO_TEST_SUITE_END()
