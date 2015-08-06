/*
 * percival_CDS_correction.cpp
 *
 *  Created on: 3 Aug 2015
 *      Author: pqm78245
 */

#include "percival_CDS_correction.h"

void percival_CDS_correction(percival_frame<float> &sample, const percival_frame<float> &reset, percival_frame<float> &output){
	//dimension checks
	if((sample.width != reset.width) || (sample.height != reset.height))
		throw dataspace_exception{"In percival_CDS_correction(): sample dimension and reset dimension mismatch."};

	if((output.width != sample.width) || (output.height != sample.height))
		output.set_frame_size(sample.height, sample.width);
//can also put this as operator overloading
	//todo: Confirm which gain bit indicates CDS_subtraction, 00, 01, 10, or 11
	//exclude points in the list
	for(int i = 0; i < sample.width * sample.height; i ++){
		if(( sample.CDS_subtraction_indices.size()==0 ) || ( i != sample.CDS_subtraction_indices.back())){
			*( output.data + i ) = *( sample.data + i ) - *( reset.data + i );			//todo:apply the delta mask!!!!!!!!
		}else{
			*( output.data + i ) = *( sample.data + i );
			sample.CDS_subtraction_indices.pop_back();
		}
	}
}
