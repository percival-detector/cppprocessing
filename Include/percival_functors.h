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
#include <immintrin.h>
#endif

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

/*
 *  Various combinations of the processing algorithm steps
 */

enum algorithm_pipeline{
	head_to_ADC_calibration_enu,
	head_to_gain_multiplication_enu,
	head_to_CDS_enu,
	ADC_calibration_to_gain_multiplication_enu,
	unit_ADC_calibrated,
	unit_gain_multiplication,
	unit_CDS_subtraction,
	unit_ADU_to_electron
};	/* unit_ADC_decode is not supported since the original unit_decode function will outperform */

struct head_to_CDS{
	const static algorithm_pipeline type = head_to_CDS_enu;
	percival_frame<unsigned short int> input_reset;
	percival_frame<unsigned short int> input_sample;
	percival_frame<float> output;
};

struct head_to_ADC_calibration{
	const static algorithm_pipeline type = head_to_ADC_calibration_enu;
	percival_frame<unsigned short int> input_sample;
	percival_frame<float> output;
};

struct head_to_gain_multiplication{
	const static algorithm_pipeline type = head_to_gain_multiplication_enu;
	percival_frame<unsigned short int> input_sample;
	percival_frame<float> output;
};

struct ADC_calibration_to_gain_multiplication{
	const static algorithm_pipeline type = ADC_calibration_to_gain_multiplication_enu;
	percival_frame<unsigned short int> coarse;
	percival_frame<unsigned short int> fine;
	percival_frame<unsigned short int> gain;
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

