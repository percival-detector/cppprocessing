/*
 * percival_ADC_decode_pipe.cpp
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */


#include "percival_processing.h"
#include "percival_parallel.h"
#include "tbb/pipeline.h"

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

class InputFilter : public tbb::filter{
private:
	const unsigned int grain_size;
	unsigned int offset;
	const unsigned int size;
	const percival_frame<unsigned short> sample;
	pair< unsigned short, float >* const input;
public:
	InputFilter(pair< unsigned short, float >* input, unsigned int grain_size):
				tbb::filter(/*is_serial=*/true),
				input(input),
				grain_size(grain_size),
				offset(0),
				size((input->sample).width * (input->sample).height)
	{}

	void* operator()(void*){
		if(offset < size){
			input->offset = offset;
			offset += grain_size;
			return input;
		}else{
			return NULL;
		}
	}
};



class ADC_decode_filter : public tbb::filter{
private:
	const unsigned int grain_size;
	percival_calib_params calib_params;
	percival_ADC_decode_p< percival_range_iterator_mock_p > ADC_decode_p;
	percival_range_iterator_mock_p range;
public:
	ADC_decode_filter(
			const percival_frame<unsigned short int> & src_frame,
			percival_frame<float> & des_frame,
			const percival_calib_params & calib_params,
			unsigned int grain_size):
				tbb::filter(/*is_serial=*/false),
				calib_params(calib_params),
				grain_size(grain_size),
				ADC_decode_p(src_frame, des_frame, calib_params),
				range(0,1)

	{}

	void* operator()(void* input){
		pair< unsigned short, float >* pairs = static_cast< pair<unsigned short, float>* >(input);
		unsigned int offset = pairs->offset;
		range.lower = offset;
		range.upper = grain_size + offset;
		ADC_decode_p(range);
		return NULL;
	}

};


void percival_ADC_decode_pf_combined_tbb_pipeline(
		const percival_frame<unsigned short int> & src_frame,
		percival_frame<float> & des_frame,
		const percival_calib_params & calib_params,
		unsigned int grain_size,
		bool store_gain)
{
	unsigned int max_tokens = 600;

	pair<unsigned short int, float> stream;
	stream.sample = src_frame;
	stream.dest = des_frame;
	stream.offset = 0;

	tbb::pipeline pipeline;

	InputFilter Input(&stream, grain_size);
	pipeline.add_filter( Input );

	ADC_decode_filter ADC_decode(src_frame, des_frame, calib_params, grain_size);
	pipeline.add_filter( ADC_decode );

	pipeline.run( max_tokens );

	pipeline.clear();
}


class InputFilter2 : public tbb::filter{
private:
	const unsigned int grain_size;
	unsigned int offset;
	const unsigned int array_size;
	frame_stack stack;
public:
	InputFilter2(frame_stack stack, unsigned int grain_size, unsigned int array_size):
				tbb::filter(/*is_serial=*/true),
				stack(stack),
				grain_size(grain_size),
				offset(0),
				array_size(array_size)
	{}

	void* operator()(void*){
		if(offset < array_size){
			stack.offset = offset;
			offset += grain_size;
			return &stack;
		}else{
			return NULL;
		}
	}
};



class ADC_decode_filter2 : public tbb::filter{
private:
	const unsigned int grain_size;
	const unsigned int NoOfPixels;
	percival_calib_params calib_params;
public:
	ADC_decode_filter2(const percival_calib_params & calib_params,
			unsigned int grain_size):
				tbb::filter(/*is_serial=*/false),
				calib_params(calib_params),
				grain_size(grain_size),
				NoOfPixels(calib_params.Gain_lookup_table1.width * calib_params.Gain_lookup_table1.height)
	{}

	void* operator()(void* input){
		frame_stack* stack = static_cast< frame_stack* >(input);
		unsigned int offset = stack->offset;
		percival_range_iterator_mock_p range(0, NoOfPixels);
		for(unsigned int i = offset; i < (grain_size + offset); ++i){
			percival_ADC_decode_p< percival_range_iterator_mock_p > ADC_decode_p (*(stack->sample + i), *(stack->output + i), calib_params);
			ADC_decode_p(range);
		}
		return NULL;
	}

};



void percival_ADC_decode_pf_combined_tbb_pipeline_stream(
		percival_frame<unsigned short int> * src_frame,
		percival_frame<float> * des_frame,
		const percival_calib_params & calib_params,
		unsigned int grain_size,
		unsigned int array_size)			/*in number of frames*/
{
	unsigned int max_tokens = 50;
	frame_stack stream;
	stream.sample = src_frame;
	stream.output = des_frame;
	stream.offset = 0;

	tbb::pipeline pipeline;

	InputFilter2 Input(stream, grain_size, array_size );
	pipeline.add_filter( Input );

	ADC_decode_filter2 ADC_decode(calib_params, grain_size);
	pipeline.add_filter( ADC_decode );

	pipeline.run( max_tokens );

	pipeline.clear();
}






