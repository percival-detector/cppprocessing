/*
 * percival_unit_ADC_decode.cpp
 *
 *  Created on: 14 Aug 2015
 *      Author: pqm78245
 */


#include "percival_processing.h"

void percival_unit_ADC_decode(
		const percival_frame<unsigned short int> & input,
		percival_frame<unsigned short int> & Coarse,
		percival_frame<unsigned short int> & Fine,
		percival_frame<unsigned short int> & Gain)
{
	if(input.width != Coarse.width || input.height != Coarse.height)
		throw dataspace_exception("In percival_unit_ADC_decode: coarse_frame and input frame dimensions mismatch.");
	if(input.width != Fine.width || input.height != Fine.height)
		throw dataspace_exception("In percival_unit_ADC_decode: fain_frame and input frame dimensions mismatch.");
	if(input.width != Gain.width || input.height != Gain.height)
		throw dataspace_exception("In percival_unit_ADC_decode: gain_frame and input frame dimensions mismatch.");

	if(Coarse.data == Fine.data)
		throw datatype_exception("In percival_unit_ADC_decode: Coarse and Fine frame pointers are identical.");
	if(Coarse.data == Gain.data)
		throw datatype_exception("In percival_unit_ADC_decode: Coarse and Gain frame pointers are identical.");
	if(Gain.data == Fine.data)
		throw datatype_exception("In percival_unit_ADC_decode: Gain and Fine frame pointers are identical.");

	unsigned int NoOfPixels = input.width * input.height;

	for(unsigned int i = 0; i < NoOfPixels; i ++){
		*(Gain.data + i) = *(input.data + i) % 0x0004;
		*(Fine.data + i) = (*(input.data + i)  >> 2 ) %0x100;
		*(Coarse.data + i) = (*(input.data + i) << 1) >> 11;
	}
}

