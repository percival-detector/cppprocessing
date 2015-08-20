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

void percival_unit_ADC_decode_check(
		const percival_frame<unsigned short int> & input,
		percival_frame<unsigned short int> & Coarse,
		percival_frame<unsigned short int> & Fine,
		percival_frame<unsigned short int> & Gain);


#endif /* INCLUDE_PERCIVAL_DATA_VALIDITY_CHECKS_H_ */
