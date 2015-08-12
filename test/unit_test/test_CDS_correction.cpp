/*
 * test_HDF5_loader.cpp
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"
#include <boost/test/unit_test.hpp>
#include <iostream>


const int TEST_FRAME_HEIGHT 	= 	32;
const int TEST_FRAME_WIDTH		=	512;
class fixture_test_percival_CDS_correction{
	public:
		percival_frame<float> sample_frame;
		percival_frame<float> reset_frame;
		percival_frame<float> output_frame;
		fixture_test_percival_CDS_correction(){
			sample_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
			reset_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
			output_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		}
};

BOOST_FIXTURE_TEST_SUITE(percival_CDS_correction_test, fixture_test_percival_CDS_correction)

//subtracting sample from reset

BOOST_AUTO_TEST_CASE ( should_throw_exception_if_sample_and_reset_dimension_mismatch ){

	int reset_frame_width = 3;
	int reset_frame_height = 7;
	reset_frame.set_frame_size(reset_frame_height, reset_frame_width);
	BOOST_REQUIRE_THROW(percival_CDS_correction(sample_frame, reset_frame, output_frame), dataspace_exception);
}

BOOST_AUTO_TEST_CASE ( should_throw_exception_when_output_and_input_dimensions_mismatch ){
	int frame_width = 10;
	int frame_height = 5;
	sample_frame.set_frame_size(frame_height, frame_width);
	reset_frame.set_frame_size(frame_height, frame_width);
	output_frame.set_frame_size(3,2);
	BOOST_REQUIRE_THROW(percival_CDS_correction(sample_frame, reset_frame, output_frame), dataspace_exception);
}

BOOST_AUTO_TEST_CASE ( should_give_sensible_result ){
	sample_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
	reset_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
	output_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);

	float test1S = 3.1;
	float test2S = 4.1;
	float test3S = 5.9;
	float test4S = 2.6;

	float test1R = 3;
	float test2R = 4;
	float test3R = 5;
	float test4R = 2;

	*(sample_frame.data)												= test1S;
	*(sample_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1)		= test2S;
	*(sample_frame.data + 					 TEST_FRAME_WIDTH - 1)		= test3S;
	*(sample_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) )	= test4S;

	*(reset_frame.data)													= test1R;
	*(reset_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1)		= test2R;
	*(reset_frame.data + 					 TEST_FRAME_WIDTH - 1)		= test3R;
	*(reset_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) )	= test4R;

	BOOST_REQUIRE_NO_THROW(percival_CDS_correction(sample_frame, reset_frame, output_frame));
	//four corners
	BOOST_REQUIRE_CLOSE_FRACTION(*(output_frame.data), test1S - test1R, 0.01);												//upper left
	BOOST_REQUIRE_CLOSE_FRACTION(*(output_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), test2S - test2R, 0.01);	//Lower right
	BOOST_REQUIRE_CLOSE_FRACTION(*(output_frame.data + 					  TEST_FRAME_WIDTH - 1), test3S - test3R, 0.01);	//upper right
	BOOST_REQUIRE_CLOSE_FRACTION(*(output_frame.data + TEST_FRAME_WIDTH  * (TEST_FRAME_HEIGHT - 1) ), test4S - test4R, 0.01);	//lower left
}

BOOST_AUTO_TEST_CASE ( should_not_do_subtraction_if_CDS_subtraction_index_includes_the_pixel ){
	sample_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
	reset_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
	output_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);

	float test1S = 3.1;
	float test1R = 3;

	*(sample_frame.data)												= test1S;
	*(reset_frame.data)													= test1R;
	sample_frame.CDS_subtraction_indices.push_back(0);			//include index 0 in the list

	BOOST_REQUIRE_NO_THROW(percival_CDS_correction(sample_frame, reset_frame, output_frame));
	//four corners
	BOOST_REQUIRE_CLOSE_FRACTION(*(output_frame.data), test1S, 0.01);												//upper left
}



BOOST_AUTO_TEST_SUITE_END()
