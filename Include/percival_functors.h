/*
 * percival_functors.h
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_FUNCTORS_H_
#define INCLUDE_PERCIVAL_FUNCTORS_H_

/*
 * This header is for Streaming SIMD Extensions (SSE)
 * SSE: cacheablity, Single Floating Point (SP, FP) operations
 *
 *  */
//#include "emmintrin.h"

/* Forward declarations */
//struct CDS_output;
//struct unit_ADC_calibration_output;
//struct unit_gain_multiplication_output;
//struct unit_CDS__output;
//
//struct percival_range_iterator_mock_p;
//class percival_unit_ADC_decode_p;
//class percival_unit_ADC_calibration_p
//class percival_unit_gain_multiplication_p
//class percival_unit_ADC_decode_pipe_p
//class percival_ADC_decode_p;
//class percival_algorithm_p;

/*
 * 	A non-parallel version of TBB's block range.
 * 	This is meant to mock block_range when TBB is absent.
 *
 */

struct percival_range_iterator_mock_p{	/*this object mocks the block_range object in tbb so that the syntax of my library can accommodate tbb library*/
	unsigned int lower;
	unsigned int upper;

	percival_range_iterator_mock_p(unsigned int begin, unsigned int size):
		lower(begin),
		upper(begin + size + 1)
	{}
	unsigned int begin() const {return lower;}
	unsigned int end() const  {return upper;}
};

/*
 * 	Unit functors
 * 		unit_ADC_decode
 * 		unit_ADC_calibration
 * 		unit_gain_multiplication
 *
 */

template<typename range_iterator>
class percival_unit_ADC_decode_p{
	unsigned short int * const input;
	unsigned short int * const Coarse;
	unsigned short int * const Fine;
	unsigned short int * const Gain;

public:
	percival_unit_ADC_decode_p (unsigned short int * a, unsigned short int *  b, unsigned short int *  c,unsigned short int *  d ):
		input(a), Coarse(b), Fine(c), Gain(d) {}

	void operator()( const range_iterator & r ) const{
		unsigned int end = r.end();
		unsigned short int pixel;
		for (unsigned int i=r.begin(); i<end; ++i ){
			pixel = *(input + i);
			*(Gain + i) = (pixel & 0x3);
			*(Fine + i) = (pixel & 0x3FC) >> 2;
			*(Coarse + i) = (pixel & 0x7c00) >> 10;
		}
	}
};

template<typename range_iterator>
class percival_unit_ADC_calibration_p{
	const percival_frame<unsigned short int> fine;
	const percival_frame<unsigned short int> coarse;
	percival_frame<float> output_frame;
	percival_calib_params calib_params;


public:
	percival_unit_ADC_calibration_p (
			const percival_frame<unsigned short int> & Coarse,
			const  percival_frame<unsigned short int> & Fine,
			percival_frame<float>& output_frame,
			const percival_calib_params & calib):
				fine(Fine),
				coarse(Coarse),
				output_frame(output_frame),
				calib_params(calib_params)
{}

	void operator()( const range_iterator & r ) const{

		unsigned int end = r.end();
		unsigned int begin = r.begin();
		/*defining reusable variables*/
		unsigned int row, position_in_calib_array, width, calib_data_width, coarseBits, fineBits;			/*28 bytes*/
		unsigned int col_counter, row_counter;		/*8 bytes*/
		float coarse_calibrated, fine_calibrated;		/*8 bytes*/
		unsigned short int * coarse, *fine;		/*8 bytes*/
		float* Gc, *Oc, *Gf,* Of, *output;		/*40 bytes. Pointers are of word size*/

		/* avoid repeated dereferencing*/
		Gc = calib_params.Gc.data;
		Oc = calib_params.Oc.data;
		Gf = calib_params.Gf.data;
		Of = calib_params.Of.data;
		output = output_frame.data;
		coarse = (this -> coarse).data;
		fine =  (this -> fine).data;
		width =  (this -> fine).width;
		calib_data_width = calib_params.Gc.width;

		row_counter = begin%width;
		col_counter = row_counter%7; row = begin/width;


		for (unsigned int i=r.begin(); i<end; ++i ){
			if( (col_counter^7) )
				col_counter++;
			else{
				col_counter = 0;
			}

			if( (row_counter^width) ){
				row_counter++;
			}else{
				row_counter = 0;
				row++;
			}

			position_in_calib_array = col_counter + row * calib_data_width;

			coarseBits = *(coarse + i);
			fineBits = *(fine + i);

			fine_calibrated = (fineBits - *(Of + position_in_calib_array)) * *(Gf + position_in_calib_array);
			coarse_calibrated = (*(Oc + position_in_calib_array) - coarseBits) * *(Gc + position_in_calib_array);

			*(output+i) = coarse_calibrated - fine_calibrated;
		}
	}
};


