/*
 * test_percival_frame.cpp
 *
 *  Created on: 3 Aug 2015
 *      Author: pqm78245
 */
#include "percival_processing.h"
#include <boost/test/unit_test.hpp>



class fixture_test_percival_frame{
public:
	percival_frame<int> test_frame;

	fixture_test_percival_frame(){
		test_frame.set_frame_size(1,1);
	}
};

BOOST_FIXTURE_TEST_SUITE (percival_frame_test,fixture_test_percival_frame)

BOOST_AUTO_TEST_CASE(should_throw_if_image_size_is_too_large){
	BOOST_REQUIRE_NO_THROW(test_frame.set_frame_size(327, 100));
	BOOST_REQUIRE_THROW(test_frame.set_frame_size(32768, 1e20), datatype_exception);
}

BOOST_AUTO_TEST_CASE(should_throw_if_image_size_is_negative){
	BOOST_REQUIRE_NO_THROW(test_frame.set_frame_size(327, 100));
	BOOST_REQUIRE_THROW(test_frame.set_frame_size(10, -2), datatype_exception);
}


BOOST_AUTO_TEST_SUITE_END()




