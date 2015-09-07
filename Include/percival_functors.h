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

// get AVX intrinsics
#ifdef __AVX__
#endif
#include <immintrin.h>
#include <emmintrin.h>


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

/*===============================================================================================================================*/
/*
 * 	Ultimate combined giant huge functor
 *	AVX version
 */

struct head_to_CDS{
	percival_frame<unsigned short int> input_reset;
	percival_frame<unsigned short int> input_sample;
	percival_frame<float> output;
};

/*
 * 	Functor used by TBB pipeline
 *
 *
 */

template<typename input_type, typename range_iterator>
class percival_algorithm_avx{
	input_type input;
	const percival_calib_params calib;
public:
	percival_algorithm_avx(input_type & input, const percival_calib_params & calib ):
		input( input ),
		calib( calib )
{}

	void operator()(range_iterator & r) const
	{
		/*
		 * array iterators
		 */
		unsigned int begin = r.begin();
		unsigned int end = r.end();

		/*listing all variables*/
		unsigned short int *data;
		unsigned int row, row_counter, col_counter, width, calib_data_width, position_in_calib_array;

		/* constants */
		float *Oc, *Gc, *Of, *Gf, *G1, *G2, *G3, *G4;
		float* ADU_to_electron;

		short unsigned int data0, data1, data2, data3, data4, data5, data6;
		short unsigned int *sample_frame, *reset_frame;
		float  *output;

		/* 10 AVX arrays, 16 registers */
		__m256 Gc_ymm, Oc_ymm, Gf_ymm, Of_ymm, ADU_2e_conv_ymm;
		__m256 fine_ymm, coarse_ymm, gain_ymm ;
		__m256 gain_mask_1_ymm, gain_mask_2_ymm, gain_mask_3_ymm, gain_mask_4_ymm;
		__m256 gain_table_1_ymm, gain_table_2_ymm, gain_table_3_ymm, gain_table_4_ymm;
		__m256 gain_factor_ymm, result_ymm, sample_result_ymm, reset_result_ymm, final_result_ymm;
		__m256 tmp_ymm, tmp_ymm0, tmp_ymm1, tmp_ymm2, tmp_ymm3;

		__m256 const_0x03_ymm = _mm256_set1_ps ( 0x03 );
		__m256 const_0x02_ymm = _mm256_set1_ps ( 0x02 );
		__m256 const_0x01_ymm = _mm256_set_ps ( 0, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 );
		__m256i const_0x01_int_ymm = _mm256_set_epi32 ( 0, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01);
		__m256 const_0_ymm = _mm256_set1_ps ( 0 );
		__m256i const_0xFF_int_ymm = _mm256_set1_epi32 ( 0xFF );
		__m256i const_0x1F_int_ymm = _mm256_set1_epi32 ( 0x1F );
		__m256i const_0x03_int_ymm = _mm256_set1_epi32 ( 0x03 );
		__m256i fine_int_ymm, coarse_int_ymm, gain_int_ymm;
		__m256 sample_gain_mask_ymm;

		float tmp[8];
		/*Initialising variables needed*/
		sample_frame = input.input_sample.data;
		output = input.output.data;
		reset_frame = input.input_reset.data;

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

		ADU_to_electron = calib.ADU_to_electrons_conversion.data;

		/*Used to correlate a pixel in sample with a pixel in calibration array*/
		row = begin / width;
		row_counter = begin%width;
		col_counter = begin % width;

		/* range to apply AVX */
		/* Needs to be done carefully to avoid segmentation fault */
		const unsigned int avx_grain_end = end - 7;	/* if the grain ends in the middle of the row */

		ADU_2e_conv_ymm = _mm256_loadu_ps( ADU_to_electron + begin );

		/*loop*/
		for(unsigned int i = begin; i < end; i = i + 7){	/* move 7 elements forward each time */
			/*
			 *  Efficient loop counter
			 *
			 */
			if( (row_counter^(width-7)) && (i ^ begin) ){	//4
				row_counter+=7;			//1
			}else{
				if(i ^ begin){	/* If this is not start of the loop */		//1
					row_counter = 0;
					row++;	/* start counting only after the first element */	//1
				}
				/*
				 * 	Load 7 new elements of calibration data Gc, Oc, Gf, Of for every increment in row.
				 * 	Also load when loop starts.
				 */

				/* load first segments of calibration data */
				position_in_calib_array = row * calib_data_width; //This starts with 0.	//3
				/*
				 * If row increases, modify avx_row_end and updata Calibration params
				 *
				 * Each load takes 8 32-bit float. All calbration arrays are multiples of 7.
				 * All percival_frame based memory buffer has at least 8 byte extra space at the end of the buffer.
				 * Segmentation fault might result if this is not guarantteed.
				 *
				 * */
				Gc_ymm = _mm256_loadu_ps( Gc + position_in_calib_array );
				Gf_ymm = _mm256_loadu_ps( Gf + position_in_calib_array );
				Oc_ymm = _mm256_loadu_ps( Oc + position_in_calib_array );
				Of_ymm = _mm256_loadu_ps( Of + position_in_calib_array );
			}


			/* loading parameters */
			gain_table_1_ymm = _mm256_loadu_ps( G1 + i);
			gain_table_2_ymm = _mm256_loadu_ps( G2 + i);
			gain_table_3_ymm = _mm256_loadu_ps( G3 + i);
			gain_table_4_ymm = _mm256_loadu_ps( G4 + i);

			sample_gain_mask_ymm = _mm256_set1_ps ( 1 );
			data = sample_frame + i;			//1

			for(unsigned short ii = 0; ii < 2; ii++){

				data0 = *(data);
				data1 = *(data + 1);
				data2 = *(data + 2);
				data3 = *(data + 3);
				data4 = *(data + 4);
				data5 = *(data + 5);
				data6 = *(data + 6);

				fine_int_ymm = _mm256_set_epi32 ( 0,
						( (data6 >> 2) ), ( (data5 >> 2) ), ( (data4 >> 2) ),			//7
						( (data3 >> 2) ), ( (data2 >> 2) ), ( (data1 >> 2) ),
						( (data0 >> 2) )
				);	/* note order is reverse */

				fine_int_ymm =  _mm256_castps_si256(_mm256_and_ps (  _mm256_castsi256_ps( fine_int_ymm ),  _mm256_castsi256_ps( const_0xFF_int_ymm ) ));
				fine_ymm = _mm256_cvtepi32_ps ( fine_int_ymm );

				coarse_int_ymm = _mm256_set_epi32 ( 0,
						( (data6 >> 10) ), ( (data5 >> 10) ), ( (data4 >> 10) ),			//7
						( (data3 >> 10) ), ( (data2 >> 10) ), ( (data1 >> 10) ),
						( (data0 >> 10) )
				);	/* note order is reverse */

				coarse_int_ymm =  _mm256_castps_si256(_mm256_and_ps (  _mm256_castsi256_ps( coarse_int_ymm ),  _mm256_castsi256_ps( const_0x1F_int_ymm ) ));
				coarse_ymm = _mm256_cvtepi32_ps ( coarse_int_ymm );

				gain_int_ymm = _mm256_set_epi32 ( 0,
						( (data6 ) ), ( (data5 ) ), ( (data4 ) ),
						( (data3 ) ), ( (data2 ) ), ( (data1 ) ),
						( (data0 ) )
				);	/* note order is reverse */

				gain_int_ymm =  _mm256_castps_si256(_mm256_and_ps (  _mm256_castsi256_ps( gain_int_ymm ),  _mm256_castsi256_ps( const_0x03_int_ymm ) ));

				//				gain_int_ymm = _mm256_set1_epi32 (1);
				gain_ymm = _mm256_cvtepi32_ps ( gain_int_ymm );

				gain_mask_1_ymm = _mm256_cmp_ps ( gain_ymm, const_0_ymm , 0);	//3*4 = 12
				gain_mask_2_ymm = _mm256_cmp_ps ( gain_ymm, const_0x01_ymm , 0 );
				gain_mask_3_ymm = _mm256_cmp_ps ( gain_ymm, const_0x02_ymm , 0 );
				gain_mask_4_ymm = _mm256_cmp_ps ( gain_ymm, const_0x03_ymm , 0 );

				gain_mask_1_ymm = _mm256_and_ps ( gain_mask_1_ymm, const_0x01_ymm );	//1*4 = 4
				gain_mask_2_ymm = _mm256_and_ps ( gain_mask_2_ymm, const_0x01_ymm );
				gain_mask_3_ymm = _mm256_and_ps ( gain_mask_3_ymm, const_0x01_ymm );
				gain_mask_4_ymm = _mm256_and_ps ( gain_mask_4_ymm, const_0x01_ymm );

				gain_factor_ymm = _mm256_mul_ps ( gain_mask_1_ymm, gain_table_1_ymm );	//5 *4 = 20
				gain_mask_2_ymm = _mm256_mul_ps ( gain_mask_2_ymm, gain_table_2_ymm );
				gain_mask_3_ymm = _mm256_mul_ps ( gain_mask_3_ymm, gain_table_3_ymm );
				gain_mask_4_ymm = _mm256_mul_ps ( gain_mask_4_ymm, gain_table_4_ymm );

				gain_factor_ymm = _mm256_add_ps ( gain_factor_ymm, gain_mask_2_ymm );	//3*3 = 9
				gain_factor_ymm = _mm256_add_ps ( gain_factor_ymm, gain_mask_3_ymm );
				gain_factor_ymm = _mm256_add_ps ( gain_factor_ymm, gain_mask_4_ymm );

				gain_factor_ymm = _mm256_mul_ps ( sample_gain_mask_ymm, gain_factor_ymm );	//5

				tmp_ymm0  = _mm256_sub_ps ( Oc_ymm, coarse_ymm );	//Oc - coarseBits		//3
				tmp_ymm0  = _mm256_mul_ps ( tmp_ymm0, Gc_ymm );	//Gc * (Oc - coarseBits)	//5

				tmp_ymm1 = _mm256_sub_ps ( fine_ymm, Of_ymm );  //Of - fineBits				//3
				tmp_ymm1 = _mm256_mul_ps ( tmp_ymm1, Gf_ymm );  //Gf * (Of - fineBits)		//5

				tmp_ymm2 = _mm256_sub_ps ( tmp_ymm0, tmp_ymm1 );  //calibrated sample		//3

				result_ymm = _mm256_mul_ps ( tmp_ymm2, gain_factor_ymm );					//5

				if(ii){
					reset_result_ymm = result_ymm;
				}else{
					sample_result_ymm = result_ymm;
					if(
							_mm256_testz_si256 ( _mm256_cvtps_epi32( _mm256_mul_ps( gain_mask_1_ymm, const_0x01_ymm ) ), const_0x01_int_ymm)	//returns 0 if gain = 1		//9
					){
						reset_result_ymm = const_0_ymm;
						goto finish;
					}
				}

				data = reset_frame + i;
				sample_gain_mask_ymm = gain_mask_1_ymm;

			}//end of inner loop

			finish:
			result_ymm = _mm256_sub_ps ( sample_result_ymm, reset_result_ymm );	//3

			final_result_ymm = _mm256_mul_ps ( ADU_2e_conv_ymm, result_ymm );		//5

			/* write to memory */
			if( i < avx_grain_end ){ //(i < avx_grain_end) ){
				/*writing to memory*/
				_mm256_storeu_ps( (output + i), final_result_ymm );
			}else{
				_mm256_storeu_ps( tmp, final_result_ymm);
				for(unsigned int kk = 0; kk < 7; ++kk){
					*(output + i + kk) = tmp[kk];				//14
				}
			}
			/* load next cycle */
			ADU_2e_conv_ymm = _mm256_loadu_ps( ADU_to_electron + i + 7);

		} //outermost for loop
	}//definition of operator overloading
}; //definition of class

