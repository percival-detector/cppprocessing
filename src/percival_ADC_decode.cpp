/*
 * ADC_decode.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"

//todo change this to a template

void percival_ADC_decode(const percival_frame<short int> & src_frame, percival_frame<float> & des_frame){
	//initialize destination matrix

	des_frame.set_frame_size(src_frame.height, src_frame.width);

	//calibration parameters
	percival_calib_params calib_params;

	const int calib_data_height = calib_params.Gc.height;
	const int calib_data_width = calib_params.Gc.width;

	//algorithm

	for(int i = 0; i < des_frame.width * des_frame.height; i++){
		/*
		 * minimising access
		short int pixel = *(src_frame.data + i);
		*/

		short int gain = *(src_frame.data + i) % 4;
		short int fineBits = (*(src_frame.data + i) >> 2) % 256; // the next 8 digits
		short int coarseBits = (*(src_frame.data + i) >> 10) % 32; // the next 5 bits

		//converting from linear representation to 2D representation. To speed up take modulo no of calibration pixels.
		int col = i % src_frame.width;			//0 ~ frame_width - 1
		int row = (i - col) / src_frame.width;
		int position_in_calib_array = (col % calib_data_height) * calib_data_width + row;

		/*
		 * The following can be used to reduce number of accesses.
			float* Gc = calib_params.Gc.data;
			float* Oc = calib_params.Oc.data;
			float* Gf = calib_params.Gf.data;
			float* Of = calib_params.Of.data;
			*/

		float Gc_at_this_pixel = *(calib_params.Gc.data + position_in_calib_array);
		float Oc_at_this_pixel = *(calib_params.Oc.data + position_in_calib_array);
		float Gf_at_this_pixel = *(calib_params.Gf.data + position_in_calib_array);
		float Of_at_this_pixel = *(calib_params.Of.data + position_in_calib_array);

//		*(des_frame.data + i) = fineBits;

//
//		 self.CMax=math.ceil(self.GC.max())
//		 self.FMax=(math.ceil(self.GF.max()*0.065))#0.065

		//TODO:Note that the precision of these numbers is not great. noticeable to 0.0001
		float VinMax=1.43;

		//these two values are from February test data from Hazem. should be changed if calibration data changes
		float FMax = 222;
		float CMax = 26;


		*(des_frame.data + i)	= (float)
	    		(FMax * CMax *
				(
					1.0-
					(
							(1.0/VinMax)*
							(
									(
											(Oc_at_this_pixel - coarseBits- 1.0) / Gc_at_this_pixel		//In hazem's code coarseBits == FineArr, fineBits == CoarseArr
									)
							+		(
											(fineBits - Of_at_this_pixel) / Gf_at_this_pixel
									)
							)
					)
				)
				);
	}
}

/*
 * TODO:
 *
 * creating a class named percival_ADC_decode_binary_to_decimal_lookup_table
 * using singleton design pattern
 * 			if table exists, look up the table for values.
 * 			if not, calculate the table and make it constant.
 * 		lookup method
 * 			1. no negative input or out-of-range indices allowed. (	CHECK CRITICAL!!! )
 * 			2. can flexibly change computation algorithm (inheriting an short interface for algorithm) (might be unnecessary)
 * decide how to partition the incoming array to effectively use CPU cache. much later...
 *
 *
 */

