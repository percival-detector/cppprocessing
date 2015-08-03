/*
 * percival_calib_params.h
 *
 *  Created on: 20 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_CALIB_PARAMS_H_
#define INCLUDE_PERCIVAL_CALIB_PARAMS_H_

#include"percival_processing.h"
#include <string>

class percival_calib_params{
	private:
		static bool is_initialised;
	public:
		static percival_frame<float> Gc;
		static percival_frame<float> Oc;
		static percival_frame<float> Gf;
		static percival_frame<float> Of;

		static percival_global_params params;

		percival_calib_params();
		percival_calib_params(percival_global_params &);
};

#endif /* INCLUDE_PERCIVAL_CALIB_PARAMS_H_ */
