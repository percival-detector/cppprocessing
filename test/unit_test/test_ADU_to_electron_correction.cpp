/*
 * test_HDF5_loader.cpp
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"
#include "percival_load_calib_params.h"

#include <boost/test/unit_test.hpp>
#include <iostream>


const int TEST_FRAME_HEIGHT 	= 	49;
const int TEST_FRAME_WIDTH		=	55;
class fixture_test_percival_ADU_to_electron_correction{
	public:
		percival_frame_mem<float> tmp1;
		percival_frame_mem<float> tmp2;
		percival_frame_mem<float> tmp3;

		percival_calib_params calib;
		percival_frame<float> src_frame;
		percival_frame<float> output_frame;

		fixture_test_percival_ADU_to_electron_correction():
			tmp1(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
			tmp2(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
			tmp3(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
			src_frame(tmp1),
			output_frame(tmp2)
		{
				calib.ADU_to_electrons_conversion = tmp3;
		}
};

BOOST_FIXTURE_TEST_SUITE(percival_ADU_to_electron_correctionn_test, fixture_test_percival_ADU_to_electron_correction)

BOOST_AUTO_TEST_CASE ( should_throw_exception_when_output_and_input_dimensions_mismatch ){
	int frame_width = 10;
	int frame_height = 5;
	src_frame.set_frame_size(frame_height, frame_width);
	output_frame.set_frame_size(frame_height + 1, frame_width + 1);
	BOOST_REQUIRE_THROW(percival_ADU_to_electron_correction(src_frame, output_frame, calib), dataspace_exception);
}

BOOST_AUTO_TEST_CASE ( should_throw_if_calibration_data_and_input_data_dimension_mismatch ){
	calib.ADU_to_electrons_conversion.set_frame_size(21,34);
	BOOST_REQUIRE_THROW(percival_ADU_to_electron_correction(src_frame, output_frame, calib), dataspace_exception);
}

BOOST_AUTO_TEST_CASE ( should_give_sensible_result ){
	src_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
	calib.ADU_to_electrons_conversion.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
	output_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);

	float test1S = 3.1;
	float test2S = 4.1;
	float test3S = 5.9;
	float test4S = 2.6;

	float test1R = 3.2;
	float test2R = 413;
	float test3R = 5.302;
	float test4R = 2.9;

	*(src_frame.data)												= test1S;
	*(src_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1)	= test2S;
	*(src_frame.data + 					 TEST_FRAME_WIDTH - 1)		= test3S;
	*(src_frame.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) )	= test4S;

	*(calib.ADU_to_electrons_conversion.data)												= test1R;
	*(calib.ADU_to_electrons_conversion.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1)	= test2R;
	*(calib.ADU_to_electrons_conversion.data + 					 TEST_FRAME_WIDTH - 1)		= test3R;
	*(calib.ADU_to_electrons_conversion.data + TEST_FRAME_WIDTH * (TEST_FRAME_HEIGHT - 1) )	= test4R;

	BOOST_REQUIRE_NO_THROW(percival_ADU_to_electron_correction(src_frame, output_frame, calib));
	//four corners
	BOOST_REQUIRE_CLOSE_FRACTION(*(output_frame.data), test1S / test1R, 0.01);												//upper left
	BOOST_REQUIRE_CLOSE_FRACTION(*(output_frame.data + TEST_FRAME_HEIGHT * TEST_FRAME_WIDTH - 1), test2S / test2R, 0.01);	//Lower right
	BOOST_REQUIRE_CLOSE_FRACTION(*(output_frame.data + 					  TEST_FRAME_WIDTH - 1), test3S / test3R, 0.01);	//upper right
	BOOST_REQUIRE_CLOSE_FRACTION(*(output_frame.data + TEST_FRAME_WIDTH  * (TEST_FRAME_HEIGHT - 1) ), test4S / test4R, 0.01);	//lower left
}

BOOST_AUTO_TEST_SUITE_END()
