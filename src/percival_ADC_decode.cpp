/*
 * ADC_decode.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"

/*
 * 	Original Non-parallel version of a combination of the following steps,
 * 	ADC decode
 * 	ADC correction
 * 	Gain multiplication
 *
 */

void percival_ADC_decode_correction_gain_multiplication(const percival_frame<unsigned short int> & src_frame, percival_frame<float> & des_frame, const percival_calib_params & calib_params, bool store_gain){
	//initialize destination matrix
	if(src_frame.width != des_frame.width || src_frame.height != des_frame.height){
		throw dataspace_exception("percival_ADC_decode: output and input dimension mismatch.");
	}//Saving time for memory allocation

	if(calib_params.Gc.height != src_frame.height){
		throw dataspace_exception("percival_ADC_decode: calibration array height and sample array height mismatch.");
	}

	if(calib_params.Gc.width != 7)
		throw dataspace_exception("percival_ADC_decode: calibration array width and sample array width mismatch. Expected: width == 7.");

	//calibration parameters
	const unsigned int calib_data_height = calib_params.Gc.height;
	const unsigned int calib_data_width = calib_params.Gc.width;

	//algorithm

	for(unsigned int i = 0; i < des_frame.width * des_frame.height; i++){	//int i is sufficient
		/*
		 * minimising access
		short int pixel = *(src_frame.data + i);
		 */
		//use unsigned datatypes
		short unsigned int gain = *(src_frame.data + i) % 4;
		short unsigned int fineBits = (*(src_frame.data + i) >> 2) % 256; // the next 8 digits
		short unsigned int coarseBits = (*(src_frame.data + i) >> 10) % 32; // the next 5 bits

		//stores pixels that DO NOT need CDS_substraction todo:change this to "that DO" later
		if((gain & 0x1) && store_gain)		//change value here for selection criteria
			des_frame.CDS_subtraction_indices.push_back(i);		//note that some reset frame does not need to be ADCdecoded.
		//This bit of code was used when the calibration arrays were all transposed.
		//		//converting from linear representation to 2D representation. To speed up take modulo no of calibration pixels.
		unsigned int col = i % src_frame.width;			//0 ~ frame_width - 1
		unsigned int row = (i - col) / src_frame.width;

		//this bit is used when the arrays are property transposed.
		unsigned int position_in_calib_array = (col % 7) + (row * calib_data_width); //7 from 7 ADCs. todo code this in config.

		float Gc_at_this_pixel = *(calib_params.Gc.data + position_in_calib_array);
		float Oc_at_this_pixel = *(calib_params.Oc.data + position_in_calib_array);
		float Gf_at_this_pixel = *(calib_params.Gf.data + position_in_calib_array);
		float Of_at_this_pixel = *(calib_params.Of.data + position_in_calib_array);

		float VinMax=1.43;
		//these two values are from February test data from Hazem. should be changed if calibration data changes
		float FMax = 222;
		float CMax = 26;

		/*
		 *
		 * Gain multiplication. Each Gain-lookup_table is as large as the sample frame, and corresponds to one gain bit.
		 *
		 */
		float gain_factor = 1;
		switch(gain){
		case 0b00:
			gain_factor = *(calib_params.Gain_lookup_table1.data + i);
			break;
		case 0b01:
			gain_factor = *(calib_params.Gain_lookup_table2.data + i);
			break;
		case 0b10:
			gain_factor = *(calib_params.Gain_lookup_table3.data + i);
			break;
		case 0b11:
			gain_factor = *(calib_params.Gain_lookup_table4.data + i);
			break;
		default:
			throw datatype_exception("Invalid gain bit detected.");
		}

		*(des_frame.data + i)	= (float)gain_factor *
				(FMax * CMax *
						(
								1.0-
								(
										(1.0/VinMax)*
										(
												(
														(Oc_at_this_pixel - fineBits - 1.0) / Gc_at_this_pixel		//In hazem's code coarseBits == FineArr, fineBits == CoarseArr
												)
												+		(
														(coarseBits - Of_at_this_pixel) / Gf_at_this_pixel
												)
										)
								)
						)
				);

	}
}
