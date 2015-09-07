/*
 * percival_ADC_decode_pipe.cpp
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */


#include "percival_processing.h"
#include "percival_parallel.h"
#include "percival_data_validity_checks.h"
#include "tbb/pipeline.h"

void percival_ADC_decode_pipe(
		const percival_frame<unsigned short int> & input,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		percival_frame<unsigned short int> gain,
		percival_frame<unsigned short int> fine,
		percival_frame<unsigned short int> coarse,
		percival_frame<float> calibrated,
		bool store_gain)
{
	unsigned int height = input.height;
	unsigned int width = input.width;

	//	percival_frame<unsigned short int> gain(height, width);
	//	percival_frame<unsigned short int> fine(height, width);
	//    percival_frame<unsigned short int> coarse(height, width);
	//	percival_frame<float> calibrated(height, width);
	unsigned int NoOfPixels = output.width * output.height;

	percival_ADC_decode_pipe_p< tbb::blocked_range<unsigned int> > percival_ADC_decode_pipe_p (input , output, calib_params, gain, fine, coarse, calibrated);
	tbb::parallel_for( tbb::blocked_range<unsigned int>(0, NoOfPixels), percival_ADC_decode_pipe_p, tbb::auto_partitioner());

}


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
	head_to_CDS CDS_input;
	CDS_input.input_sample = sample;
	CDS_input.input_reset = reset;
	CDS_input.output = output;

	/*
	 *  Constructing the pipeline
	 *
	 */

	percival_pipeline_stream_generator Input(offset_ptr, grain_size, NoOfPixels, max_tokens);
	pipeline.add_filter( Input );

	ADC_decode_combined_filter<head_to_CDS> ADC_decode_CDS ( CDS_input, calib_params, grain_size );
	pipeline.add_filter( ADC_decode_CDS );

	pipeline.run( max_tokens );

	pipeline.clear();

	/*
	 *
	 *  Parallel_for version for comparison purposes
	 *
	 */

//	unsigned int NoOfPixels = sample.width * sample.height;
//
//	percival_algorithm_p< head_to_CDS, tbb::blocked_range<unsigned int> > percival_p (CDS_input,calib_params);
//	tbb::parallel_for( tbb::blocked_range<unsigned int>(0, NoOfPixels, grain_size), percival_p, tbb::auto_partitioner());

}


