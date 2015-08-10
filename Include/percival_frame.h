/*
 * percival_frame.h
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#ifndef PERCIVAL_FRAME_H_
#define PERCIVAL_FRAME_H_

#include "percival_processing.h"
#include <vector>
template<typename T>
class percival_frame{
public:
	/*
	 * typically width, height ~= 10,000, use signed short is sufficient. width * height ~= 100,000,000, 27 bit unsigned int, int is sufficient.
	 * if number of pixels is to be assumed less than range of int (INT_MIN - (2^31 -1)), width and height should be at most 2^15
	 * thus use int width, height to allow multiplications etc
	 * use a check in method set_frame_size to set width and height less than 2^15
	 *
	 * Evetually I still decide to use unsigned int for width and height, and unsigned int for all loop using width * height. This allows a larger range of widht and height.
	 * Width * height is guaranteed to be less than the range of unsigned int (0x7fffffff)
	 */
	unsigned int width, height;			//
	T* data;
	//add an offset if splitting up the image is needed
	std::vector<int> CDS_subtraction_indices;		//stores pixel indices requiring CDS_substraction

	void set_frame_size(unsigned int h, unsigned int w)
	{
		if((unsigned int)(h * w) > 0x7fffffff ){
			throw datatype_exception{"Image size overflows. Should be less than 32768 pixels in each dimension."};
		}else if(h < 0 || w < 0 || h * w < 0){
			throw datatype_exception{"Image size overflows. Should be greater than or equal to 0 and less than 32768 pixels in each dimension."};
		}else{
			delete [] data;
			width = w;
			height = h;
			data = new T[width * height];
		}
	}

	percival_frame(){data = new T[1]; set_frame_size(1,1);}
	percival_frame(unsigned int x, int y){data = new T[1]; set_frame_size(x,y);}
	~percival_frame(){delete [] data;}
};

#endif /* PERCIVAL_FRAME_H_ */
//TODO: check overflow problem. Add test for it!!!!
