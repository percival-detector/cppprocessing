/*
 * percival_ADC_decode_pipe.cpp
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */


#include "percival_processing.h"
#include "percival_parallel.h"
#include "percival_data_validity_checks.h"
#include "percival_avx.h"
#include "tbb/pipeline.h"

/*===============================================================================================================================*/
/*
 *  Generate a stream of data for later stages
 */
/*
 *
 *	A few rows as a token.
 *	the number of rows should be a factor of 3717 = ( 3 * 3 * 7 * 59)
 *  Possible choices are
 *  	1,3,7,9,21, 59,63,177,413,531,
 *  	1239,3717
 *
 *	Or use a fraction of a row as tokens.
 *	The fraction should be a factor of 3528 = (2 * 2 * 2 * 3 * 3 * 7 * 7 * 9)
 *	Possible choices are :
 *		1, 2, 3, 4, 6,  7, 8, 9, 12, 14,
 *		18, 21, 24, 28, 36,  42, 49,  56, 63, 72,
 *		84, 98, 126, 147, 168,  196, 252, 392, 441,504,
 *		588, 882, 1176, 1764, 3528
 */

/* Generating a stream of data*/
/* For segmentation */



void percival_ADC_decode_combined_pipeline(
		const percival_frame<unsigned short int> & sample,
		const percival_frame<unsigned short int> & reset,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		unsigned int grain_size,
		unsigned int max_tokens)
{
	unsigned short int default_grain_size = sample.width;

	/* Input, calib dimension checks */
	percival_input_calib_dimension_check(sample,calib_params);
	/* sample reset dimension check */
	if( (sample.width != reset.width) || (sample.height != reset.height) )
		throw dataspace_exception("Sample, reset frames dimension mismatch.");
	if( (sample.width != output.width) || (sample.height != output.height) )
		throw dataspace_exception("Sample, Output frames dimension mismatch.");

	unsigned int width = sample.width;
	unsigned int height = sample.height;
	unsigned int NoOfPixels = width * height;

	/* grain size check */
	if( (grain_size > NoOfPixels) || (grain_size <= 0) )
		grain_size = default_grain_size;

	/* starting a pipeline */
	tbb::pipeline pipeline;

	/*
	 * A list of offset from the start of image array,
	 * corresponding to each token
	 *
	 * */
	unsigned int offset_arr [max_tokens];
	unsigned int *offset_ptr = & offset_arr[0];

	/* Initialising the input image struct */
	/*
	 *  Constructing the pipeline
	 *
	 */

	percival_pipeline_stream_generator Input(offset_ptr, grain_size, NoOfPixels, max_tokens);
	pipeline.add_filter( Input );

//	ADC_decode_combined_filter< percival_algorithm_p< percival_range_iterator_mock_p > > ADC_decode_CDS( sample, reset, output, calib_params, grain_size );
//	pipeline.add_filter( ADC_decode_CDS );

	pipeline.run( max_tokens );

	pipeline.clear();
}


