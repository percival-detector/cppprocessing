/*
 * percival_unit_ADC_decode_p.cpp
 *
 *  Created on: 18 Aug 2015
 *      Author: pqm78245
 */

#include "percival_parallel.h"
#include "percival_exceptions.h"
#include "tbb/task_scheduler_init.h"
#include "percival_data_validity_checks.h"

void percival_unit_ADC_decode_pf(
		const percival_frame<unsigned short int> & input,
		percival_frame<unsigned short int> & Coarse,
		percival_frame<unsigned short int> & Fine,
		percival_frame<unsigned short int> & Gain)
{
	percival_unit_ADC_decode_check(input,Coarse,Fine,Gain);

	unsigned int NoOfPixels = input.width * input.height;


	percival_unit_ADC_decode_p< tbb::blocked_range<unsigned int> > unit_ADC_decode_p(input.data, Coarse.data, Fine.data, Gain.data);
	tbb::parallel_for( tbb::blocked_range<unsigned int>(0, NoOfPixels), unit_ADC_decode_p, tbb::auto_partitioner());
}


