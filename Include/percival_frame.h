/*
 * percival_frame.h
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#ifndef PERCIVAL_FRAME_H_
#define PERCIVAL_FRAME_H_

template<typename T>
class percival_frame{
public:
	int width, height;
	T* data;

	void set_frame_size(int h, int w)
	{
		delete [] data;
		width = w;
		height = h;
		data = new T[width * height];
	}

	percival_frame(){data = new T[1]; set_frame_size(1,1);}
	percival_frame(int x, int y){data = new T[1]; set_frame_size(x,y);}
	~percival_frame(){delete [] data;}
};

#endif /* PERCIVAL_FRAME_H_ */
