/*
 * test_unit_ADC_decode_pf.cpp
 *
 *  Created on: 18 Aug 2015
 *      Author: pqm78245
 */

#include "percival_parallel.h"

#include <boost/test/unit_test.hpp>

const int TEST_FRAME_HEIGHT 	= 	3;		//cannot be 10
const int TEST_FRAME_WIDTH		=	5;		//cannot be 10

class fixture_test_unit_ADC_decode_frame_pf{
public:
	percival_frame_mem<short unsigned int> tmp1;
	percival_frame_mem<short unsigned int> tmp2;
	percival_frame_mem<short unsigned int> tmp3;
	percival_frame_mem<short unsigned int> tmp4;

	percival_frame<short unsigned int> src_frame;
	percival_frame<short unsigned int> coarse_frame;
	percival_frame<short unsigned int> fine_frame;
	percival_frame<short unsigned int> gain_frame;

	fixture_test_unit_ADC_decode_frame_pf():
		tmp1(TEST_FRAME_HEIGHT,TEST_FRAME_WIDTH),
		tmp2(TEST_FRAME_HEIGHT,TEST_FRAME_WIDTH),
		tmp3(TEST_FRAME_HEIGHT,TEST_FRAME_WIDTH),
		tmp4(TEST_FRAME_HEIGHT,TEST_FRAME_WIDTH),
		src_frame(tmp1),
		coarse_frame(tmp2),
		fine_frame(tmp3),
		gain_frame(tmp4)
	{}
};

//percival_unit_ADC_decode_pf(const percival_frame_mem<unsigned short int> &, percival_frame_mem<unsigned short int> & Coarse, percival_frame_mem<unsigned short int> & Fine, percival_frame_mem<short int> & Gain);

BOOST_FIXTURE_TEST_SUITE (percival_unit_ADC_decode_pf_test,fixture_test_unit_ADC_decode_frame_pf)
	BOOST_AUTO_TEST_CASE (should_throw_exception_when_output_and_input_dimensions_mismatch){
		percival_frame<short unsigned int> wrong_size_frame;
		wrong_size_frame.set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_unit_ADC_decode_pf(src_frame, wrong_size_frame, fine_frame, gain_frame), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_unit_ADC_decode_pf(src_frame, coarse_frame, wrong_size_frame, gain_frame), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_unit_ADC_decode_pf(src_frame, coarse_frame, fine_frame, wrong_size_frame), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE (compare_with_correct_output){
		*(src_frame.data) = 0b0111111111111111; //0b011111 11111111 11
		BOOST_REQUIRE_NO_THROW(percival_unit_ADC_decode_pf(src_frame, coarse_frame, fine_frame, gain_frame));
		BOOST_REQUIRE_EQUAL(*coarse_frame.data, 0b11111);
		BOOST_REQUIRE_EQUAL(*fine_frame.data, 0b11111111);
		BOOST_REQUIRE_EQUAL(*gain_frame.data, 0b11);

		unsigned int end = TEST_FRAME_HEIGHT*TEST_FRAME_WIDTH - 1;
		*(src_frame.data + end) = 0b0111111111111111; //0b011111 11111111 11
		BOOST_REQUIRE_NO_THROW(percival_unit_ADC_decode_pf(src_frame, coarse_frame, fine_frame, gain_frame));
		BOOST_REQUIRE_EQUAL(*(coarse_frame.data + end), 0b11111);
		BOOST_REQUIRE_EQUAL(*(fine_frame.data + end), 0b11111111);
		BOOST_REQUIRE_EQUAL(*(gain_frame.data + end), 0b11);
	}

	BOOST_AUTO_TEST_CASE (should_throw_if_any_two_of_the_output_buffers_are_equal){
		BOOST_REQUIRE_THROW(percival_unit_ADC_decode_pf(src_frame, coarse_frame, coarse_frame, gain_frame), datatype_exception);
		BOOST_REQUIRE_THROW(percival_unit_ADC_decode_pf(src_frame, coarse_frame, coarse_frame, coarse_frame), datatype_exception);
		BOOST_REQUIRE_THROW(percival_unit_ADC_decode_pf(src_frame, fine_frame, coarse_frame, coarse_frame), datatype_exception);
		BOOST_REQUIRE_THROW(percival_unit_ADC_decode_pf(src_frame, coarse_frame, fine_frame, coarse_frame), datatype_exception);
	}	//note that the user can write to the input file. This is useful when an in place computation is needed. However, it will be the user's reponsibility to check data integrity.
BOOST_AUTO_TEST_SUITE_END()



