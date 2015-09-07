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

class percival_pipeline_stream_generator : public tbb::filter{
private:

	unsigned int offset;	/*offset within the frame*/
	unsigned int* initial_ptr;
	const unsigned int grain_size;
	const unsigned int size;	/* image size to issue an end of frame */
	const unsigned int max_number_of_tokens;
	unsigned int current_index;
	unsigned int next_index;

public:
	percival_pipeline_stream_generator(
			unsigned int* initial_ptr,
			unsigned int grain_size,
			unsigned int frame_size,
			unsigned int max_number_of_tokens
	):
		tbb::filter(/*is_serial=*/true),
		initial_ptr( initial_ptr ),
		grain_size( grain_size ),
		offset( 0 ),
		size( frame_size ),
		max_number_of_tokens(max_number_of_tokens),
		current_index( 0 ),
		next_index( 0 )
	{}

	void* operator()(void*){
		if(offset < size){
			current_index = next_index;
			*(initial_ptr + current_index) = offset;
			next_index++;
			if(next_index == max_number_of_tokens){
				next_index = 0;
			}
			offset += grain_size;
			return initial_ptr + current_index;
		}else{
			return NULL;
		}
	}
};

class ADC_decode_combined_filter : public tbb::filter{
private:
	const unsigned int grain_size;		/* size of loop */
	percival_calib_params calib_params;

	percival_algorithm_avx< percival_range_iterator_mock_p > algorithm;
	percival_range_iterator_mock_p range;

public:
	ADC_decode_combined_filter(
			const percival_frame<unsigned short> & sample,
			const percival_frame<unsigned short> & reset,
			percival_frame<float> & output,
			const percival_calib_params & calib_params,
			const unsigned int grain_size):
				tbb::filter(/*is_serial=*/false),
				calib_params(calib_params),
				grain_size(grain_size),
				algorithm(sample, reset, output, calib_params),
				range(0,1){}


	void* operator()(void* input){
		unsigned int * offset_ptr = static_cast < unsigned int* >(input);
		unsigned int offset = *offset_ptr;
		/* range to loop over */
		range.lower = offset;
		range.upper = grain_size + offset;
		/*running the algorithm*/
		algorithm(range);
		return NULL;	/*return to next stage if needed*/
	}
};

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
	percival_input_calib_dimension_check_AVX(sample,calib_params);
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

	ADC_decode_combined_filter ADC_decode_CDS ( sample, reset, output, calib_params, grain_size );
	pipeline.add_filter( ADC_decode_CDS );

	pipeline.run( max_tokens );

	pipeline.clear();
}


