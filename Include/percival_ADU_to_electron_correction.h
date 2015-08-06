/*
 * percival_ADU_to_electron_correction.h
 *
 *  Created on: 4 Aug 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_ADU_TO_ELECTRON_CORRECTION_H_
#define INCLUDE_PERCIVAL_ADU_TO_ELECTRON_CORRECTION_H_

#include "percival_processing.h"
#include <iostream>

void percival_ADU_to_electron_correction(percival_frame<float> &CDS_Img, percival_frame<float> &output, const percival_calib_params &);

#endif /* INCLUDE_PERCIVAL_ADU_TO_ELECTRON_CORRECTION_H_ */