	void operator()(range_iterator & r) const
	{
		/*
		 * array iterators
		 */
		unsigned int begin = r.begin();
		unsigned int end = r.end();
		unsigned short int pixel;

		/*listing all variables*/
		unsigned short int gain, coarseBits, fineBits;
		unsigned short int count,current_count, index;	/* Only choose 0 or 1 for process_reset */
		unsigned short int *data;
		unsigned int row, row_counter, col_counter, width, calib_data_width, position_in_calib_array, location;
		unsigned int is_reset_frame;

		/* constants */
		float *Oc, *Gc, *Of, *Gf, *G1, *G2, *G3, *G4;
		float* ADU_to_electron;
		float gain_factor;

		short unsigned int *sample_frame, *reset_frame;
		float  *output;

		/* 10 AVX arrays, 16 registers */
		__m256 Gc_ymm, Oc_ymm, Gf_ymm, Of_ymm, ADU_2e_conv_ymm;
		__m256 gain_mask_ymm;
		__m256 result_ymm;
		__m256 gain_factor_ymm;
		__m256 sample_result_ymm, reset_result_ymm;
		__m256 tmp_ymm0, tmp_ymm1, tmp_ymm2, tmp_ymm3, tmp_ymm4, tmp_ymm5;

		/* memory to temporarily store arrays of 8 float */
		float fine_arr[16], coarse_arr[16], gain_factor_arr[16];
		float gain_mask[8];
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

		ADU_2e_conv_ymm = _mm256_loadu_ps( ADU_to_electron );

		for(unsigned short int m = 0; m < 8; m ++){
			*( gain_mask + m ) = 0;
			*( gain_factor_arr + m) = 0;
			*( gain_factor_arr + 8 + m ) = 0;
		}

		/*loop*/
		for(unsigned int i = begin; i < end; ++i){	/* move 7 elements forward each time */
			/*
			 *  Efficient loop counter
			 *
			 */
			if( (row_counter^width) && (i ^ begin) ){
				row_counter++;
			}else{
				if(i ^ begin){	/* If this is not start of the loop */
					row_counter = 0;
					row++;	/* start counting only after the first element */
				}
				/*
				 * 	Load 7 new elements of calibration data Gc, Oc, Gf, Of for every increment in row.
				 * 	Also load when loop starts.
				 */

				/* load first segments of calibration data */
				position_in_calib_array = row * calib_data_width; //This starts with 0.
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

			/*
			 *  The count and current_count are two counters.
			 *  They used to track whether the sample or reset data is being worked on by the while loop.
			 */



			/*
			 *  While loop to work on sample and reset frame.
			 * 	The specification required CDS_subtraction to be applied only to pixels with gain == 0b00
			 * */
			data = sample_frame;
			count = 0; current_count = 0;
			/* counting the array index for temporary storage */
			index = current_count * 8 + col_counter;

			do{

				pixel = *(data + i);

				/* unit_ADC_decode */
				gain = pixel & 0x0003;
				fineBits = (pixel & 0x3FC) >> 2;
				coarseBits = (pixel & 0x7c00) >> 10;

				/* unit_ADC_gain_multiplication */
				switch(gain){
				case 0b00:
					gain_factor = *(G1 +  i);
					count = 1;			/* choose 1 if processing of reset frame is needed. choose 0 otherwise */
					data = reset_frame;
					break;
				case 0b01:
					gain_factor = *(G2  + i);
					break;
				case 0b10:
					gain_factor = *(G3  + i);
					break;
				case 0b11:
					gain_factor = *(G4  + i);
					break;
				default:
					throw datatype_exception("Invalid gain bit detected.");
				}

				/*
				 * store calibrated sample in slot 0 and calibrated reset in slot 1.
				 * slot 1 is zero if CDS_subtraction is not needed
				 * */
//				coarse_arr[current_count][col_counter] = coarseBits;
//				fine_arr[current_count][col_counter] = fineBits;
//				gain_factor_arr[current_count][col_counter] = gain_factor;
				/* change this to pointer dereferencing */

				*( coarse_arr + index ) = coarseBits;
				*( fine_arr + index ) = fineBits;
				*( gain_factor_arr + index ) = gain_factor;

				*( gain_mask + col_counter ) = count;

				/* Updating current count after each iteration */
				current_count = (current_count+1)&0x1;
			}while( count&current_count );


			/* do a computation every 7 iterations */
			if( (col_counter^6) )
				col_counter++;
			else{
				/* do a computation */
				/* sample frame: is_reset_frame = 0 */
				is_reset_frame = 0;
				tmp_ymm0  = _mm256_loadu_ps( coarse_arr );
				tmp_ymm0  = _mm256_sub_ps ( Oc_ymm, tmp_ymm0 );	//Oc - coarseBits
				tmp_ymm0  = _mm256_mul_ps ( tmp_ymm0, Gc_ymm );	//Gc * (Oc - coarseBits)

				tmp_ymm1 = _mm256_loadu_ps( fine_arr );
				tmp_ymm1 = _mm256_sub_ps ( tmp_ymm1, Of_ymm );  //Of - fineBits
				tmp_ymm1 = _mm256_mul_ps ( tmp_ymm1, Gf_ymm );  //Gf * (Of - fineBits)

				tmp_ymm2 = _mm256_sub_ps ( tmp_ymm0, tmp_ymm1 );  //calibrated sample

				gain_factor_ymm = _mm256_loadu_ps( gain_factor_arr );

				sample_result_ymm = _mm256_mul_ps ( tmp_ymm2, gain_factor_ymm );

				/* Reset frame: is_reset_frame = 1 */
				is_reset_frame = 1;
				tmp_ymm3  = _mm256_loadu_ps( coarse_arr + 8 );
				tmp_ymm3  = _mm256_sub_ps ( Oc_ymm, tmp_ymm3 );
				tmp_ymm3  = _mm256_mul_ps ( tmp_ymm3, Gc_ymm );

				tmp_ymm4 = _mm256_loadu_ps( fine_arr + 8 );
				tmp_ymm4 = _mm256_sub_ps ( tmp_ymm4, Of_ymm );
				tmp_ymm4 = _mm256_mul_ps ( tmp_ymm4, Gf_ymm );

				tmp_ymm5 = _mm256_sub_ps ( tmp_ymm3, tmp_ymm4 );

				gain_factor_ymm = _mm256_loadu_ps( gain_factor_arr + 8 );

				reset_result_ymm = _mm256_mul_ps ( tmp_ymm5, gain_factor_ymm );

				gain_mask_ymm = _mm256_loadu_ps(gain_mask);

				/* subtraction */
				tmp_ymm1 = _mm256_mul_ps ( gain_mask_ymm, reset_result_ymm );

				tmp_ymm1 = _mm256_sub_ps ( sample_result_ymm, tmp_ymm1 );

				/* Apply scaling if needed */
				/* flat field correction and dark image subtraction can be applied here too. */
				result_ymm = _mm256_mul_ps ( ADU_2e_conv_ymm, tmp_ymm1 );


				/* write to memory */
				if( (i < avx_grain_end) ){
					/*writing to memory*/
					_mm256_storeu_ps( (output + i - col_counter), result_ymm );
				}else{
					_mm256_storeu_ps( tmp, result_ymm);
					for(unsigned int kk = 0; kk < 7; ++kk){
						*(output + i + kk - col_counter) = tmp[kk];
					}
				}

				/* reset for next iteration */
				for(unsigned short int m = 0; m < 8; m ++){
					*( gain_mask + m ) = 0;
					*( gain_factor_arr + m) = 0;
					*( gain_factor_arr + 8 + m ) = 0;
				}

				/* load next cycle */
				ADU_2e_conv_ymm = _mm256_loadu_ps( ADU_to_electron + i + 1 );

				if( (i + 28) < end ){
					_mm_prefetch( ( sample_frame + i + 1 + 28 ) ,_MM_HINT_T0);	//Preload 14 cycle ahead ~154*14 instructions
					_mm_prefetch( ( reset_frame + i + 1 + 28 ) ,_MM_HINT_T0);
				}
				col_counter = 0;
			}  // end of loop counter




		} //outermost for loop
	}//definition of operator overloading
}; //definition of class





#endif /* INCLUDE_PERCIVAL_FUNCTORS_H_ */
