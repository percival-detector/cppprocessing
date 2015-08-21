/*
 * percival_unit_ADC_calibration_pf.cpp
 *
 *  Created on: 19 Aug 2015
 *      Author: pqm78245
 */

#include "percival_parallel.h"		/*for tbb lib and functors*/
#include "percival_data_validity_checks.h"

void percival_unit_ADC_calibration_pf(const percival_frame<unsigned short int> & Coarse,
		const  percival_frame<unsigned short int> & Fine,
		percival_frame<float>& output,
		const percival_calib_params & calib,
		bool check_dimensions)
{
	if(check_dimensions){
		percival_unit_ADC_calibration_check(Coarse,Fine,output,calib);
	}

	unsigned int NoOfPixels = output.width * output.height;
	percival_unit_ADC_calibration_p< tbb::blocked_range<unsigned int> > unit_ADC_calibration_p(Coarse,Fine,output,calib);
	tbb::parallel_for( tbb::blocked_range<unsigned int>(0, NoOfPixels), unit_ADC_calibration_p, tbb::auto_partitioner());

}


