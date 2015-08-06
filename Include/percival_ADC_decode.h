/*
 * ADC_decode.h
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_ADC_DECODE_H_
#define INCLUDE_PERCIVAL_ADC_DECODE_H_

#include "percival_processing.h"
#include<exception>

void percival_ADC_decode(const percival_frame<short int> &, percival_frame<float> &, const percival_calib_params & calib_params, bool store_gain = false);

/*
 * TODO:
 *
 * creating a class named percival_ADC_decode_binary_to_decimal_lookup_table
 * using singleton design pattern
 * 			if table exists, look up the table for values.
 * 			if not, calculate the table and make it constant.
 * 		lookup method
 * 			1. no negative input or out-of-range indices allowed. (	CHECK CRITICAL!!! )
 * 			2. can flexibly change computation algorithm (inheriting an interface for algorithm) (might be unnecessary)
 * decide how to partition the incoming array to effectively use CPU cache. much later...
 *
 *
 */




#endif /* INCLUDE_PERCIVAL_ADC_DECODE_H_ */
