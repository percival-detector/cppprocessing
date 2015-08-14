/*
 * test_unit_gain_multiplication.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"
#include "percival_load_calib_params.h"

#include <boost/test/unit_test.hpp>
#include<iostream>
const int TEST_FRAME_HEIGHT 	= 	160;		//cannot be 10
const int TEST_FRAME_WIDTH		=	210;		//cannot be 10



class fixture_frame1{
public:
	percival_frame<unsigned short int> src_frame;
	percival_frame<float> des_frame;
	percival_frame<float> output_frame;
	percival_calib_params calib_params;

	fixture_frame1(){
		percival_global_params global_params;

		src_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		des_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		output_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		percival_load_calib_params(calib_params, global_params);
		BOOST_REQUIRE_NO_THROW(percival_ADC_decode(src_frame, des_frame, calib_params));
	}
};

BOOST_FIXTURE_TEST_SUITE (percival_ADC_decode_test1,fixture_frame1)

//RANGE TESTS
	BOOST_AUTO_TEST_CASE (should_throw_exception_when_output_and_input_dimensions_mismatch){
//		percival_frame<float> wrong_size_frame;
//		wrong_size_frame.set_frame_size(10,10);
//		BOOST_REQUIRE_THROW(percival_ADC_decode(src_frame,wrong_size_frame, calib_params), dataspace_exception);
	}
//
//
//
//	BOOST_AUTO_TEST_CASE (output_pixel_should_be_32_bit_float){
//		BOOST_CHECK_EQUAL(4,sizeof(*(des_frame.data)));
//	}


BOOST_AUTO_TEST_SUITE_END()









