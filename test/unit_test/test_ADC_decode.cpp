/*
 * test_ADC_decode.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"

#include <boost/test/included/unit_test.hpp>

const int TEST_FRAME_HEIGHT 	= 	160;		//cannot be 10
const int TEST_FRAME_WIDTH		=	210;		//cannot be 10

percival_global_params global_params;
percival_calib_params calib_params(global_params);

class fixture_frame{
public:
	percival_frame<short int> src_frame;
	percival_frame<float> des_frame;
	percival_frame<float> output_frame;

	fixture_frame(){

		src_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		des_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		output_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);

		BOOST_REQUIRE_NO_THROW(percival_ADC_decode(src_frame, des_frame, calib_params));
	}
};

BOOST_FIXTURE_TEST_SUITE (percival_ADC_decode_test,fixture_frame)

//RANGE TESTS
	BOOST_AUTO_TEST_CASE (should_set_size_of_the_destination_image_to_be_the_same_as_src){
		percival_frame<float> wrong_size_frame;
		wrong_size_frame.set_frame_size(10,10);
		percival_ADC_decode(src_frame,wrong_size_frame, calib_params);
		BOOST_REQUIRE_EQUAL(src_frame.width, wrong_size_frame.width);
		BOOST_REQUIRE_EQUAL(src_frame.height, wrong_size_frame.height);
	}



	BOOST_AUTO_TEST_CASE (output_pixel_should_be_32_bit_float){
		BOOST_CHECK_EQUAL(4,sizeof(*(des_frame.data)));
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









