/*
 * percival_unit_ADC_calibration.cpp
 *
 *  Created on: 14 Aug 2015
 *      Author: pqm78245
 */
#include "percival_processing.h"
#include "percival_functors.h"
#include "percival_data_validity_checks.h"

void percival_unit_ADC_calibration(
		const percival_frame<unsigned short int> & Coarse,
		const  percival_frame<unsigned short int> & Fine,
		percival_frame<float>& output,
		const percival_calib_params & calib,
		bool check_dimensions)
{
	if(check_dimensions){
		percival_unit_ADC_calibration_check(Coarse,Fine,output,calib);
	}

	unsigned int NoOfPixels = output.width * output.height;
	percival_range_iterator_mock_p iterator(0, NoOfPixels);
	percival_unit_ADC_calibration_p <percival_range_iterator_mock_p> unit_ADC_calibration(Coarse, Fine, output, calib );
	unit_ADC_calibration(iterator);



}


