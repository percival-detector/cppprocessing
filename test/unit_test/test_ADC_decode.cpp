/*
 * test_ADC_decode.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_ADC_decode.h"
#include "percival_frame.h"

#define BOOST_TEST_MODULE stringtest
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include<iostream>

const int TEST_FRAME_HEIGHT 	= 	32;
const int TEST_FRAME_WIDTH		=	1024;

class fixture_frame{
public:
	percival_frame<int> src_frame;
	percival_frame<int> des_frame;
	percival_frame<int> output_frame;

	fixture_frame(){
		src_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		des_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);
		output_frame.set_frame_size(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH);

		std::ifstream expected_result;
		BOOST_REQUIRE_NO_THROW(expected_result.open ("../data/ADC_decode_output"));

		/*
		 * files to be read have the format
		 *
		 * input'\t'output'n'
		 *
		 * all numbers assumed to be integral for atoi() to work
		 */

		char readValue[255];
		int input, output;
		int i = 0;
		while(!expected_result.eof() && i < TEST_FRAME_WIDTH * TEST_FRAME_HEIGHT){
			expected_result.getline(readValue, 255, '\t');						//reading input data
			input = atoi(readValue);
			expected_result.getline(readValue, 255, '\n');						//reading output data
			output = atoi(readValue);

			src_frame.data[i] = input;
			des_frame.data[i] = output;
			i++;
		}
		BOOST_REQUIRE_NO_THROW(percival_ADC_decode(src_frame, des_frame));
		expected_result.close();
	}
};

int gain_coarse_fine_to_int_output(int coarse, int fine, int gain){
	int output = (gain) * (coarse * 4 + fine * 1);
	return output;
}

BOOST_FIXTURE_TEST_SUITE (percival_ADC_decode_test,fixture_frame)

//RANGE TESTS
	BOOST_AUTO_TEST_CASE (should_throw_exception_if_width_and_height_of_output_are_not_the_same_as_input	){
		percival_frame<int> wrong_size_frame;
		wrong_size_frame.set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_ADC_decode(src_frame,wrong_size_frame),dimension_mismatch_exception);
	}

	BOOST_AUTO_TEST_CASE (output_pixel_should_be_of_same_type_as_input){
		BOOST_CHECK_EQUAL(sizeof(*(src_frame.data)),sizeof(*(des_frame.data)));
	}

	BOOST_AUTO_TEST_CASE (output_pixel_should_be_non_negative){
		for(int i=0;i<src_frame.height * des_frame.width;i++)
			BOOST_CHECK_GE(*(des_frame.data+i),0);
	}

//COMPUTATION TESTS
	BOOST_AUTO_TEST_CASE (output_should_only_depend_on_input_irrespective_of_pixel_location){
		/*
		 * use manually calculated data to verify
		 * 0 		-> 	0
		 * 32767 	->	1137
		 *
		 * for corners at indices 0, width -1, (width * (height - 1)), (width * height - 1)
		 * and centre at (width * height)/2
		 *
		*/

		const int given_input = 32767;
		const int expected_output = 1137;

		*(src_frame.data + 										0) 		= given_input;
		*(src_frame.data + src_frame.width 						- 1) 	= given_input;
		*(src_frame.data + src_frame.width * (src_frame.height - 1)) 	= given_input;
		*(src_frame.data + src_frame.width * (src_frame.height) -1) 	= given_input;
		*(src_frame.data + src_frame.width * (src_frame.height) /2) 	= given_input;		//central pixel

		BOOST_REQUIRE_NO_THROW( percival_ADC_decode(src_frame, des_frame) );

		BOOST_REQUIRE_EQUAL(expected_output, *(des_frame.data + 										0));
		BOOST_REQUIRE_EQUAL(expected_output, *(des_frame.data + des_frame.width 						-1));
		BOOST_REQUIRE_EQUAL(expected_output, *(des_frame.data + des_frame.width * (des_frame.height - 1)));
		BOOST_REQUIRE_EQUAL(expected_output, *(des_frame.data + des_frame.width * (des_frame.height) -1));
		BOOST_REQUIRE_EQUAL(expected_output, *(des_frame.data + des_frame.width * (des_frame.height) /2));
	}

	BOOST_AUTO_TEST_CASE ( test_algorithm_correctness_using_manually_calculated_data ){
		/*
		 * the test data used are manually computed using
		 * output = gain * ( coarseBits * 4 + fineBits * 1 )
		 *
		 * Considering boundary cases and a random number in between
		 *
		 *  input		binary					coarse		fine	gain	output
		 *  0			0000,0000,0000,0000		0			0		0		0
		 *  21853		0101,0101,0101,1101		21			87		1		171
		 *  32767		0111,1111,1111,1111		31			255		3		1137
		 */
		*(src_frame.data 	+ 0) = 0;
		*(src_frame.data	+ 1) = 32767;
		*(src_frame.data 	+ 2) = 21853;

		BOOST_REQUIRE_NO_THROW(percival_ADC_decode(src_frame, des_frame));

		BOOST_REQUIRE_EQUAL(	gain_coarse_fine_to_int_output(0,0,0), 		*(des_frame.data + 0)	);
		BOOST_REQUIRE_EQUAL(	gain_coarse_fine_to_int_output(31,255,3), 	*(des_frame.data + 1)	);
		BOOST_REQUIRE_EQUAL(	gain_coarse_fine_to_int_output(21,87,1), 	*(des_frame.data + 2)	);
	}

	BOOST_AUTO_TEST_CASE(should_comply_with_the_table_expected_output_from_file){
		for(int i = 0; i < TEST_FRAME_WIDTH * TEST_FRAME_HEIGHT; i++)
			BOOST_REQUIRE_EQUAL(*(src_frame.data + i), *(output_frame.data + i));
	}

BOOST_AUTO_TEST_SUITE_END()

//TODO:checking on input data types if using template functions








