/*
 * percival_functors.h
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_FUNCTORS_H_
#define INCLUDE_PERCIVAL_FUNCTORS_H_

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

#endif /* INCLUDE_PERCIVAL_FUNCTORS_H_ */
