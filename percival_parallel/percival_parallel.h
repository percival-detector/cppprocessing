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

template<typename input_type>
class ADC_decode_combined_filter : public tbb::filter{
private:
	const unsigned int grain_size;		/* size of loop */
	percival_calib_params calib_params;

	percival_algorithm_avx< input_type, percival_range_iterator_mock_p > algorithm;
	percival_range_iterator_mock_p range;

public:
	ADC_decode_combined_filter(
			input_type & input,
			const percival_calib_params & calib_params,
			const unsigned int grain_size):
				tbb::filter(/*is_serial=*/false),
				calib_params(calib_params),
				grain_size(grain_size),
				algorithm(input, calib_params),
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



#endif /* PERCIVAL_PARALLEL_PERCIVAL_PARALLEL_H_ */
