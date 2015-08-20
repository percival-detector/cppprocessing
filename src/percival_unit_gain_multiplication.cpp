/*
 * percival_unit_gain_multiplication.cpp
 *
 *  Created on: 14 Aug 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"
#include "percival_data_validity_checks.h"

void percival_unit_gain_multiplication(
		const percival_frame<unsigned short int> & src_frame,
		const percival_frame<float> & calibrated,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		bool check_dimensions)
{
	if(check_dimensions)
		percival_unit_gain_multiplication_check(src_frame,calibrated,output,calib_params);

	unsigned int NoOfPixels = src_frame.width * src_frame.height;
	unsigned short int gain;
	float gain_factor;

	for(unsigned int i = 0; i < NoOfPixels; i ++){

		gain =  *(src_frame.data + i) % 0b100;

		switch(gain){
		case 0b00:
			gain_factor = *(calib_params.Gain_lookup_table1.data + i);
			break;
		case 0b01:
			gain_factor = *(calib_params.Gain_lookup_table2.data + i);
			break;
		case 0b10:
			gain_factor = *(calib_params.Gain_lookup_table3.data + i);
			break;
		case 0b11:
			gain_factor = *(calib_params.Gain_lookup_table4.data + i);
			break;
		default:
			throw datatype_exception("Invalid gain bit detected.");
		}
		*(output.data + i) = *(calibrated.data + i) * gain_factor;

	}
}
