/*
 * ADC_decode.h
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_ADC_DECODE_H_
#define INCLUDE_PERCIVAL_ADC_DECODE_H_

#include"percival_frame.h"
#include<exception>

void percival_ADC_decode(const percival_frame<int> &, percival_frame<int> &);
class dimension_mismatch_exception: public std::exception{};

#endif /* INCLUDE_PERCIVAL_ADC_DECODE_H_ */
