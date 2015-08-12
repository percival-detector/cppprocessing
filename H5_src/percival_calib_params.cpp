/*
 * percival_calib_params.cpp
 *
 *  Created on: 20 Jul 2015
 *      Author: pqm78245
 */

#include"percival_calib_params.h"

bool transposed = true;
percival_frame<float> percival_calib_params::Gc;
percival_frame<float> percival_calib_params::Oc;
percival_frame<float> percival_calib_params::Gf;
percival_frame<float> percival_calib_params::Of;
percival_frame<float> percival_calib_params::ADU_to_electrons_conversion;		//32 bit floating point
