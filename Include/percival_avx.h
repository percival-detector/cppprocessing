/*
 * percival_AVX.h
 *
 *  Created on: 7 Sep 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_AVX_H_
#define INCLUDE_PERCIVAL_AVX_H_



#include <immintrin.h>
#include <emmintrin.h>
#include "percival_processing.h"
#include "percival_functors.h"
#include "percival_data_validity_checks.h"

template <typename range_iterator>
class percival_algorithm_avx{
	const percival_frame<unsigned short> input_sample;
	const percival_frame<unsigned short> input_reset;
	percival_frame<float> output_frame;
	const percival_calib_params calib;
public:
	percival_algorithm_avx(
			const percival_frame<unsigned short> &input_sample,
			const percival_frame<unsigned short> &input_reset,
			percival_frame<float> &output_frame,
			const percival_calib_params & calib ):
		input_sample( input_sample ),
		input_reset( input_reset ),
		output_frame(output_frame),
		calib( calib )
{}

	void operator()(range_iterator & r) const
	{
		/*
		 * array iterators
		 */
		unsigned int begin = r.begin();
		unsigned int end = r.end();

		if((begin%7)){
			throw dataspace_exception("Grain not aligned with 7-element boundary.");
		}

		/*listing all variables*/
		unsigned short int *data;
		unsigned int row, row_counter, col_counter, width, calib_data_width, position_in_calib_array, counter;

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

		/* provide aligned memory access to _mm256_store_ps */
		float* unaligned = new float[ 8 + 8 ];
		float* tmp = reinterpret_cast<float*>(
				reinterpret_cast<std::size_t>( unaligned ) +
				reinterpret_cast<std::size_t>( unaligned ) % 32
		);

		/*Initialising variables needed*/
		sample_frame = input_sample.data;
		output = output_frame.data;
		reset_frame = input_reset.data;

		calib_data_width = calib.Gc.width;
		width = input_sample.width;
		counter = (begin/7) * 8;

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
				Gc_ymm = _mm256_load_ps( Gc + position_in_calib_array );
				Gf_ymm = _mm256_load_ps( Gf + position_in_calib_array );
				Oc_ymm = _mm256_load_ps( Oc + position_in_calib_array );
				Of_ymm = _mm256_load_ps( Of + position_in_calib_array );
			}
			/* loading parameters */
			gain_table_1_ymm = _mm256_load_ps( G1 + counter);
			gain_table_2_ymm = _mm256_load_ps( G2 + counter);
			gain_table_3_ymm = _mm256_load_ps( G3 + counter);
			gain_table_4_ymm = _mm256_load_ps( G4 + counter);
			ADU_2e_conv_ymm = _mm256_load_ps( ADU_to_electron + counter);

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
			if( i < avx_grain_end ){
				/*writing to memory*/
				_mm256_storeu_ps( (output + i), final_result_ymm );
			}else{
				_mm256_store_ps( tmp, final_result_ymm);
				for(unsigned int kk = 0; kk < 7; ++kk){
					*(output + i + kk) = tmp[kk];				//14
				}
			}
			counter += 8;
		} //outermost for loop
		delete [] unaligned;
		tmp = NULL;
	}//definition of operator overloading
}; //definition of class

#include "percival_parallel.h"
void percival_ADC_decode_combined_pipeline_avx(
		const percival_frame<unsigned short int> & sample,
		const percival_frame<unsigned short int> & reset,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		unsigned int grain_size,
		unsigned int max_tokens = 20);


#endif /* INCLUDE_PERCIVAL_AVX_H_ */
