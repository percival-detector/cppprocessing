/*
 * percival_data_validity_checks.h
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_DATA_VALIDITY_CHECKS_H_
#define INCLUDE_PERCIVAL_DATA_VALIDITY_CHECKS_H_
/*Separate out functions that check for dimensions, datatype and range validity etc.*/
#include "percival_exceptions.h"
#include "percival_processing.h"

/*Function 1*/
void percival_unit_ADC_decode_check(
		const percival_frame<unsigned short int> & input,
		percival_frame<unsigned short int> & Coarse,
		percival_frame<unsigned short int> & Fine,
		percival_frame<unsigned short int> & Gain);

/*Function 2*/
void percival_unit_ADC_calibration_check(const percival_frame<unsigned short int> & Coarse,
		const  percival_frame<unsigned short int> & Fine,
		percival_frame<float>& output,
		const percival_calib_params & calib);

/*Function 3*/
void percival_unit_gain_multiplication_check(
		const percival_frame<unsigned short int> & src_frame,
		const percival_frame<float> & calibrated,
		percival_frame<float> & output,
		const percival_calib_params & calib_params);

/*Function 4*/
void percival_CDS_correction_check(
		percival_frame<float> &sample,
		const percival_frame<float> &reset,
		percival_frame<float> &output);

/*Function 5*/
void percival_ADU_to_electron_correction_check(
		percival_frame<float> &CDS_Img,
		percival_frame<float> &output,
		const percival_calib_params & calib);

/* Function 6 */
/* Checks whether input and calibration array dimensions are consistent */
template<typename T>
void percival_input_calib_dimension_check(
		const percival_frame<T> & input,
		const percival_calib_params & calib){
	if(calib.Gc.width != 7)
		throw dataspace_exception("Width of Gc array is not 7.");
	if(calib.Gf.width != 7)
		throw dataspace_exception("Width of Gf array is not 7.");
	if(calib.Oc.width != 7)
		throw dataspace_exception("Width of Oc array is not 7.");
	if(calib.Of.width != 7)
		throw dataspace_exception("Width of Of array is not 7.");

	if(calib.Gc.height != input.height)
		throw dataspace_exception("Height of Gc array and height of output are unequal.");
	if(calib.Gf.height != input.height)
		throw dataspace_exception("Height of Gf array and height of output are unequal.");
	if(calib.Oc.height != input.height)
		throw dataspace_exception("Height of Oc array and height of output are unequal.");
	if(calib.Of.height != input.height)
		throw dataspace_exception("Height of Of array and height of output are unequal.");

	if((input.width%7))
		throw dataspace_exception("Width of Gc, Gf, Oc, Of arrays is not divisible by 7.");

}

/* Function 7 */
/* Checks whether input and output array dimensions are consistent */
template<typename in_type, typename out_type>
void percival_input_output_dimension_check(
		const percival_frame<in_type> & input,
		const percival_frame<out_type> & output)
{
	if(input.width!=output.width)
		throw dataspace_exception("Input and output frame width mismatch.");
	if(input.height!=output.height)
		throw dataspace_exception("Input and output frame width mismatch.");
}

/* Function 8 */
/* Checks whether input and calibration array dimensions are consistent */
void percival_input_calib_dimension_check_AVX(
		const percival_frame<unsigned short int> & input,
		const percival_calib_params & calib);

/* Function 9 */
/* Checks whether whether calibration parameters contain and NULL pointers */
void percival_input_calib_NULL_check(const percival_calib_params & calib);

template<typename T>
inline void percival_null_check(const percival_frame<T> &input)
{
	if(input.data == NULL)
		throw dataspace_exception("Pointer to calibration data is NULL.");
}



#endif /* INCLUDE_PERCIVAL_DATA_VALIDITY_CHECKS_H_ */
