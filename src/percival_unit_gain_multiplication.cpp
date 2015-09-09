/*
 * percival_unit_gain_multiplication.cpp
 *
 *  Created on: 14 Aug 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"
#include "percival_data_validity_checks.h"
#include "percival_functors.h"

void percival_unit_gain_multiplication(
		const percival_frame<unsigned short int> & src_frame,
		const percival_frame<float> & calibrated,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		bool check_dimensions)
{
	if(check_dimensions){
		percival_input_output_dimension_check(src_frame, calibrated);
		percival_input_output_dimension_check(src_frame, output);
		percival_input_calib_dimension_check(src_frame, calib_params);
		percival_input_calib_NULL_check(calib_params);
		percival_null_check(src_frame);
		percival_null_check(calibrated);
		percival_null_check(output);

	}

	unsigned int NoOfPixels = src_frame.width * src_frame.height;

	percival_unit_gain_multiplication_p<percival_range_iterator_mock_p> unit_gain_multiplication(src_frame,calibrated,output,calib_params);
	unit_gain_multiplication(percival_range_iterator_mock_p(0,NoOfPixels));
}
