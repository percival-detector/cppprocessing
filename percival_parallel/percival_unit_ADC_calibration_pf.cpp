/*
 * percival_unit_ADC_calibration_pf.cpp
 *
 *  Created on: 19 Aug 2015
 *      Author: pqm78245
 */

#include "percival_parallel.h"

void percival_unit_ADC_calibration_pf(const percival_frame<unsigned short int> & Coarse,
		const  percival_frame<unsigned short int> & Fine,
		percival_frame<float>& output,
		const percival_calib_params & calib,
		bool check_dimensions)
{
	if(check_dimensions){
		if(output.width != Coarse.width || output.height != Coarse.height)
				throw dataspace_exception("In percival_unit_ADC_calibration: coarse_frame and output frame dimensions mismatch.");
		if(output.width != Fine.width || output.height != Fine.height)
				throw dataspace_exception("In percival_unit_ADC_calibration: fine_frame and output frame dimensions mismatch.");

		if(calib.Gc.width != 7)
			throw dataspace_exception("In percival_unit_ADC_calibration: width of Gc array is not 7.");
		if(calib.Gf.width != 7)
				throw dataspace_exception("In percival_unit_ADC_calibration: width of Gf array is not 7.");
		if(calib.Oc.width != 7)
				throw dataspace_exception("In percival_unit_ADC_calibration: width of Oc array is not 7.");
		if(calib.Of.width != 7)
				throw dataspace_exception("In percival_unit_ADC_calibration: width of Of array is not 7.");

		if(calib.Gc.height != output.height)
			throw dataspace_exception("In percival_unit_ADC_calibration: height of Gc array and height of output are unequal.");
		if(calib.Gf.height != output.height)
					throw dataspace_exception("In percival_unit_ADC_calibration: height of Gf array and height of output are unequal.");
		if(calib.Oc.height != output.height)
					throw dataspace_exception("In percival_unit_ADC_calibration: height of Oc array and height of output are unequal.");
		if(calib.Of.height != output.height)
					throw dataspace_exception("In percival_unit_ADC_calibration: height of Of array and height of output are unequal.");
	}

	unsigned int NoOfPixels = output.width * output.height;
	percival_unit_ADC_calibration_p< tbb::blocked_range<unsigned int> > unit_ADC_calibration_p(Coarse,Fine,output,calib);
	tbb::parallel_for( tbb::blocked_range<unsigned int>(0, NoOfPixels, 25000), unit_ADC_calibration_p);

}


