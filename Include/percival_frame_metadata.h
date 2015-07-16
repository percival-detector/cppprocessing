/*
 * percival_frame_metadata.h
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_FRAME_METADATA_H_
#define INCLUDE_PERCIVAL_FRAME_METADATA_H_

struct percival_frame_metadata{
	char path_name[];
	int frame_number;
	int width, height;
	float coarse_gain, coarse_offset;
	float fine_gain, fine_offset;
};

#endif /* INCLUDE_PERCIVAL_FRAME_METADATA_H_ */
