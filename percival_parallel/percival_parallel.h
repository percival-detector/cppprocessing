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

#endif /* PERCIVAL_PARALLEL_PERCIVAL_PARALLEL_H_ */
