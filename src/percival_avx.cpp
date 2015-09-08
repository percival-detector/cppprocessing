/*
 * percival_avx.cpp
 *
 *  Created on: 7 Sep 2015
 *      Author: pqm78245
 */

#include "percival_avx.h"
#include "percival_parallel.h"

void percival_ADC_decode_combined_pipeline_avx(
		const percival_frame<unsigned short int> & sample,
		const percival_frame<unsigned short int> & reset,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		unsigned int grain_size,
		unsigned int max_tokens)
{
	unsigned short int default_grain_size = sample.width;

	/* Input, calib dimension checks */
	percival_input_calib_dimension_check_AVX(sample,calib_params);
	/* sample reset dimension check */
//	if( (sample.width != reset.width) || (sample.height != reset.height) )
//		throw dataspace_exception("Sample, reset frames dimension mismatch.");
//	if( (sample.width != output.width) || (sample.height != output.height) )
//		throw dataspace_exception("Sample, Output frames dimension mismatch.");
//
//	if( ( sample.data == NULL) || ( reset.data == NULL ) || ( output.data == NULL ) )
//		throw dataspace_exception("Pointer to source or destination frame is NULL.");
//
//	if( ( calib_params.Gc.data == NULL )|| ( calib_params.Gf.data == NULL )|| ( calib_params.Oc.data == NULL ) || ( calib_params.Of.data == NULL ))
//		throw dataspace_exception("Pointer to calibration data is NULL.");
//
//	if( ( calib_params.Gain_lookup_table1.data == NULL )|| ( calib_params.Gain_lookup_table2.data == NULL )|| ( calib_params.Gain_lookup_table3.data == NULL ) || ( calib_params.Gain_lookup_table4.data == NULL ))
//		throw dataspace_exception("Pointer to calibration data is NULL.");
//
//	if( ( calib_params.ADU_to_electrons_conversion.data == NULL) )
//		throw dataspace_exception("Pointer to calibration data is NULL.");

	unsigned int width = sample.width;
	unsigned int height = sample.height;
	unsigned int NoOfPixels = width * height;

	/* grain size check */
	if( (grain_size > NoOfPixels) || (grain_size <= 0) )
		grain_size = default_grain_size;

	/* starting a pipeline */
	tbb::pipeline pipeline;

	/*
	 * A list of offset from the start of image array,
	 * corresponding to each token
	 *
	 * */
	unsigned int offset_arr [max_tokens];
	unsigned int *offset_ptr = & offset_arr[0];

	/* Initialising the input image struct */
	/*
	 *  Constructing the pipeline
	 *
	 */

	percival_pipeline_stream_generator Input(offset_ptr, grain_size, NoOfPixels, max_tokens);
	pipeline.add_filter( Input );

	ADC_decode_combined_filter ADC_decode_CDS ( sample, reset, output, calib_params, grain_size );
	pipeline.add_filter( ADC_decode_CDS );

	pipeline.run( max_tokens );

	pipeline.clear();
}


