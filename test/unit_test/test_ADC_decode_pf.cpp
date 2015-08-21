/*
 * test_ADC_decode_pf.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"
#include "percival_load_calib_params.h"
#include "percival_parallel.h"

#include <boost/test/unit_test.hpp>

const int TEST_FRAME_HEIGHT 	= 	160;		//cannot be 10
const int TEST_FRAME_WIDTH		=	210;		//cannot be 10



class fixture_frame{
public:
	percival_frame<unsigned short int> src_frame;
	percival_frame<float> des_frame;
	percival_frame<float> output_frame;
	percival_calib_params calib_params;

	fixture_frame(){
		src_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		des_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		output_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);

		calib_params.Gain_lookup_table1.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		calib_params.Gain_lookup_table2.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		calib_params.Gain_lookup_table3.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		calib_params.Gain_lookup_table4.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);

		calib_params.Gc.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		calib_params.Gf.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		calib_params.Oc.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		calib_params.Of.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
	}
};

BOOST_FIXTURE_TEST_SUITE (percival_ADC_decode_pf_pf_test,fixture_frame)

//RANGE TESTS
	BOOST_AUTO_TEST_CASE (should_throw_exception_when_output_and_input_dimensions_mismatch){
		percival_frame<float> wrong_size_frame;
		wrong_size_frame.set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_ADC_decode_pf(src_frame,wrong_size_frame, calib_params), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE (should_throw_exception_when_calibration_arrays_and_input_dimensions_mismatch){
		calib_params.Gc.set_frame_size(10, 7);
		BOOST_REQUIRE_THROW(percival_ADC_decode_pf(src_frame,des_frame, calib_params), dataspace_exception);

		calib_params.Gc.set_frame_size(TEST_FRAME_HEIGHT, 10);
		BOOST_REQUIRE_THROW(percival_ADC_decode_pf(src_frame,des_frame, calib_params), dataspace_exception);

		calib_params.Gain_lookup_table1.set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_ADC_decode_pf(src_frame,des_frame, calib_params), dataspace_exception);
	}
//COMPUTATION TESTS

BOOST_AUTO_TEST_SUITE_END()









