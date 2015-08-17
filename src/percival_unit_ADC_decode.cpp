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
	percival_range_iterator_mock_p iterator(0, NoOfPixels);
	percival_unit_ADC_decode_p<percival_range_iterator_mock_p> unit_ADC_decode(input.data, Coarse.data, Fine.data, Gain.data);
	unit_ADC_decode(iterator);
}

