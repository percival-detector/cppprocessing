/*
 * percival_avx.cpp
 *
 *  Created on: 7 Sep 2015
 *      Author: pqm78245
 */

#include "percival_avx.h"
#include "percival_parallel.h"
#include "tbb/blocked_range.h"

void percival_algorithm_avx_pf(
		const percival_frame<unsigned short int> & sample,
		const percival_frame<unsigned short int> & reset,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		unsigned int grain_size)
{
	unsigned short int default_grain_size = sample.width;

	/* Input, calib dimension checks */
	percival_input_calib_dimension_check_AVX(sample,calib_params);
	/* sample reset dimension check */
	percival_input_output_dimension_check(sample, reset);
	percival_input_output_dimension_check(sample, output);

	percival_null_check(sample);
	percival_null_check(reset);
	percival_null_check(output);
	percival_input_calib_NULL_check(calib_params);

	unsigned int width = sample.width;
	unsigned int height = sample.height;
	unsigned int NoOfPixels = width * height;

	/* grain size check */
	if( (grain_size > NoOfPixels) || (grain_size <= 0) )
		grain_size = default_grain_size;
	if( (NoOfPixels%grain_size) )
		throw dataspace_exception{"Grain size is not a factor of No of pixels."};

	percival_algorithm_avx< tbb::blocked_range<unsigned int> > algorithm(sample, reset, output, calib_params, grain_size);

	/*
	 * 	The chunk size should be multiples of grain_size.
	 *	grain size must be factors of NoOfPixels
	 */
	tbb::blocked_range<unsigned int> range(0, NoOfPixels/grain_size, 2);	/* This particular constant 2 is chosen on purpose */
	tbb::parallel_for( range, algorithm);
}