template<typename range_iterator>
class percival_unit_gain_multiplication_p{
	percival_frame<float> output;
	percival_frame<float> calibrated;
	percival_frame<unsigned short int> src_frame;
	percival_calib_params calib_params;

public:
	percival_unit_gain_multiplication_p (
			const percival_frame<unsigned short int> & src_frame,
			const percival_frame<float> & calibrated,
			percival_frame<float> & output,
			const percival_calib_params & calib):
				src_frame(src_frame),
				calibrated(calibrated),
				output(output),
				calib_params(calib)
{}

	void operator()( const range_iterator & r ) const{
		unsigned int end = r.end();
		unsigned short int gain;
		float gain_factor;

		float* G1 = calib_params.Gain_lookup_table1.data;
		float* G2 = calib_params.Gain_lookup_table2.data;
		float* G3 = calib_params.Gain_lookup_table3.data;
		float* G4 = calib_params.Gain_lookup_table4.data;
		float* output = (this -> output).data;
		float* calibrated = (this -> calibrated).data;
		unsigned short int* src_frame = (this -> src_frame).data;

		for (unsigned int i=r.begin(); i<end; ++i ){
			gain =  *(src_frame + i) & 0x3;

			switch(gain){
			case 0b00:
				gain_factor = *(G1 + i);
				break;
			case 0b01:
				gain_factor = *(G2 + i);
				break;
			case 0b10:
				gain_factor = *(G3 + i);
				break;
			case 0b11:
				gain_factor = *(G4 + i);
				break;
			default:
				throw datatype_exception("Invalid gain bit detected.");
			}
			*(output + i) = *(calibrated + i) * gain_factor;
		}
	}
};

/*
 *  A functor that groups three algorithms.
 *  Mainly used in parallel_for.
 *  Consider removing
 *
 */

template<typename range_iterator>
class percival_ADC_decode_pipe_p{
	const percival_frame<unsigned short int> input;
	percival_frame<float> output;
	percival_calib_params calib_params;

	percival_unit_ADC_decode_p< range_iterator > unit_ADC_decode_p;
	percival_unit_ADC_calibration_p< range_iterator > unit_ADC_calibration_p;
	percival_unit_gain_multiplication_p< range_iterator > unit_gain_multiplication_p;


public:
	percival_ADC_decode_pipe_p (
			const percival_frame<unsigned short int>& input ,
			percival_frame<float> & output,
			const percival_calib_params & calib_params,
			percival_frame<unsigned short int> &gain,
			percival_frame<unsigned short int>& fine,
			percival_frame<unsigned short int>& coarse,
			percival_frame<float> &calibrated):
				input(input),
				output(output),
				calib_params(calib_params),
				unit_ADC_decode_p(input.data, coarse.data, fine.data, gain.data),
				unit_ADC_calibration_p(coarse, fine, output, calib_params),
				unit_gain_multiplication_p(input,calibrated,output,calib_params)
{}

	void operator()( const range_iterator & r ) const
	{
		unit_ADC_decode_p(r);
		unit_ADC_calibration_p(r);
		unit_gain_multiplication_p(r);
	}
};

/*
 *  A combined ADC_decode function used for parallel_for algorithms
 *  Consider removing
 *
 */


