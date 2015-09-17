/*
 * percival_functors.h
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_FUNCTORS_H_
#define INCLUDE_PERCIVAL_FUNCTORS_H_
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

/*===============================================================================================================================*/
/*
 * 	Ultimate combined giant huge functor
 *	Non-AVX version
 */


template<typename range_iterator>
class percival_algorithm_p{
	const percival_frame<unsigned short> input_sample;
	const percival_frame<unsigned short> input_reset;
	percival_frame<float> output_frame;
	const percival_calib_params calib;
public:
	percival_algorithm_p(
			const percival_frame<unsigned short> &input_sample,
			const percival_frame<unsigned short> &output_reset,
			percival_frame<float> &output_frame,
			const percival_calib_params & calib ):
		input_sample( input_sample ),
		input_reset( input_reset ),
		output_frame(output_frame),
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
		unsigned short int count,current_count;
		unsigned short int *data, *sample_frame, *reset_frame;
		unsigned int row, col_counter, row_counter, width, calib_data_width, position_in_calib_array;
		float coarse_calibrated, fine_calibrated, gain_factor, result;
		float *Oc, *Gc, *Of, *Gf, *G1, *G2, *G3, *G4, *output;
		float ADU_to_electron = 1;

		/*Initialising variables needed*/
		sample_frame = input_sample.data;
		output = output_frame.data;
		reset_frame = input_reset.data;

		calib_data_width = calib.Gc.width;
		width = input_sample.width;

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
