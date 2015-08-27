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
/*===============================================================================================================================*/
/* Pipeline design mark one*/
/*
 *
 *	A few rows as a token.
 *	the number of rows should be a factor of 3717 = ( 3 * 3 * 7 * 59)
 *  Possible choices are 1,3,7,9,21,59,63,177,413,531,1239,3717
 *
 *
 */


/*
 *  Generate a stream of data for later stages
 */
class InputFilter : public tbb::filter{
private:

	const unsigned int grain_size;
	unsigned int offset;	/*offset within the frame*/
	const unsigned int size;	/* image size to issue an end of frame */
	const percival_frame<unsigned short> sample;
	pair< unsigned short, float >* const input;	/* a pair of input and output frames*/

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
			input->offset = offset;			/*seems that I am lucky here*/
			offset += grain_size;
			return input;

			/* There was no memory allocation for the pair struct.
			 *  consequently all threads are modifying the same offset variable in pair.
			 *  Fortunately, since these threads were created in sequence, and offset in pair is accessed only once, each thread read offset in sequence.
			 *  this thus does not produce an error.
			 *
			 *  */
		}else{
			return NULL;
		}
	}
};

/*
 * ADC_decode stage filter
 */

class ADC_decode_filter : public tbb::filter{
private:

	const unsigned int grain_size;		/* size of loop */
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
		/* range to loop over */
		range.lower = offset;
		range.upper = grain_size + offset - 1;
		/*running the algorithm*/
		ADC_decode_p(range);
		return NULL;	/*return to next stage if needed*/
	}
};


void percival_ADC_decode_pf_combined_tbb_pipeline(
		const percival_frame<unsigned short int> & src_frame,
		percival_frame<float> & des_frame,
		const percival_calib_params & calib_params,
		unsigned int grain_size,
		bool store_gain)
{
	/* Maximum number of tokens in existence at one point in time */
	unsigned int max_tokens = 600;
	/* generating the input */
	pair<unsigned short int, float> stream;
	stream.sample = src_frame;
	stream.dest = des_frame;
	stream.offset = 0;
	/* starting a pipeline */
	tbb::pipeline pipeline;

	InputFilter Input(&stream, grain_size);
	pipeline.add_filter( Input );

	ADC_decode_filter ADC_decode(src_frame, des_frame, calib_params, grain_size);
	pipeline.add_filter( ADC_decode );

	pipeline.run( max_tokens );

	pipeline.clear();
}


/*===============================================================================================================================*/
/* Pipeline design mark two*/
/*
 *
 * A set of frames as a token
 *
 */

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

/*===============================================================================================================================*/
/* Pipeline design mark three*/
/*
 *  Generate a stream of data for later stages
 */
class InputFilter1 : public tbb::filter{
private:

	unsigned int offset;	/*offset within the frame*/
	unsigned int* initial_ptr;
	const unsigned int grain_size;
	const unsigned int size;	/* image size to issue an end of frame */
	const unsigned int max_number_of_tokens;
	unsigned int current_index;

public:
	InputFilter1(
			unsigned int* initial_ptr,
			unsigned int grain_size,
			unsigned int frame_size,
			unsigned int max_number_of_tokens
			):
				tbb::filter(/*is_serial=*/true),
				initial_ptr( initial_ptr ),
				grain_size( grain_size ),
				offset( 0 ),
				size( frame_size ),
				max_number_of_tokens(max_number_of_tokens),
				current_index( 0 )
	{ *initial_ptr = offset;}

	void* operator()(void*){
		if(offset < size){
			*(initial_ptr + current_index) = offset;
			current_index++;
			if(current_index == max_number_of_tokens){
				current_index -= (max_number_of_tokens);
			}
			offset += grain_size;
			return initial_ptr + current_index - 1;
		}else{
			return NULL;
		}
	}
};

/*
 * ADC_decode stage filter
 */

class ADC_decode_filter1 : public tbb::filter{
private:

	const unsigned int grain_size;		/* size of loop */
	percival_calib_params calib_params;

	percival_ADC_decode_p< percival_range_iterator_mock_p > ADC_decode_p;
	percival_range_iterator_mock_p range;

public:
	ADC_decode_filter1(
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
		unsigned int * offset_ptr = static_cast < unsigned int* >(input);
		unsigned int offset = *offset_ptr;
		/* range to loop over */
		range.lower = offset;
		range.upper = grain_size + offset;
		/*running the algorithm*/
		ADC_decode_p(range);
		return NULL;	/*return to next stage if needed*/
	}
};


//class unit_ADC_decode_filter1 : public tbb::filter{
//private:
//
//	const unsigned int grain_size;		/* size of loop */
//	percival_calib_params calib_params;
//
//	percival_unit_ADC_decode_p< percival_range_iterator_mock_p > unit_ADC_decode_p;
//	percival_range_iterator_mock_p range;
//
//public:
//	unit_ADC_decode_filter1(
//			const percival_frame<unsigned short int> & src_frame,
//			percival_frame<float> & des_frame,
//			const percival_calib_params & calib_params,
//			unsigned int grain_size):
//
//				tbb::filter(/*is_serial=*/false),
//				calib_params(calib_params),
//				grain_size(grain_size),
//				unit_ADC_decode_p(src_frame, des_frame, calib_params),
//				range(0,1)
//	{}
//
//	void* operator()(void* input){
//		unsigned int * offset_ptr = static_cast < unsigned int* >(input);
//		unsigned int offset = *offset_ptr;
//		/* range to loop over */
//		range.lower = offset;
//		range.upper = grain_size + offset;
//		/*running the algorithm*/
//		unit_ADC_decode_p(range);
//		return NULL;	/*return to next stage if needed*/
//	}
//};


void percival_ADC_decode_pf_combined_tbb_pipeline1(
		const percival_frame<unsigned short int> & src_frame,
		percival_frame<float> & des_frame,
		const percival_calib_params & calib_params,
		unsigned int grain_size,
		bool store_gain)
{
	/* Maximum number of tokens in existence at one point in time */
	unsigned int max_tokens = 600;

	/* starting a pipeline */
	tbb::pipeline pipeline;
	unsigned int offset_arr [max_tokens];
	unsigned int *offset_ptr = & offset_arr[0];

	InputFilter1 Input(offset_ptr, grain_size, src_frame.height * src_frame.width, max_tokens);
	pipeline.add_filter( Input );

	ADC_decode_filter1 ADC_decode(src_frame, des_frame, calib_params, grain_size);
	pipeline.add_filter( ADC_decode );

	pipeline.run( max_tokens );

	pipeline.clear();
}