/*===============================================================================================================================*/
/*
 * 	Ultimate combined giant huge functor
 *	Non-AVX version
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

		/*listing all variables*/
		float arr[2] = {0,0};
		unsigned short int gain, coarseBits, fineBits;
		unsigned short int count,current_count, increment;
		unsigned short int *data, *sample_frame, *reset_frame;
		unsigned int row, col_counter, row_counter, width, calib_data_width, position_in_calib_array;
		float coarse_calibrated, fine_calibrated, gain_factor, result;
		float *Oc, *Gc, *Of, *Gf, *G1, *G2, *G3, *G4, *output;
		float ADU_to_electron = 1;

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
			arr[0] = 0; arr[1] = 0;
			count = 0;current_count = 0;
			data = sample_frame;

			position_in_calib_array = col_counter + row * calib_data_width;

			do{
				pixel = *(data + i);
				/* unit_ADC_decode */
				gain = pixel & 0x0003;
				fineBits = (pixel & 0x3FC) >> 2;
				coarseBits = (pixel & 0x7c00) >> 10;
				/* unit_ADC_calib */
				coarse_calibrated = (*(Oc + position_in_calib_array) - coarseBits) * *(Gc + position_in_calib_array);
				fine_calibrated = (fineBits - *(Of + position_in_calib_array)) * *(Gf + position_in_calib_array);
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
			arr[current_count] = gain_factor * (coarse_calibrated - fine_calibrated); /*store sample in slot 1 and reset in slot 0*/
			current_count = (current_count+1)&0x1;
		}while( count&current_count );
		/* subtraction */
		result = arr[0] - arr[1];

		result *= ADU_to_electron;

		/*writing to memory*/
		*(output+i) = result;

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
}
};





#endif /* INCLUDE_PERCIVAL_FUNCTORS_H_ */
