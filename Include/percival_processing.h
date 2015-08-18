/*
 * percival_processing.h
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_PROCESSING_H_
#define INCLUDE_PERCIVAL_PROCESSING_H_

#include "percival_exceptions.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>


template<typename T>
class percival_frame{
public:
	/*
	 * typically width, height ~= 10,000, use signed short is sufficient. width * height ~= 100,000,000, 27 bit unsigned int, int is sufficient.
	 * if number of pixels is to be assumed less than range of int (INT_MIN - (2^31 -1)), width and height should be at most 2^15
	 * thus use int width, height to allow multiplications etc
	 * use a check in method set_frame_size to set width and height less than 2^15
	 *
	 * Evetually I still decide to use unsigned int for width and height, and unsigned int for all loop using width * height. This allows a larger range of widht and height.
	 * Width * height is guaranteed to be less than the range of unsigned int (0x7fffffff)
	 */
	unsigned int width, height;			//
	T* data;
	//add an offset if splitting up the image is needed
	std::vector<int> CDS_subtraction_indices;		//stores pixel indices requiring CDS_substraction

	void set_frame_size(unsigned int h, unsigned int w)
	{
		if((unsigned int)(h * w) > 0x7fffffff ){			//separated into two ifs to make code more readable
			throw datatype_exception("Image size overflows. Should be less than 32768 pixels in each dimension.");
		}else if(h < 0 || w < 0 || h * w < 0){
			throw datatype_exception("Image size overflows. Should be greater than or equal to 0 and less than 32768 pixels in each dimension.");
		}else{
			delete [] data;
			width = w;
			height = h;
			data = new T[width * height];
		}
	}

	percival_frame(){data = new T[1]; set_frame_size(1,1);}
	percival_frame(unsigned int x, int y){data = new T[1]; set_frame_size(x,y);}
	~percival_frame(){delete [] data;}
};

struct percival_calib_params{
public:
	static percival_frame<float> Gc;
	static percival_frame<float> Oc;
	static percival_frame<float> Gf;
	static percival_frame<float> Of;
	static percival_frame<float> ADU_to_electrons_conversion;
	static percival_frame<float> Gain_lookup_table1;
	static percival_frame<float> Gain_lookup_table2;
	static percival_frame<float> Gain_lookup_table3;
	static percival_frame<float> Gain_lookup_table4;

};

class percival_global_params{
private:
	static bool is_initialised;
	static bool is_initialised_every_member[255];
	static int current_frame;

	//"KnifeQuadBPos1/";, rather than "KnifeQuadBPos1/!X!/Sample";
	static std::string top_level_data_set_name;

public:
	//frame and file properties
	static unsigned int total_number_of_frames;

	static unsigned int frame_height;
	static unsigned int frame_width;

	static std::string default_path_name;
	static std::string default_data_set_name;			//make this private

	//calibration parameter properties
	static bool is_transposed_calib_params;
	static unsigned int calib_params_height;
	static unsigned int calib_params_width;

	static std::string default_location_Gc;
	static std::string default_location_Gf;
	static std::string default_location_Oc;
	static std::string default_location_Of;
	static std::string default_location_ADU_to_electrons_conversion;
	static std::string default_location_Gain_lookup_table1;
	static std::string default_location_Gain_lookup_table2;
	static std::string default_location_Gain_lookup_table3;
	static std::string default_location_Gain_lookup_table4;

	static std::string default_calib_params_dataset_name;

	percival_global_params();
	percival_global_params(const std::string & master_param_file);


	void initialisation(const std::string & master_param_file);
	bool initialised();
	bool load_master_param_file(const std::string & master_param_file);
	bool check_initialisation();
};

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

void percival_ADC_decode(const percival_frame<unsigned short int> &, percival_frame<float> &, const percival_calib_params & calib_params, bool store_gain = false);
void percival_ADU_to_electron_correction(percival_frame<float> &CDS_Img, percival_frame<float> &output, const percival_calib_params &);
void percival_CDS_correction(percival_frame<float> &sample, const percival_frame<float> &reset, percival_frame<float>& output);

void percival_unit_gain_multiplication(const percival_frame<unsigned short int> & src_frame, const percival_frame<float> & calibrated, percival_frame<float> & output, const percival_calib_params & calib_params, bool check_dimensions = true);
void percival_unit_ADC_decode(const percival_frame<unsigned short int> &, percival_frame<unsigned short int> & Coarse, percival_frame<unsigned short int> & Fine, percival_frame<unsigned short int> & Gain);
void percival_unit_ADC_calibration(const percival_frame<unsigned short int> & Coarse,const  percival_frame<unsigned short int> & Fine, percival_frame<float>& output, const percival_calib_params &, bool check_dimensions = true);

#endif /* INCLUDE_PERCIVAL_PROCESSING_H_ */
