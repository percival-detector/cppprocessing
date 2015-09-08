/*
 * percival_parallel.h
 *
 *  Created on: 18 Aug 2015
 *      Author: pqm78245
 */

#ifndef PERCIVAL_PARALLEL_PERCIVAL_PARALLEL_H_
#define PERCIVAL_PARALLEL_PERCIVAL_PARALLEL_H_

#include "tbb/tbb.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "percival_processing.h"
#include "percival_functors.h"
#include "percival_avx.h"

/* Use the following to set optimised parameters
struct optimised_parameter{
	const static unsigned int grain_size;
	const static unsigned int max_tokens;
	const static unsigned int max_threads;
};
*/

void percival_unit_ADC_decode_pf(
		const percival_frame<unsigned short int> & input,
		percival_frame<unsigned short int> & Coarse,
		percival_frame<unsigned short int> & Fine,
		percival_frame<unsigned short int> & Gain);

void percival_unit_ADC_calibration_pf(
		const percival_frame<unsigned short int> & Coarse,
		const  percival_frame<unsigned short int> & Fine,
		percival_frame<float>& output,
		const percival_calib_params &,
		bool check_dimensions = true);

void percival_unit_gain_multiplication_pf(
		const percival_frame<unsigned short int> & src_frame,
		const percival_frame<float> & calibrated,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		bool check_dimensions = true);
/*
 * ADC_decode stage filter
 */
void percival_ADC_decode_combined_pipeline(
		const percival_frame<unsigned short int> & sample,
		const percival_frame<unsigned short int> & reset,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		unsigned int grain_size = 3528,
		unsigned int max_tokens = 20);

class percival_pipeline_stream_generator;


class ADC_decode_combined_filter : public tbb::filter{
private:
	const unsigned int grain_size;		/* size of loop */
	percival_calib_params calib_params;

	percival_algorithm_avx algorithm;
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


#endif /* PERCIVAL_PARALLEL_PERCIVAL_PARALLEL_H_ */
