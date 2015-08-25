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
#include "emmintrin.h"

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
		for (unsigned int i=r.begin(); i!=end; ++i ){
			*(Gain + i) = *(input + i) % 0x0004;
			*(Fine + i) = (*(input + i)  >> 2 ) %0x100;
			*(Coarse + i) = (*(input + i) >> 10) %0x20;
		}
	}
};

template<typename range_iterator>
class percival_unit_ADC_calibration_p{
	unsigned int width, height, calib_width, calib_height;
	float* Gc, *Oc,* Gf, *Of, *output;
	unsigned short int* coarse, *fine;

public:
	percival_unit_ADC_calibration_p (const percival_frame<unsigned short int> & Coarse ,
			const  percival_frame<unsigned short int> & Fine,
			percival_frame<float> & output,
			const percival_calib_params & calib ):
				width(Coarse.width),  height(Coarse.height),  calib_width(calib.Gc.width),  calib_height(calib.Gc.height),
				Gc(calib.Gc.data), Oc(calib.Oc.data), Gf(calib.Gf.data), Of(calib.Of.data), output(output.data),
				coarse(Coarse.data), fine(Fine.data)
{}

	void operator()( const range_iterator & r ) const{
		unsigned int end = r.end();

		/*defining reusable variables*/
		unsigned int col, row, position_in_calib_array, width, calib_width;
		float Gc_at_this_pixel, Oc_at_this_pixel, Gf_at_this_pixel, Of_at_this_pixel;
		unsigned short int * coarse, *fine;
		float* Gc, *Oc, *Gf,* Of, *output;
		/* avoid repeated dereferencing*/
		Gc = this -> Gc;
		Oc = this -> Oc;
		Gf = this -> Gf;
		Of = this -> Of;
		output = this -> output;
		coarse = this -> coarse;
		fine = this -> fine;
		width = this->width;
		calib_width = this -> calib_width;
		/*constants used in calculation*/
		const float VinMax=1.43;
		//these two values are from February test data from Hazem. should be changed if calibration data changes
		const float FMax = 222;
		const float CMax = 26;
		for (unsigned int i=r.begin(); i!=end; ++i ){
			col = i % width;			//0 ~ frame_width - 1
			row = (i - col) / width;
			position_in_calib_array = (col % 7) + (row * calib_width); //7 from 7 ADCs. todo code this in config.

			Gc_at_this_pixel = *(Gc + position_in_calib_array);
			Oc_at_this_pixel = *(Oc + position_in_calib_array);
			Gf_at_this_pixel = *(Gf + position_in_calib_array);
			Of_at_this_pixel = *(Of + position_in_calib_array);

			*(output + i)	= (float)
									(FMax * CMax *
											(
													1.0-
													(
															(1.0/VinMax)*
															(
																	(
																			(Oc_at_this_pixel - *(fine + i) - 1.0) / Gc_at_this_pixel		//In hazem's code coarseBits == FineArr, fineBits == CoarseArr
																	)
																	+		(
																			(*(coarse + i) - Of_at_this_pixel) / Gf_at_this_pixel
																	)
															)
													)
											)
									);

		}
	}
};


template<typename range_iterator>
class percival_unit_gain_multiplication_p{
	unsigned short int* const src_frame;
	float* const calibrated;
	float* const output;
	float* G1;
	float* G2;
	float* G3;
	float* G4;

public:
	percival_unit_gain_multiplication_p (const percival_frame<unsigned short int> & src_frame,
			const percival_frame<float> & calibrated,
			percival_frame<float> & output,
			const percival_calib_params & calib_params):
				src_frame(src_frame.data),
				calibrated(calibrated.data),
				output(output.data),
				G1 ( calib_params.Gain_lookup_table1.data),
				G2 ( calib_params.Gain_lookup_table2.data),
				G3 ( calib_params.Gain_lookup_table3.data),
				G4 ( calib_params.Gain_lookup_table4.data) {}

	void operator()( const range_iterator & r ) const{
		unsigned int end = r.end();
		unsigned short int gain;
		float gain_factor;

		float* G1 = this -> G1;
		float* G2 = this -> G2;
		float* G3 = this -> G3;
		float* G4 = this -> G4;

		for (unsigned int i=r.begin(); i!=end; ++i ){
			gain =  *(src_frame + i) % 0b100;

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
		__m128* vec = new __m128[4];
		float* output_array = new float[4];
		//algorithm
		unsigned int end = r.end() - r.end() % 16;
		unsigned int offset = 16 - r.begin() % 16;
		unsigned int begin =  offset + r.begin();

		/*start of for loop*/
		for(unsigned int i = begin; i < end ; i+=16){	//int i is sufficient
			for(unsigned int k = 0; k < 16; k +=4){
				for(unsigned int j = 0; j < 4; ++j){	/*use streaming SIMD to vectorize floating point computation*/
					/*
					 * minimising access */
					pixel = *(src_frame.data + i + j + k);

					/*Use binary masks instead*/
					gain = pixel & 0x0003;
					fineBits = (pixel & 0x07fc) >> 2;
					coarseBits = (pixel & 0x7c00) >> 10;

					//		//converting from linear representation to 2D representation. To speed up take modulo no of calibration pixels.
					col = i % width;			//0 ~ frame_width - 1
					row = (i - col) / width;	/*todo think of a way to convert this division to a multiplication*/
					position_in_calib_array = (col % 7) + (row * calib_data_width); //7 from 7 ADCs. todo code this in config.
					//TODO:Note that the precision of these numbers is not great. noticeable to 0.0001

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

					*(output_array + j)	= (float)gain_factor *
							(		/*this factor can be absorbed into gain and needs not be here.*/
									(
											VinMax -
											(
													/*this can be done permanently to the calibration parameter and needs not be here*/
													(
															(
																	(*(Oc + position_in_calib_array) - (fineBits - (unsigned short int)1)) / *(Gc + position_in_calib_array)		//In hazem's code coarseBits == FineArr, fineBits == CoarseArr
															)
															+		(
																	(coarseBits - *(Of + position_in_calib_array)) / *(Gf + position_in_calib_array)
															)
													)
											)
									)
							);
				}

			/*writing data to memory without polluting the cache*/
			vec[k/4] = _mm_load_ps(output_array);
		}
		/*The bus is capable of transferring 64Bytes each time. Thus accumulate four sets of four data and transfer in one go*/
		_mm_stream_ps( (output + i), vec[0]);
		_mm_stream_ps((output+i + 4), vec[1]);
		_mm_stream_ps((output+i + 8), vec[2]);
		_mm_stream_ps((output+i + 12), vec[3]);

	}
}
};


#endif /* INCLUDE_PERCIVAL_FUNCTORS_H_ */
