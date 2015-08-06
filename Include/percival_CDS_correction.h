/*
 * percival_CDS_correction.h
 *
 *  Created on: 3 Aug 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_CDS_CORRECTION_H_
#define INCLUDE_PERCIVAL_CDS_CORRECTION_H_
#include "percival_processing.h"
#include <iostream>

void percival_CDS_correction(percival_frame<float> &sample, const percival_frame<float> &reset, percival_frame<float>& output);




#endif /* INCLUDE_PERCIVAL_CDS_CORRECTION_H_ */