template<typename range_iterator>
class percival_ADC_decode_p{
	const percival_frame<unsigned short int> src_frame;
	percival_frame<float> des_frame;
	percival_calib_params calib_params;

public:
	percival_ADC_decode_p (
			const percival_frame<unsigned short int> & src_frame,
			percival_frame<float> & des_frame,
			const percival_calib_params & calib_params):
				src_frame(src_frame),
				des_frame(des_frame),
				calib_params(calib_params)
{}

	void operator()( const range_iterator & r ) const
	{
		//calibration parameters
		const unsigned int calib_data_width = calib_params.Gc.width;
		const float VinMax=1.43;
		//these two values are from February test data from Hazem. should be changed if calibration data changes
		//		const float FMax = 222;
		//		const float CMax = 26;
		//		const float factor = 5772;// 222 * 26;
		//		const float inverseVinMax = 0.6993007; //1/1.43
		float gain_factor = 1;

		/*Allocate memory for reusable variables*/
		short unsigned int gain, fineBits, coarseBits;
		short unsigned int col, row, position_in_calib_array;

		short unsigned int pixel;
		unsigned int width;

		width = src_frame.width;

		float* Gc = calib_params.Gc.data;
		float* Oc = calib_params.Oc.data;
		float* Gf = calib_params.Gf.data;
		float* Of = calib_params.Of.data;

		float* G1 = calib_params.Gain_lookup_table1.data;
		float* G2 = calib_params.Gain_lookup_table2.data;
		float* G3 = calib_params.Gain_lookup_table3.data;
		float* G4 = calib_params.Gain_lookup_table4.data;

		float* output = des_frame.data;
		//		__m128* vec = new __m128[4];
		//		float* output_array = new float[4];
		//algorithm
		unsigned int end = r.end();
		unsigned int begin = r.begin();
		unsigned int col_counter, row_counter;
		row_counter = begin%width;
		col_counter = row_counter%7; row = begin/width;
		float coarse_calibrated, fine_calibrated;

		//		unsigned int end = r.end() - r.end() % 16;
		//		unsigned int offset = 16 - r.begin() % 16;
		//		unsigned int begin =  offset + r.begin();

		/*start of for loop*/
		for(unsigned int i = begin; i < end ; ++i){	//int i is sufficient
			//			for(unsigned int k = 0; k < 16; k +=4){
			//				for(unsigned int j = 0; j < 4; ++j){	/*use streaming SIMD to vectorize floating point computation*/
			/*
			 * minimising access */
			pixel = *(src_frame.data + i);

			/*
			 * uses two bitwise AND, two negations, two additions, in exchange for
			 * two modulus, one division,  6 cycles VS 30 cycles
			 *
			 */

			/*Use binary masks instead*/
			gain = pixel & 0x0003;
			fineBits = (pixel & 0x3FC) >> 2;
			coarseBits = (pixel & 0x7c00) >> 10;


			position_in_calib_array = col_counter + row * calib_data_width;

			//					col = i % width;			//0 ~ frame_width - 1
			//					row = (i - col) / width;
			//					position_in_calib_array = (col % 7) + (row * calib_data_width); //7 from 7 ADCs. todo code this in config.


			switch(gain){
			case 0b00:
				gain_factor = *(G1 + i);
				break;
			case 0b01:
				gain_factor = *(G2 + i);
				break;
			case 0b10:
				gain_factor = *(G3 + i);
				break;
			case 0b11:
				gain_factor = *(G4 + i);
				break;
			default:
				throw datatype_exception("Invalid gain bit detected.");
			}

			coarse_calibrated = (*(Oc + position_in_calib_array) - coarseBits) * *(Gc + position_in_calib_array);
			fine_calibrated = (fineBits - *(Of + position_in_calib_array)) * *(Gf + position_in_calib_array);

			*(output+i) = gain_factor * (coarse_calibrated - fine_calibrated);

			//					*(output + i)	= (float)gain_factor *
			//							(		/*this factor can be absorbed into gain and needs not be here.*/
			//									(
			//											VinMax -
			//											(
			//													/*this can be done permanently to the calibration parameter and needs not be here*/
			//													(
			//															(
			//																	(*(Oc + position_in_calib_array) - (fineBits - (unsigned short int)1)) / *(Gc + position_in_calib_array)		//In hazem's code coarseBits == FineArr, fineBits == CoarseArr
			//															)
			//															+		(
			//																	(coarseBits - *(Of + position_in_calib_array)) / *(Gf + position_in_calib_array)
			//															)
			//													)
			//											)
			//									)
			//							);
			//				}

			/*writing data to memory without polluting the cache*/
			//			vec[k/4] = _mm_load_ps(output_array);
			/*
			 * uses two bitwise AND, two negations, two additions, in exchange for
			 * two modulus, one division,  6 cycles VS 30 cycles
			 *
			 */
			if( (col_counter^7) )
				col_counter++;
			else{
				col_counter = 0;
			}

			if( (row_counter^width) ){
				row_counter++;
			}else{
				row_counter = 0;
				row++;
			}

		}
		/*The bus is capable of transferring 64Bytes each time. Thus accumulate four sets of four data and transfer in one go*/
		//		_mm_stream_ps( (output + i), vec[0]);
		//		_mm_stream_ps((output+i + 4), vec[1]);
		//		_mm_stream_ps((output+i + 8), vec[2]);
		//		_mm_stream_ps((output+i + 12), vec[3]);

	}
};



