/*
 * percival_unit_ADC_calibration.cpp
 *
 *  Created on: 14 Aug 2015
 *      Author: pqm78245
 */
#include "percival_processing.h"
void percival_unit_ADC_calibration(
		const percival_frame<unsigned short int> & Coarse,
		const  percival_frame<unsigned short int> & Fine,
		const percival_frame<unsigned short int> & Gain,
		percival_frame<float>& output,
		const percival_calib_params & calib,
		bool check_dimensions)
{
	if(check_dimensions){
		if(output.width != Coarse.width || output.height != Coarse.height)
				throw dataspace_exception("In percival_unit_ADC_calibration: coarse_frame and output frame dimensions mismatch.");
		if(output.width != Fine.width || output.height != Fine.height)
				throw dataspace_exception("In percival_unit_ADC_calibration: fine_frame and output frame dimensions mismatch.");
		if(output.width != Gain.width || output.height != Gain.height)
				throw dataspace_exception("In percival_unit_ADC_calibration: gain_frame and output frame dimensions mismatch.");

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

	for(unsigned int i = 0; i < NoOfPixels; i++){

		unsigned int col = i % output.width;			//0 ~ frame_width - 1
		unsigned int row = (i - col) / output.width;
		unsigned int position_in_calib_array = (col % 7) + (row * calib.Gc.width); //7 from 7 ADCs. todo code this in config.

		float Gc_at_this_pixel = *(calib.Gc.data + position_in_calib_array);
		float Oc_at_this_pixel = *(calib.Oc.data + position_in_calib_array);
		float Gf_at_this_pixel = *(calib.Gf.data + position_in_calib_array);
		float Of_at_this_pixel = *(calib.Of.data + position_in_calib_array);

		float VinMax=1.43;
		//these two values are from February test data from Hazem. should be changed if calibration data changes
		float FMax = 222;
		float CMax = 26;

		*(output.data + i)	= (float)
	    		(FMax * CMax *
				(
					1.0-
					(
							(1.0/VinMax)*
							(
									(
											(Oc_at_this_pixel - *(Fine.data + i) - 1.0) / Gc_at_this_pixel		//In hazem's code coarseBits == FineArr, fineBits == CoarseArr
									)
							+		(
											(*(Coarse.data + i) - Of_at_this_pixel) / Gf_at_this_pixel
									)
							)
					)
				)
				);


	}


}


