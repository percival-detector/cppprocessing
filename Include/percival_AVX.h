/*
 * percival_AVX.h
 *
 *  Created on: 7 Sep 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_AVX_H_
#define INCLUDE_PERCIVAL_AVX_H_

template<typename T>
percival_frame<T> percival_align_memory(percival_frame_mem<T> & input, percival_frame_mem<T> & output_buffer, unsigned int stride, unsigned int boundary_size){
	/* Check size */
	unsigned int input_NoOfPixels = input.width * input.height;
	unsigned int output_NoOfPixels = output_buffer.width * output_buffer.height;
	if((input.width % stride))
		throw dataspace_exception("percival_align_memory: stride does not divide width.");

	if( ((input_NoOfPixels/stride))*boundary_size > output_NoOfPixels )
		throw dataspace_exception("percival_align_memory: spare space is insufficient.");

	if( reinterpret_cast<std::size_t>(input.data)% (sizeof(T) * boundary_size))
		throw dataspace_exception("percival_align_memory: first element of input data is not aligned.");
	percival_frame<T> output;
	output.height = input.height;
	output.width = (input.width / stride ) * boundary_size;

	for(unsigned int i = 0; i < input.width / stride; i ++){
		for (unsigned int j = 0; j < boundary_size; j ++){
			if(j < stride){
				*(output_buffer.data + i * boundary_size + j) = *(input.data + i * stride + j);
			}else{
				*(output_buffer.data + i * boundary_size + j) = 0;
			}
		}
	}

	output = output_buffer;
	return output;
}





#endif /* INCLUDE_PERCIVAL_AVX_H_ */