/*===============================================================================================================================*/
/*
 * 	Ultimate combined giant huge functor
 *
 */

/*
 *  Possible choices of algorithms
 *  	step 1: unit_ADC_decode
 *  	step 2: unit_ADC_calibration
 *  	step 3: unit_gain_multiplication
 *  	step 4: unit_CDS_subtraction
 *  	step 5: unit_ADU_to_electron
 *
 * 	The following enum lists the possible EXIT point from the above steps.
 * 	e.g. unit_CDS_subtraction implies that step 1 - 4 will be performed.
 *
 */

enum algorithm_pipeline{
	unit_ADC_calibrated,
	unit_gain_multiplication,
	unit_CDS_subtraction,
	unit_ADU_to_electron
};	/* unit_ADC_decode is not supported since the original unit_decode function will outperform */

struct CDS_output{
	const static algorithm_pipeline type = unit_CDS_subtraction;
	percival_frame<unsigned short int> input_reset;
	percival_frame<unsigned short int> input_sample;
	percival_frame<float> output;
};

struct unit_ADC_calibration_output{
	const static algorithm_pipeline type = unit_ADC_calibrated;
	percival_frame<unsigned short int> input;
	percival_frame<float> output;
};

struct unit_gain_multiplication_output{
	const static algorithm_pipeline type = unit_gain_multiplication;
	percival_frame<unsigned short int> input;
	percival_frame<float> output;
};


/*
 * 	Functor used by TBB pipeline
 *
 *
 */

template<typename input_type, typename range_iterator>
class percival_algorithm_p{
	input_type input;
	const percival_calib_params calib;
public:
	percival_algorithm_p(input_type & input, const percival_calib_params & calib ):
		input( input ),
		calib( calib )
{}

