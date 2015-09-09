/*
 * test_unit_gain_multiplication.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"

#include <boost/test/unit_test.hpp>
#include<iostream>

const int TEST_FRAME_HEIGHT 	= 	59;		//cannot be 10
const int TEST_FRAME_WIDTH		=	37;		//cannot be 10

class fixture_test_gain_multiplication{
public:
	percival_frame_mem<unsigned short int> tmp1;
	percival_frame_mem<float> tmp2;
	percival_frame_mem<float> tmp3;
	percival_frame_mem<float> tmp4;
	percival_frame_mem<float> tmp5;
	percival_frame_mem<float> tmp6;
	percival_frame_mem<float> tmp7;


	percival_frame<unsigned short int> src_frame;
	percival_frame<float> des_frame;
	percival_frame<float> calibrated;
	percival_calib_params calib_params;

	fixture_test_gain_multiplication():
		tmp1(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp2(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp3(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp4(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp5(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp6(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp7(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		src_frame(tmp1),
		des_frame(tmp2),
		calibrated(tmp3)
	{
		(calib_params.Gain_lookup_table1) = tmp4;
		(calib_params.Gain_lookup_table2) = tmp5;
		(calib_params.Gain_lookup_table3) = tmp6;
		(calib_params.Gain_lookup_table4) = tmp7;
	}
};

BOOST_FIXTURE_TEST_SUITE (percival_unit_gain_multiplication_test,fixture_test_gain_multiplication)

//void percival_unit_gain_multiplication(const percival_frame_mem<float> &, percival_frame_mem<float> &, const percival_calib_params & calib_params);

	BOOST_AUTO_TEST_CASE (should_throw_exception_when_calibration_data_is_of_wrong_dimension){
		(calib_params.Gain_lookup_table1).set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_unit_gain_multiplication(src_frame, calibrated, des_frame, calib_params), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE (should_throw_exception_when_output_and_input_dimensions_mismatch){
		percival_frame<float> wrong_size_frame;
		percival_frame<unsigned short int> wrong_size_frame_int;
		wrong_size_frame.set_frame_size(10,10);
		wrong_size_frame_int.set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_unit_gain_multiplication(src_frame, wrong_size_frame, des_frame, calib_params), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_unit_gain_multiplication(src_frame, calibrated, wrong_size_frame, calib_params), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_unit_gain_multiplication(wrong_size_frame_int, calibrated, des_frame, calib_params), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE (compare_computation_results){
		*calib_params.Gain_lookup_table1.data = 2.3;
		*calib_params.Gain_lookup_table2.data = 3.5;
		*calib_params.Gain_lookup_table3.data = 4.9;
		*calib_params.Gain_lookup_table4.data = 8.0;

		*src_frame.data = 0b010100; //gain bit 00
		*calibrated.data = 4.5;
		BOOST_REQUIRE_NO_THROW(percival_unit_gain_multiplication(src_frame, calibrated, des_frame, calib_params, false));
		BOOST_REQUIRE_CLOSE(*des_frame.data, 4.5 * 2.3, 0.01);

		*src_frame.data = 0b010101; //gain bit 01
		*calibrated.data = 4.5;
		BOOST_REQUIRE_NO_THROW(percival_unit_gain_multiplication(src_frame, calibrated, des_frame, calib_params, false));
		BOOST_REQUIRE_CLOSE(*des_frame.data, 4.5 * 3.5, 0.01);

		*src_frame.data = 0b010110; //gain bit 10
		*calibrated.data = 4.5;
		BOOST_REQUIRE_NO_THROW(percival_unit_gain_multiplication(src_frame, calibrated, des_frame, calib_params, false));
		BOOST_REQUIRE_CLOSE(*des_frame.data, 4.5 * 4.9, 0.01);

		*src_frame.data = 0b010111; //gain bit 11
		*calibrated.data = 4.5;
		BOOST_REQUIRE_NO_THROW(percival_unit_gain_multiplication(src_frame, calibrated, des_frame, calib_params, false));
		BOOST_REQUIRE_CLOSE(*des_frame.data, 4.5 * 8.0, 0.01);
	}


BOOST_AUTO_TEST_SUITE_END()









