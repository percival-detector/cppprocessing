/*
 * percival_avx.cpp
 *
 *  Created on: 7 Sep 2015
 *      Author: pqm78245
 */

#include "percival_avx.h"
#include "percival_parallel.h"
#include "tbb/blocked_range.h"

/*
 * 	This function is used to get the largest 2's exponential smaller than x.
 * 	If x is 1, return 1; if x is 0, return 0;
 * 	Only accepts uint32
 */
inline unsigned int get_two_exponentials(unsigned int x){
	if(x == 0)
		return 0;
	if(x == 1)
		return 1;

	unsigned int two_exponentials = 1;
	for(unsigned int i=0; i < 32; i++){
		if(two_exponentials*2 <= x)
			two_exponentials*=2;
	}
	return two_exponentials;
}

void percival_ADC_decode_combined_pipeline_avx(
		const percival_frame<unsigned short int> & sample,
		const percival_frame<unsigned short int> & reset,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		unsigned int grain_size,
		unsigned int max_tokens)
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

	percival_algorithm_avx< tbb::blocked_range<unsigned int> > algorithm(sample, reset, output, calib_params, grain_size);

	/*
	 * 	The chunk size should be multiples of grain_size.
	 *	grain size must be factors of NoOfPixels
	 */
	tbb::blocked_range<unsigned int> range(0, NoOfPixels/grain_size, 2);
	tbb::parallel_for( range, algorithm);

//	do{
//		lower = upper - get_two_exponentials(upper);
//		tbb::blocked_range<unsigned int> (lower*grain_size, upper*grain_size, grain_size);
//		tbb::parallel_for( range, algorithm);
//		upper = lower;
//	}while(upper);

}
