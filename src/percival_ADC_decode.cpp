/*
 * ADC_decode.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"

void percival_ADC_decode(const percival_frame<int> & src_frame, percival_frame<int> & des_frame){
	//initialize destination matrix
	//Throw exception if dimensions are not the same
	if(src_frame.width != des_frame.width)
		throw dimension_mismatch_exception{};
	if(src_frame.height != des_frame.height)
		throw dimension_mismatch_exception{};

	//algorithm
	int NoOfPixels = des_frame.width * des_frame.height;
	for(int i = 0; i < NoOfPixels; i++){

		int pixel = *(src_frame.data+i);
		int gain = pixel % 4;
		int fineBits = (pixel >> 2) % 256; // the next 8 digits
		int coarseBits = (pixel >> 10) % 32; // the next 5 bits

		*(des_frame.data + i) = gain * (coarseBits * 4 + fineBits * 1); //algorithm
	}
}

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

