/*
 * percival_unit_ADC_decode.cpp
 *
 *  Created on: 14 Aug 2015
 *      Author: pqm78245
 */


#include "percival_processing.h"
#include "percival_functors.h"
#include "percival_data_validity_checks.h"

void percival_unit_ADC_decode(
		const percival_frame<unsigned short int> & input,
		percival_frame<unsigned short int> & Coarse,
		percival_frame<unsigned short int> & Fine,
		percival_frame<unsigned short int> & Gain)
{
	percival_unit_ADC_decode_check(input,Coarse,Fine,Gain);
	unsigned int NoOfPixels = input.width * input.height;
	percival_range_iterator_mock_p iterator(0, NoOfPixels);
	percival_unit_ADC_decode_p<percival_range_iterator_mock_p> unit_ADC_decode(input.data, Coarse.data, Fine.data, Gain.data);
	unit_ADC_decode(iterator);
}

