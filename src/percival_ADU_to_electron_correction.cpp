/*
 * ADC_to_electron_correction.cpp
 *
 *  Created on: 4 Aug 2015
 *      Author: pqm78245
 */
#include "percival_processing.h"
#include "percival_data_validity_checks.h"

void percival_ADU_to_electron_correction(percival_frame<float> &CDS_Img, percival_frame<float> &output, const percival_calib_params & calib){
		//dimension checks
		float *ADU_per_el = calib.ADU_to_electrons_conversion.data;

		percival_ADU_to_electron_correction_check(CDS_Img,output,calib);

		for(unsigned int i = 0; i < CDS_Img.width * CDS_Img.height; i ++){
				*( output.data + i ) = *( CDS_Img.data + i ) / *( ADU_per_el + i);
		}
}