	void operator()(range_iterator & r)
	{
		/*
		 * array iterators
		 */
		unsigned int begin = r.begin();
		unsigned int end = r.end();
		unsigned short int pixel;
		algorithm_pipeline pipeline_type = input.type;

		/*listing all variables*/
		float arr[2] = {0,0};
		unsigned short int gain, coarseBits, fineBits;
		unsigned short int count,current_count, increment;
		unsigned short int *data, *sample_frame, *reset_frame;
		unsigned int row, col_counter, row_counter, width, calib_data_width, position_in_calib_array;
		float coarse_calibrated, fine_calibrated, gain_factor, result;
		float *Oc, *Gc, *Of, *Gf, *G1, *G2, *G3, *G4, *output;
		float* ADU_to_electron;


		switch(pipeline_type){
		case unit_ADC_calibrated:
			reset_frame = NULL;
			increment = 0;
			break;
		case unit_gain_multiplication:
			reset_frame = NULL;
			increment = 0;
			break;
		case unit_CDS_subtraction:
			reset_frame = input.input_reset.data;
			increment = 0;
			break;
		case unit_ADU_to_electron:
			reset_frame = input.input_reset.data;
			increment = 1;
			break;
		default:
			/*throw appropriately here or assert.*/
			break;
		}

		/*Initialising variables needed*/
		sample_frame = input.input_sample.data;
		output = input.output.data;

		calib_data_width = calib.Gc.width;
		width = input.input_sample.width;

		Gc = calib.Gc.data;
		Oc = calib.Oc.data;
		Gf = calib.Gf.data;
		Of = calib.Of.data;

		G1 = calib.Gain_lookup_table1.data;
		G2 = calib.Gain_lookup_table2.data;
		G3 = calib.Gain_lookup_table3.data;
		G4 = calib.Gain_lookup_table4.data;

		/*Used to correlate a pixel in sample with a pixel in calibration array*/
		row = begin / width;
		row_counter = begin%width;
		col_counter = row_counter%7;

		/*loop*/
		for(unsigned int i = begin; i < end; ++i){

			/* Initialising variables in loop appropriately */
			arr[0] = 0; arr[1] = 0;
			data = sample_frame;

			/*
			 *  The count and current_count are two counters.
			 *  They used to track whether the sample or reset data is being worked on by the while loop.
			 */
			count = 0;current_count = 0;

			position_in_calib_array = col_counter + row * calib_data_width;

			/*
			 *  While loop to work on sample and reset frame.
			 * 	The specification required CDS_subtraction to be applied only to pixels with gain == 0b00
			 * */
			do{
				pixel = *(data + i);
				/* unit_ADC_decode */
				gain = pixel & 0x0003;
				fineBits = (pixel & 0x3FC) >> 2;
				coarseBits = (pixel & 0x7c00) >> 10;

				/* unit_ADC_calib */
				coarse_calibrated = (*(Oc + position_in_calib_array) - coarseBits) * *(Gc + position_in_calib_array);
				fine_calibrated = (fineBits - *(Of + position_in_calib_array)) * *(Gf + position_in_calib_array);

				if(pipeline_type != unit_ADC_calibrated){
					/* unit_ADC_gain_multiplication */
					switch(gain){
					case 0b00:
						gain_factor = *(G1 + i);
						count = 1;
						data = reset_frame;
						break;
					case 0b01:
						gain_factor = *(G2 + i);
						break;
					case 0b10:
						gain_factor = *(G3 + i);
						break;
					case 0b11:
						gain_factor = *(G4 + i);
						break;
					default:
						throw datatype_exception("Invalid gain bit detected.");
					}
				}

				/*
				 * store calibrated sample in slot 0 and calibrated reset in slot 1.
				 * slot 1 is zero if CDS_subtraction is not needed
				 * */
				arr[current_count] = gain_factor * (coarse_calibrated - fine_calibrated);

				/* Updating current count after each iteration */
				current_count = (current_count+1)&0x1;

			}while( count&current_count );



			/* subtraction */
			result = arr[0] - arr[1];

			/* Apply scaling if neede */
			/* flat field correction and dark image subtraction can be applied here too. */
			if(pipeline_type == unit_ADU_to_electron){
				result *= *(ADU_to_electron + i);
			}

			/*writing to memory*/
			*(output+i) = result;

			/*
			 * Correlating a pixel in image frame with a pixel in calibration data.
			 */

			if( (col_counter^7) )			/* Bitwise XOR operator ^ is equivalent to !=. Used here for performance.*/
				col_counter++;
			else
				col_counter = 0;

			if( (row_counter^width) )
				row_counter++;
			else{
				row_counter = 0;
				row++;
			}

		}
	}
};





#endif /* INCLUDE_PERCIVAL_FUNCTORS_H_ */
