/*
 * test_unit_ADC_calibration_pf.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_parallel.h"

#include <boost/test/unit_test.hpp>

const int TEST_FRAME_HEIGHT 	= 	49;		//cannot be 10
const int TEST_FRAME_WIDTH		=	21;		//cannot be 10

class fixture_unit_ADC_calibration_pf_test{
public:

	percival_frame_mem<unsigned short int> tmp1;
	percival_frame_mem<unsigned short int> tmp2;
	percival_frame_mem<float> tmp3;

	percival_frame_mem<float> tmp4;
	percival_frame_mem<float> tmp5;
	percival_frame_mem<float> tmp6;
	percival_frame_mem<float> tmp7;

	percival_frame<short unsigned int> coarse_frame;
	percival_frame<short unsigned int> fine_frame;
	percival_frame<float> des_frame;
	percival_calib_params calib_params;

	fixture_unit_ADC_calibration_pf_test():
		tmp1(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp2(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp3(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp4(TEST_FRAME_HEIGHT, 7),
		tmp5(TEST_FRAME_HEIGHT, 7),
		tmp6(TEST_FRAME_HEIGHT, 7),
		tmp7(TEST_FRAME_HEIGHT, 7),
		coarse_frame(tmp1),
		fine_frame(tmp2),
		des_frame(tmp3)
	{
		calib_params.Gc = tmp4;
		calib_params.Gf = tmp5;
		calib_params.Oc = tmp6;
		calib_params.Of = tmp7;
	}
};

BOOST_FIXTURE_TEST_SUITE (percival_unit_ADC_calibration_pf_test,fixture_unit_ADC_calibration_pf_test)

//void percival_unit_ADC_calibration_pf(const percival_frame_mem<unsigned short int> & Coarse,const  percival_frame_mem<unsigned short int> & Fine,const percival_frame_mem<unsigned short int> & Gain, percival_frame_mem<float>& output, const percival_calib_params &);

//RANGE TESTS
	BOOST_AUTO_TEST_CASE (should_throw_exception_when_calibration_data_is_of_wrong_dimension){
		(calib_params.Gc).set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_unit_ADC_calibration_pf(coarse_frame, fine_frame, des_frame, calib_params), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE (should_throw_exception_when_output_and_input_dimensions_mismatch){
		percival_frame<unsigned short int> wrong_size_frame;
		percival_frame<float> wrong_size_frame_float;
		wrong_size_frame.set_frame_size(10,10);

		BOOST_REQUIRE_THROW(percival_unit_ADC_calibration_pf(wrong_size_frame, fine_frame, des_frame, calib_params), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_unit_ADC_calibration_pf(coarse_frame, wrong_size_frame, des_frame, calib_params), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_unit_ADC_calibration_pf(coarse_frame, fine_frame, wrong_size_frame_float, calib_params), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE ( should_use_correct_location_in_calibration_array ){
		*(coarse_frame.data) = 25;
		*(fine_frame.data) = 156;
		*(coarse_frame.data + 7) = 25;
		*(fine_frame.data + 7) = 156;
		*(coarse_frame.data + 14) = 25;
		*(fine_frame.data + 14) = 156;

		*(calib_params.Gc.data) = 1.0;
		*(calib_params.Oc.data) = 2.0;
		*(calib_params.Gf.data) = 3.0;
		*(calib_params.Of.data) = 4.0;

		BOOST_REQUIRE_NO_THROW(percival_unit_ADC_calibration_pf(coarse_frame, fine_frame, des_frame, calib_params));
		BOOST_REQUIRE_CLOSE(*(des_frame.data), *(des_frame.data + 7), 0.01);
		BOOST_REQUIRE_CLOSE(*(des_frame.data), *(des_frame.data + 14), 0.01);
	}



BOOST_AUTO_TEST_SUITE_END()









