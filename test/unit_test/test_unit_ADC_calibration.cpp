/*
 * test_unit_ADC_calibration.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"

#include <boost/test/unit_test.hpp>

const int TEST_FRAME_HEIGHT 	= 	49;		//cannot be 10
const int TEST_FRAME_WIDTH		=	21;		//cannot be 10

class fixture_unit_ADC_calibration_test{
public:
	percival_frame<short unsigned int> coarse_frame;
	percival_frame<short unsigned int> fine_frame;
	percival_frame<short unsigned int> gain_frame;

	percival_frame<float> des_frame;
	percival_calib_params calib_params;

	fixture_unit_ADC_calibration_test(){
		(calib_params.Gc).set_frame_size(TEST_FRAME_HEIGHT, 7);
		(calib_params.Gf).set_frame_size(TEST_FRAME_HEIGHT, 7);
		(calib_params.Oc).set_frame_size(TEST_FRAME_HEIGHT, 7);
		(calib_params.Of).set_frame_size(TEST_FRAME_HEIGHT, 7);

		des_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);

		fine_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		coarse_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		gain_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
	}
};

BOOST_FIXTURE_TEST_SUITE (percival_unit_ADC_calibration_test,fixture_unit_ADC_calibration_test)

//void percival_unit_ADC_calibration(const percival_frame<unsigned short int> & Coarse,const  percival_frame<unsigned short int> & Fine,const percival_frame<unsigned short int> & Gain, percival_frame<float>& output, const percival_calib_params &);

//RANGE TESTS
	BOOST_AUTO_TEST_CASE (should_throw_exception_when_calibration_data_is_of_wrong_dimension){
		(calib_params.Gc).set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_unit_ADC_calibration(coarse_frame, fine_frame, gain_frame, des_frame, calib_params), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE (should_throw_exception_when_output_and_input_dimensions_mismatch){
		percival_frame<unsigned short int> wrong_size_frame;
		percival_frame<float> wrong_size_frame_float;
		wrong_size_frame.set_frame_size(10,10);

		BOOST_REQUIRE_THROW(percival_unit_ADC_calibration(wrong_size_frame, fine_frame, gain_frame, des_frame, calib_params), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_unit_ADC_calibration(coarse_frame, wrong_size_frame, gain_frame, des_frame, calib_params), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_unit_ADC_calibration(coarse_frame, fine_frame, wrong_size_frame, des_frame, calib_params), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_unit_ADC_calibration(coarse_frame, fine_frame, gain_frame, wrong_size_frame_float, calib_params), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE ( should_use_correct_location_in_calibration_array ){
		*(coarse_frame.data) = 25;
		*(fine_frame.data) = 156;
		*(gain_frame.data) = 2;
		*(coarse_frame.data + 7) = 25;
		*(fine_frame.data + 7) = 156;
		*(gain_frame.data + 7) = 2;
		*(coarse_frame.data + 14) = 25;
		*(fine_frame.data + 14) = 156;
		*(gain_frame.data + 14) = 2;

		*(calib_params.Gc.data) = 1.0;
		*(calib_params.Oc.data) = 2.0;
		*(calib_params.Gf.data) = 3.0;
		*(calib_params.Of.data) = 4.0;

		BOOST_REQUIRE_NO_THROW(percival_unit_ADC_calibration(coarse_frame, fine_frame, gain_frame, des_frame, calib_params));
		BOOST_REQUIRE_CLOSE(*(des_frame.data), *(des_frame.data + 7), 0.01);
		BOOST_REQUIRE_CLOSE(*(des_frame.data), *(des_frame.data + 14), 0.01);
	}



BOOST_AUTO_TEST_SUITE_END()









