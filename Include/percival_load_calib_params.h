/*
 * percival_load_calib_params.h
 *
 *  Created on: 12 Aug 2015
 *      Author: pqm78245
 */

#ifndef H5_SRC_PERCIVAL_LOAD_CALIB_PARAMS_H_
#define H5_SRC_PERCIVAL_LOAD_CALIB_PARAMS_H_

#include "percival_processing.h"

void percival_load_calib_params(percival_calib_params & calib_params, percival_global_params & global_params, bool align_mem = 0);
void percival_inverse_frame(percival_frame<float> &input, percival_frame<float> &output);

#endif /* H5_SRC_PERCIVAL_LOAD_CALIB_PARAMS_H_ */
