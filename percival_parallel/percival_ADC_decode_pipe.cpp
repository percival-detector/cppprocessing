/*
 * percival_ADC_decode_pipe.cpp
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */


#include "percival_processing.h"
#include "percival_parallel.h"

void percival_ADC_decode_pipe(
		const percival_frame<unsigned short int> & input,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		percival_frame<unsigned short int> gain,
		percival_frame<unsigned short int> fine,
		percival_frame<unsigned short int> coarse,
		percival_frame<float> calibrated,
		bool store_gain)
{
	unsigned int height = input.height;
	unsigned int width = input.width;

//	percival_frame<unsigned short int> gain(height, width);
//	percival_frame<unsigned short int> fine(height, width);
//    percival_frame<unsigned short int> coarse(height, width);
//	percival_frame<float> calibrated(height, width);
	unsigned int NoOfPixels = output.width * output.height;

	percival_ADC_decode_pipe_p< tbb::blocked_range<unsigned int> > percival_ADC_decode_pipe_p (input , output, calib_params, gain, fine, coarse, calibrated);
	tbb::parallel_for( tbb::blocked_range<unsigned int>(0, NoOfPixels), percival_ADC_decode_pipe_p, tbb::auto_partitioner());

}


void percival_ADC_decode_pf(
		const percival_frame<unsigned short int> & src_frame,
		percival_frame<float> & des_frame,
		const percival_calib_params & calib_params,
		unsigned int grain_size,
		bool store_gain)
{
	//initialize destination matrix
	if(src_frame.width != des_frame.width || src_frame.height != des_frame.height){
		throw dataspace_exception("percival_ADC_decode: output and input dimension mismatch.");
	}//Saving time for memory allocation

	if(calib_params.Gc.height != src_frame.height){
		throw dataspace_exception("percival_ADC_decode: calibration array height and sample array height mismatch.");
	}

	if(calib_params.Gc.width != 7)
		throw dataspace_exception("percival_ADC_decode: calibration array width and sample array width mismatch. Expected: width == 7.");

	unsigned int NoOfPixels = src_frame.width * src_frame.height;

	percival_ADC_decode_p< tbb::blocked_range<unsigned int> > ADC_decode_p (src_frame, des_frame, calib_params);
	tbb::parallel_for( tbb::blocked_range<unsigned int>(0, NoOfPixels, 1000), ADC_decode_p);

	//	percival_ADC_decode_p< percival_range_iterator_mock_p > ADC_decode_p (src_frame, des_frame, calib_params);
	//	percival_range_iterator_mock_p range(0, NoOfPixels);
	//	unsigned int NoOfGrains = NoOfPixels / grain_size;
	//	unsigned int lower, upper;
	//	for(unsigned int i = 0; i < NoOfGrains; ++i){
	//		lower = i * grain_size;
	//		upper = (i + 1) * grain_size;
	//		range.lower = lower;
	//		range.upper = upper;
	//		ADC_decode_p(range);
	//	}


}


