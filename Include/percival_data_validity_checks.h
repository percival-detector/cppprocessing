/*
 * percival_data_validity_checks.h
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_DATA_VALIDITY_CHECKS_H_
#define INCLUDE_PERCIVAL_DATA_VALIDITY_CHECKS_H_
/*Separate out functions that check for dimensions, datatype and range validity etc.*/
#include "percival_exceptions.h"
#include "percival_processing.h"

/*Function 1*/
void percival_unit_ADC_decode_check(
		const percival_frame<unsigned short int> & input,
		percival_frame<unsigned short int> & Coarse,
		percival_frame<unsigned short int> & Fine,
		percival_frame<unsigned short int> & Gain);

/*Function 2*/
void percival_unit_ADC_calibration_check(const percival_frame<unsigned short int> & Coarse,
		const  percival_frame<unsigned short int> & Fine,
		percival_frame<float>& output,
		const percival_calib_params & calib);

/*Function 3*/
void percival_unit_gain_multiplication_check(
		const percival_frame<unsigned short int> & src_frame,
		const percival_frame<float> & calibrated,
		percival_frame<float> & output,
		const percival_calib_params & calib_params);

/*Function 4*/
void percival_CDS_correction_check(
		percival_frame<float> &sample,
		const percival_frame<float> &reset,
		percival_frame<float> &output);

/*Function 5*/
void percival_ADU_to_electron_correction_check(
		percival_frame<float> &CDS_Img,
		percival_frame<float> &output,
		const percival_calib_params & calib);

/* Function 6 */
/* Checks whether input and calibration array dimensions are consistent */
void percival_input_calib_dimension_check(
		const percival_frame<unsigned short int> & input,
		const percival_calib_params & calib);

#endif /* INCLUDE_PERCIVAL_DATA_VALIDITY_CHECKS_H_ */
