/*
 * percival_unit_gain_multiplication_pf.cpp
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */

#include "percival_parallel.h"
#include "percival_exceptions.h"
#include "tbb/task_scheduler_init.h"
#include "percival_data_validity_checks.h"

void percival_unit_gain_multiplication_pf(
		const percival_frame<unsigned short int> & src_frame,
		const percival_frame<float> & calibrated,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		bool check_dimensions)
{
	if(check_dimensions)
		percival_unit_gain_multiplication_check(src_frame,calibrated,output,calib_params);

	unsigned int NoOfPixels = src_frame.width * src_frame.height;

	percival_unit_gain_multiplication_p< tbb::blocked_range<unsigned int> > unit_gain_multiplication_p(src_frame,calibrated,output,calib_params);
	tbb::parallel_for( tbb::blocked_range<unsigned int>(0, NoOfPixels, 5000), unit_gain_multiplication_p);
}



