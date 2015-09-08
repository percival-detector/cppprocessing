/*
 * percival_data_validity_checks.cpp
 *
 *  Created on: 20 Aug 2015
 *      Author: pqm78245
 */

#include "percival_data_validity_checks.h"

void percival_unit_ADC_decode_check(
		const percival_frame<unsigned short int> & input,
		percival_frame<unsigned short int> & Coarse,
		percival_frame<unsigned short int> & Fine,
		percival_frame<unsigned short int> & Gain)
{
	if(input.width != Coarse.width || input.height != Coarse.height)
		throw dataspace_exception("In percival_unit_ADC_decode(_pf): coarse_frame and input frame dimensions mismatch.");
	if(input.width != Fine.width || input.height != Fine.height)
		throw dataspace_exception("In percival_unit_ADC_decode(_pf): fain_frame and input frame dimensions mismatch.");
	if(input.width != Gain.width || input.height != Gain.height)
		throw dataspace_exception("In percival_unit_ADC_decode(_pf): gain_frame and input frame dimensions mismatch.");

	if(Coarse.data == Fine.data)
		throw datatype_exception("In percival_unit_ADC_decode(_pf): Coarse and Fine frame pointers are identical.");
	if(Coarse.data == Gain.data)
		throw datatype_exception("In percival_unit_ADC_decode(_pf): Coarse and Gain frame pointers are identical.");
	if(Gain.data == Fine.data)
		throw datatype_exception("In percival_unit_ADC_decode(_pf): Gain and Fine frame pointers are identical.");
}

void percival_unit_ADC_calibration_check(const percival_frame<unsigned short int> & Coarse,
		const  percival_frame<unsigned short int> & Fine,
		percival_frame<float>& output,
		const percival_calib_params & calib)
{
	if(output.width != Coarse.width || output.height != Coarse.height)
		throw dataspace_exception("In percival_unit_ADC_calibration(_pf): coarse_frame and output frame dimensions mismatch.");
	if(output.width != Fine.width || output.height != Fine.height)
		throw dataspace_exception("In percival_unit_ADC_calibration(_pf): fine_frame and output frame dimensions mismatch.");

	if(calib.Gc.width != 7)
		throw dataspace_exception("In percival_unit_ADC_calibration(_pf): width of Gc array is not 7.");
	if(calib.Gf.width != 7)
		throw dataspace_exception("In percival_unit_ADC_calibration(_pf): width of Gf array is not 7.");
	if(calib.Oc.width != 7)
		throw dataspace_exception("In percival_unit_ADC_calibration(_pf): width of Oc array is not 7.");
	if(calib.Of.width != 7)
		throw dataspace_exception("In percival_unit_ADC_calibration(_pf): width of Of array is not 7.");

	if(calib.Gc.height != output.height)
		throw dataspace_exception("In percival_unit_ADC_calibration(_pf): height of Gc array and height of output are unequal.");
	if(calib.Gf.height != output.height)
		throw dataspace_exception("In percival_unit_ADC_calibration(_pf): height of Gf array and height of output are unequal.");
	if(calib.Oc.height != output.height)
		throw dataspace_exception("In percival_unit_ADC_calibration(_pf): height of Oc array and height of output are unequal.");
	if(calib.Of.height != output.height)
		throw dataspace_exception("In percival_unit_ADC_calibration(_pf): height of Of array and height of output are unequal.");
}


void percival_unit_gain_multiplication_check(
		const percival_frame<unsigned short int> & src_frame,
		const percival_frame<float> & calibrated,
		percival_frame<float> & output,
		const percival_calib_params & calib_params)
{
	if(src_frame.width != calibrated.width || src_frame.height != calibrated.height)
		throw dataspace_exception("In percival_unit_gain_multiplication: calibrated array and src array dimensions mismatch.");
	if(src_frame.width != output.width || src_frame.height != output.height)
		throw dataspace_exception("In percival_unit_gain_multiplication: output array and src array dimensions mismatch.");

	if(src_frame.width != calib_params.Gain_lookup_table1.width || src_frame.height != calib_params.Gain_lookup_table1.height)
		throw dataspace_exception("In percival_unit_gain_multiplication: gain_lookup_table1 and input array dimensions mismatch.");
	if(src_frame.width != calib_params.Gain_lookup_table2.width || src_frame.height != calib_params.Gain_lookup_table2.height)
		throw dataspace_exception("In percival_unit_gain_multiplication: gain_lookup_table2 and input array dimensions mismatch.");
	if(src_frame.width != calib_params.Gain_lookup_table3.width || src_frame.height != calib_params.Gain_lookup_table3.height)
		throw dataspace_exception("In percival_unit_gain_multiplication: gain_lookup_table3 and input array dimensions mismatch.");
	if(src_frame.width != calib_params.Gain_lookup_table4.width || src_frame.height != calib_params.Gain_lookup_table4.height)
		throw dataspace_exception("In percival_unit_gain_multiplication: gain_lookup_table4 and input array dimensions mismatch.");
}

void percival_CDS_correction_check(
		percival_frame<float> &sample,
		const percival_frame<float> &reset,
		percival_frame<float> &output)
{
	//dimension checks
	if((sample.width != reset.width) || (sample.height != reset.height))
		throw dataspace_exception("In percival_CDS_correction(): sample dimension and reset dimension mismatch.");

	if((output.width != sample.width) || (output.height != sample.height))
		throw dataspace_exception("In percival_CDS_correction: calibration array height and sample array height mismatch.");
}

void percival_ADU_to_electron_correction_check(
		percival_frame<float> &CDS_Img,
		percival_frame<float> &output,
		const percival_calib_params & calib)
{
	if((CDS_Img.width != calib.ADU_to_electrons_conversion.width) || (CDS_Img.height != calib.ADU_to_electrons_conversion.height))
		throw dataspace_exception("In percival_ADU_to_electron_correction(): CDS_Img dimension and ADU_per_el dimension mismatch.");

	if((output.width != CDS_Img.width) || (output.height != CDS_Img.height))		//todo:change this to a inline function
		throw dataspace_exception("In percival_ADU_to_electron_correction: calibration array height and sample array height mismatch.");

}

void percival_input_calib_dimension_check(
		const percival_frame<unsigned short int> & input,
		const percival_calib_params & calib)
{
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

void percival_input_calib_dimension_check_AVX(
		const percival_frame<unsigned short int> & input,
		const percival_calib_params & calib)
{
	if(calib.Gc.width != 8)
		throw dataspace_exception("Width of Gc array is not 8.");
	if(calib.Gf.width != 8)
		throw dataspace_exception("Width of Gf array is not 8.");
	if(calib.Oc.width != 8)
		throw dataspace_exception("Width of Oc array is not 8.");
	if(calib.Of.width != 8)
		throw dataspace_exception("Width of Of array is not 8.");

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

void  percival_input_calib_NULL_check(const percival_calib_params & calib_params){
	if( ( calib_params.Gc.data == NULL )|| ( calib_params.Gf.data == NULL )|| ( calib_params.Oc.data == NULL ) || ( calib_params.Of.data == NULL ))
		throw dataspace_exception("Pointer to calibration data is NULL.");

	if( ( calib_params.Gain_lookup_table1.data == NULL )|| ( calib_params.Gain_lookup_table2.data == NULL )|| ( calib_params.Gain_lookup_table3.data == NULL ) || ( calib_params.Gain_lookup_table4.data == NULL ))
		throw dataspace_exception("Pointer to calibration data is NULL.");

	if( ( calib_params.ADU_to_electrons_conversion.data == NULL) )
		throw dataspace_exception("Pointer to calibration data is NULL.");
}
