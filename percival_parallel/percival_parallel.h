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

void percival_ADC_decode_pipe(
		const percival_frame<unsigned short int> & input,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		percival_frame<unsigned short int> gain,
		percival_frame<unsigned short int> fine,
		percival_frame<unsigned short int> coarse,
		percival_frame<float> calibrated,
		bool store_gain = false);

void percival_ADC_decode_pf(
		const percival_frame<unsigned short int> & src_frame,
		percival_frame<float> & des_frame,
		const percival_calib_params & calib_params,
		unsigned int grain_size = 100000,
		bool store_gain = false);

template<typename type>
struct seg_sam_res_pair{
	percival_frame<type> sample;
	percival_frame<type> reset;
};

#endif /* PERCIVAL_PARALLEL_PERCIVAL_PARALLEL_H_ */
