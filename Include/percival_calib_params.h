/*
 * percival_calib_params.h
 *
 *  Created on: 20 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_CALIB_PARAMS_H_
#define INCLUDE_PERCIVAL_CALIB_PARAMS_H_

#include"percival_processing.h"

struct percival_calib_params{
	public:
		static percival_frame<float> Gc;
		static percival_frame<float> Oc;
		static percival_frame<float> Gf;
		static percival_frame<float> Of;
		static percival_frame<float> ADU_to_electrons_conversion;
};

#endif /* INCLUDE_PERCIVAL_CALIB_PARAMS_H_ */
