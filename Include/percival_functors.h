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

/*the following classes are created for tbb*/


struct percival_range_iterator_mock_p{	/*this object mocks the block_range object in tbb so that the syntax of my library can accommodate tbb library*/
	unsigned int lower;
	unsigned int upper;

	percival_range_iterator_mock_p(int begin, int size):
		lower(begin),
		upper(begin + size + 1)
	{}
	unsigned int begin() const {return lower;}
	unsigned int end() const  {return upper;}
};

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
		unsigned int pixel;
		for (unsigned int i=r.begin(); i!=end; ++i ){
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

		/*defining reusable variables*/
		unsigned int col, row, position_in_calib_array, width, calib_data_width, coarseBits, fineBits;			/*28 bytes*/
		unsigned int col_counter, row_counter;		/*8 bytes*/
		float Gc_at_this_pixel, Oc_at_this_pixel, Gf_at_this_pixel, Of_at_this_pixel;		/*16 bytes*/
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

		for (unsigned int i=r.begin(); i!=end; ++i ){

			if( (col_counter&7) )
				col_counter = 0;
			else{
				col_counter++;
			}

			if( (row_counter&width) ){
				row_counter = 0;
				row++;}
			else{
				row_counter++;
			}

			position_in_calib_array = col_counter + row * calib_data_width;

			coarseBits = *(coarse + i);
			fineBits = *(fine + i);

			coarse_calibrated = (*(Oc + position_in_calib_array) - coarseBits) * *(Gc + position_in_calib_array);
			fine_calibrated = (fineBits - *(Of + position_in_calib_array)) * *(Gf + position_in_calib_array);

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

		for (unsigned int i=r.begin(); i!=end; ++i ){
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
		col_counter = (begin%width)%7; row_counter = begin/width;
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

			/*
			 * uses two bitwise AND, two negations, two additions, in exchange for
			 * two modulus, one division,  6 cycles VS 30 cycles
			 *
			 */
			if( (col_counter&7) )
				col_counter = 0;
			else{
				col_counter++;
			}

			if( (row_counter&width) ){
				row_counter = 0;
				row++;}
			else{
				row_counter++;
			}

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
		}
		/*The bus is capable of transferring 64Bytes each time. Thus accumulate four sets of four data and transfer in one go*/
		//		_mm_stream_ps( (output + i), vec[0]);
		//		_mm_stream_ps((output+i + 4), vec[1]);
		//		_mm_stream_ps((output+i + 8), vec[2]);
		//		_mm_stream_ps((output+i + 12), vec[3]);
	}
};

#endif /* INCLUDE_PERCIVAL_FUNCTORS_H_ */
