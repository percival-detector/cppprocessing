/*
 * test_ADC_decode.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"
#include "percival_load_calib_params.h"

#include <boost/test/included/unit_test.hpp>

const int TEST_FRAME_HEIGHT 	= 	160;		//cannot be 10
const int TEST_FRAME_WIDTH		=	210;		//cannot be 10



class fixture_frame{
public:
	percival_frame_mem<unsigned short int> tmp1;
	percival_frame_mem<float> tmp2;
	percival_frame_mem<float> tmp3;

	percival_frame_mem<float> tmp4;
	percival_frame_mem<float> tmp5;
	percival_frame_mem<float> tmp6;
	percival_frame_mem<float> tmp7;
	percival_frame_mem<float> tmp8;
	percival_frame_mem<float> tmp9;
	percival_frame_mem<float> tmp10;
	percival_frame_mem<float> tmp11;

	percival_calib_params calib_params;
	percival_frame<unsigned short int> src_frame;
	percival_frame<float> des_frame;
	percival_frame<float> output_frame;

	fixture_frame():
		tmp1(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp2(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp3(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp4(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp5(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp6(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp7(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp8(TEST_FRAME_HEIGHT, 7),
		tmp9(TEST_FRAME_HEIGHT, 7),
		tmp10(TEST_FRAME_HEIGHT, 7),
		tmp11(TEST_FRAME_HEIGHT, 7),
		src_frame(tmp1),
		des_frame(tmp2),
		output_frame(tmp3)

	{
		calib_params.Gain_lookup_table1 = tmp4;
		calib_params.Gain_lookup_table2 = tmp5;
		calib_params.Gain_lookup_table3 = tmp6;
		calib_params.Gain_lookup_table4 = tmp7;
		calib_params.Gc = tmp8;
		calib_params.Gf = tmp9;
		calib_params.Oc = tmp10;
		calib_params.Of = tmp11;
	}
};

BOOST_FIXTURE_TEST_SUITE (percival_ADC_decode_test,fixture_frame)

//RANGE TESTS
	BOOST_AUTO_TEST_CASE (should_throw_exception_when_output_and_input_dimensions_mismatch){
		percival_frame<float> wrong_size_frame;
		wrong_size_frame.set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_ADC_decode(src_frame,wrong_size_frame, calib_params), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE (should_throw_exception_when_calibration_arrays_and_input_dimensions_mismatch){
		calib_params.Gc.set_frame_size(10, 7);
		BOOST_REQUIRE_THROW(percival_ADC_decode(src_frame,des_frame, calib_params), dataspace_exception);

		calib_params.Gc.set_frame_size(TEST_FRAME_HEIGHT, 10);
		BOOST_REQUIRE_THROW(percival_ADC_decode(src_frame,des_frame, calib_params), dataspace_exception);

		calib_params.Gain_lookup_table1.set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_ADC_decode(src_frame,des_frame, calib_params), dataspace_exception);
	}

//Data check
//	BOOST_AUTO_TEST_CASE(two_input_data_point_with_the_last_fifteen_bits_equal_should_return_same_result){
//		*(src_frame.data) = 0b1111111111101111;		//overflow if all digits are 1's
//		BOOST_REQUIRE_NO_THROW(percival_ADC_decode(src_frame, des_frame));
//		std::cout << *(src_frame.data) << std::endl;
//		float result = *(des_frame.data);
//		*(src_frame.data) = 0b0111111111101111;
//		BOOST_REQUIRE_NO_THROW(percival_ADC_decode(src_frame, des_frame));
//		BOOST_REQUIRE_CLOSE(result, *(des_frame.data), 0.01);
//
//	}

//	BOOST_AUTO_TEST_CASE (output_pixel_should_be_non_negative){
//		std::cout << 5 << std::endl;
//		for(int i=0;i<src_frame.height * des_frame.width;i++)
//			BOOST_CHECK_GE(*(des_frame.data+i),0);
//	}

//
//
////COMPUTATION TESTS
//	BOOST_AUTO_TEST_CASE (output_should_only_depend_on_input_irrespective_of_pixel_location){
//		/*
//		 * use manually calculated data to verify
//		 * 0 		-> 	0
//		 * 32767 	->	1137
//		 *
//		 * for corners at indices 0, width -1, (width * (height - 1)), (width * height - 1)
//		 * and centre at (width * height)/2
//		 *
//		*/
//
//		const int given_input = 32767;
//		const int expected_output = 1137;
//
//		*(src_frame.data + 										0) 		= given_input;
//		*(src_frame.data + src_frame.width 						- 1) 	= given_input;
//		*(src_frame.data + src_frame.width * (src_frame.height - 1)) 	= given_input;
//		*(src_frame.data + src_frame.width * (src_frame.height) -1) 	= given_input;
//		*(src_frame.data + src_frame.width * (src_frame.height) /2) 	= given_input;		//central pixel
//
//		BOOST_REQUIRE_NO_THROW( percival_ADC_decode(src_frame, des_frame) );
//
//		BOOST_REQUIRE_EQUAL(expected_output, *(des_frame.data + 										0));
//		BOOST_REQUIRE_EQUAL(expected_output, *(des_frame.data + des_frame.width 						-1));
//		BOOST_REQUIRE_EQUAL(expected_output, *(des_frame.data + des_frame.width * (des_frame.height - 1)));
//		BOOST_REQUIRE_EQUAL(expected_output, *(des_frame.data + des_frame.width * (des_frame.height) -1));
//		BOOST_REQUIRE_EQUAL(expected_output, *(des_frame.data + des_frame.width * (des_frame.height) /2));
//	}
//
//	BOOST_AUTO_TEST_CASE ( test_algorithm_correctness_using_manually_calculated_data ){
//		/*
//		 * the test data used are manually computed using
//		 * output = gain * ( coarseBits * 4 + fineBits * 1 )
//		 *
//		 * Considering boundary cases and a random number in between
//		 *
//		 *  input		binary					coarse		fine	gain	output
//		 *  0			0000,0000,0000,0000		0			0		0		0
//		 *  21853		0101,0101,0101,1101		21			87		1		171
//		 *  32767		0111,1111,1111,1111		31			255		3		1137
//		 */
//		*(src_frame.data 	+ 0) = 0;
//		*(src_frame.data	+ 1) = 32767;
//		*(src_frame.data 	+ 2) = 21853;
//
//		BOOST_REQUIRE_NO_THROW(percival_ADC_decode(src_frame, des_frame));
//
//		BOOST_REQUIRE_EQUAL(	gain_coarse_fine_to_int_output(0,0,0), 		*(des_frame.data + 0)	);
//		BOOST_REQUIRE_EQUAL(	gain_coarse_fine_to_int_output(31,255,3), 	*(des_frame.data + 1)	);
//		BOOST_REQUIRE_EQUAL(	gain_coarse_fine_to_int_output(21,87,1), 	*(des_frame.data + 2)	);
//	}
//
//	BOOST_AUTO_TEST_CASE(should_comply_with_the_table_expected_output_from_file){
//		for(int i = 0; i < TEST_FRAME_WIDTH * TEST_FRAME_HEIGHT; i++)
//			BOOST_REQUIRE_EQUAL(*(src_frame.data + i), *(output_frame.data + i));
//	}

BOOST_AUTO_TEST_SUITE_END()









