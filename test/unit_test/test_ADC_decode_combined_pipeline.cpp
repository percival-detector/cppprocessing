/*
 * test_ADC_decode_combined_pipeline.cpp
 *
 *  Created on: 02 Sep 2015
 *      Author: pqm78245
 */

#include "percival_processing.h"
#include "percival_load_calib_params.h"
#include "percival_parallel.h"

#include <boost/test/unit_test.hpp>

const int TEST_FRAME_HEIGHT 	= 	160;		//cannot be 10
const int TEST_FRAME_WIDTH		=	210;		//cannot be 10



class fixture_ADC_decode_combined_pipeline{
public:
	percival_frame_mem<unsigned short int> tmp1;
	percival_frame_mem<unsigned short int> tmp2;
	percival_frame_mem<float> tmp3;

	percival_frame_mem<float> tmp4;
	percival_frame_mem<float> tmp5;
	percival_frame_mem<float> tmp6;
	percival_frame_mem<float> tmp7;
	percival_frame_mem<float> tmp8;
	percival_frame_mem<float> tmp9;
	percival_frame_mem<float> tmp10;
	percival_frame_mem<float> tmp11;
	percival_frame_mem<float> tmp12;

	percival_calib_params calib_params;
	percival_frame<unsigned short int> sample_frame;
	percival_frame<unsigned short int> reset_frame;
	percival_frame<float> output_frame;

	fixture_ADC_decode_combined_pipeline():
		tmp1(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp2(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp3(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp4(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp5(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp6(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp7(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		tmp8(TEST_FRAME_HEIGHT, 7),
		tmp9(TEST_FRAME_HEIGHT, 7),
		tmp10(TEST_FRAME_HEIGHT, 7),
		tmp11(TEST_FRAME_HEIGHT, 7),
		tmp12(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
		sample_frame(tmp1),
		reset_frame(tmp2),
		output_frame(tmp3)

	{
		calib_params.Gain_lookup_table1 = tmp4;
		calib_params.Gain_lookup_table2 = tmp5;
		calib_params.Gain_lookup_table3 = tmp6;
		calib_params.Gain_lookup_table4 = tmp7;
		calib_params.Gc = tmp8;
		calib_params.Gf = tmp9;
		calib_params.Oc = tmp10;
		calib_params.Of = tmp11;
		calib_params.ADU_to_electrons_conversion = tmp12;
	}
};

BOOST_FIXTURE_TEST_SUITE (percival_ADC_decode_combined_pipeline_test,fixture_ADC_decode_combined_pipeline)

/*
percival_ADC_decode_combined_pipeline(
		const percival_frame<unsigned short int> & sample,
		const percival_frame<unsigned short int> & reset,
		percival_frame<float> & output,
		const percival_calib_params & calib_params,
		unsigned int grain_size,
		bool store_gain)
*/


//RANGE TESTS
	BOOST_AUTO_TEST_CASE (should_throw_exception_when_output_and_input_dimensions_mismatch){
		percival_frame_mem<float> tmp_float(10,10);
		percival_frame_mem<unsigned short> tmp_int(10,10);

		percival_frame<float> wrong_size_frame_float(tmp_float);
		percival_frame<unsigned short> wrong_size_frame_int(tmp_int);

		BOOST_REQUIRE_THROW(percival_ADC_decode_combined_pipeline(sample_frame, wrong_size_frame_int,output_frame, calib_params, TEST_FRAME_WIDTH), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_ADC_decode_combined_pipeline(wrong_size_frame_int,reset_frame,output_frame, calib_params, TEST_FRAME_WIDTH), dataspace_exception);
		BOOST_REQUIRE_THROW(percival_ADC_decode_combined_pipeline(sample_frame,reset_frame,wrong_size_frame_float, calib_params, TEST_FRAME_WIDTH), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE (should_throw_exception_when_calibration_arrays_and_input_dimensions_mismatch){
		calib_params.Gc.set_frame_size(10, 7);
		BOOST_REQUIRE_THROW(percival_ADC_decode_combined_pipeline(sample_frame,reset_frame,output_frame, calib_params, TEST_FRAME_WIDTH), dataspace_exception);

		calib_params.Gc.set_frame_size(TEST_FRAME_HEIGHT, 10);
		BOOST_REQUIRE_THROW(percival_ADC_decode_combined_pipeline(sample_frame,reset_frame,output_frame, calib_params, TEST_FRAME_WIDTH), dataspace_exception);

		calib_params.Gain_lookup_table1.set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_ADC_decode_combined_pipeline(sample_frame,reset_frame,output_frame, calib_params, TEST_FRAME_WIDTH), dataspace_exception);

		calib_params.ADU_to_electrons_conversion.set_frame_size(10,10);
		BOOST_REQUIRE_THROW(percival_ADC_decode_combined_pipeline(sample_frame,reset_frame,output_frame, calib_params, TEST_FRAME_WIDTH), dataspace_exception);
	}

	BOOST_AUTO_TEST_CASE ( should_throw_exception_if_grain_size_exceed_no_of_element_or_below_zero ){
		BOOST_REQUIRE_NO_THROW(percival_ADC_decode_combined_pipeline(sample_frame,reset_frame,output_frame, calib_params, TEST_FRAME_WIDTH * TEST_FRAME_HEIGHT + 1));
		BOOST_REQUIRE_NO_THROW(percival_ADC_decode_combined_pipeline(sample_frame,reset_frame,output_frame, calib_params, -10));
		BOOST_REQUIRE_NO_THROW(percival_ADC_decode_combined_pipeline(sample_frame,reset_frame,output_frame, calib_params, 0));
	}

	BOOST_AUTO_TEST_CASE ( should_not_throw_exception_if_grain_size_is_zero ){
		BOOST_REQUIRE_NO_THROW(percival_ADC_decode_combined_pipeline(sample_frame,reset_frame,output_frame, calib_params, 0));
	}


//COMPUTATION TESTS
	BOOST_AUTO_TEST_CASE (try_correct_values){
			unsigned int location = 0;
			/* calibration data */
			float Gc, Gf, Oc, Of;
			Gc = 0.5; Gf = 1; Oc = 2.3; Of = 2.9;

			float G1, G2, G3, G4;
			G1 = 1.1; G2 = 2.7; G3 = 3.5; G4 = 5.3;

			float ADU_2e_conv = 7.1;
			float dark_image = 11.9;

			*(calib_params.Gain_lookup_table1.data) = G1;
			*(calib_params.Gain_lookup_table2.data) = G2;
			*(calib_params.Gain_lookup_table3.data) = G3;
			*(calib_params.Gain_lookup_table4.data) = G4;

			*(calib_params.Gc.data) = Gc;
			*(calib_params.Gf.data) = Gf;
			*(calib_params.Oc.data) = Oc;
			*(calib_params.Of.data) = Of;

			*(calib_params.ADU_to_electrons_conversion.data) = ADU_2e_conv;

			*(sample_frame.data + location) = 0b0111110111110101;	/* No CDS_subtraction needed 0b011111 01111101 01 */
			*(reset_frame.data + location) = 0b0111110111110101;

			float correct_result = ( ( Oc - 0b011111) * Gc - ( 0b01111101 - Of) * Gf ) * G2 * ADU_2e_conv;

			BOOST_REQUIRE_NO_THROW(percival_ADC_decode_combined_pipeline(sample_frame, reset_frame, output_frame, calib_params, TEST_FRAME_WIDTH));
			BOOST_REQUIRE_CLOSE(correct_result, *(output_frame.data + location), 0.01);
		}


		BOOST_AUTO_TEST_CASE ( given_equal_values_to_pixels_at_periodic_locations_should_return_equal_value ){
				unsigned int location = 14;
				/* calibration data */
				float Gc, Gf, Oc, Of;
				Gc = 0.5; Gf = 1; Oc = 2.3; Of = 2.9;

				float G1, G2, G3, G4;
				G1 = 1.1; G2 = 2.7; G3 = 3.5; G4 = 5.3;

				float ADU_2e_conv = 7.1;
				float dark_image = 11.9;

				*(calib_params.Gain_lookup_table1.data + location) = G1;
				*(calib_params.Gain_lookup_table2.data + location) = G2;
				*(calib_params.Gain_lookup_table3.data + location) = G3;
				*(calib_params.Gain_lookup_table4.data + location) = G4;

				*(calib_params.Gc.data) = Gc;
				*(calib_params.Gf.data) = Gf;
				*(calib_params.Oc.data) = Oc;
				*(calib_params.Of.data) = Of;

				*(calib_params.ADU_to_electrons_conversion.data + location) = ADU_2e_conv;

				*(sample_frame.data + location) = 0b0111110111110101;	/* No CDS_subtraction needed 0b011111 01111101 01 */
				*(reset_frame.data + location) = 0b0111110111110101;

				float correct_result = ( ( Oc - 0b011111 ) * Gc - ( 0b01111101 - Of ) * Gf ) * G2 * ADU_2e_conv;

				BOOST_REQUIRE_NO_THROW(percival_ADC_decode_combined_pipeline(sample_frame, reset_frame, output_frame, calib_params, TEST_FRAME_WIDTH));
				BOOST_REQUIRE_CLOSE(correct_result, *(output_frame.data + location), 0.01);
			}

		BOOST_AUTO_TEST_CASE ( should_use_correct_rows_in_calibration ){
						unsigned int location = TEST_FRAME_WIDTH;
						/* calibration data */
						float Gc, Gf, Oc, Of;
						Gc = 0.5; Gf = 1; Oc = 2.3; Of = 2.9;

						float G1, G2, G3, G4;
						G1 = 1.1; G2 = 2.7; G3 = 3.5; G4 = 5.3;

						float ADU_2e_conv = 7.1;
						float dark_image = 11.9;

						*(calib_params.Gain_lookup_table1.data + location) = G1;
						*(calib_params.Gain_lookup_table2.data + location) = G2;
						*(calib_params.Gain_lookup_table3.data + location) = G3;
						*(calib_params.Gain_lookup_table4.data + location) = G4;

						*(calib_params.Gc.data + 7) = Gc;
						*(calib_params.Gf.data + 7) = Gf;
						*(calib_params.Oc.data + 7) = Oc;
						*(calib_params.Of.data + 7) = Of;

						*(calib_params.ADU_to_electrons_conversion.data + location) = ADU_2e_conv;

						*(sample_frame.data + location) = 0b0111110111110101;	/* No CDS_subtraction needed 0b011111 01111101 01 */
						*(reset_frame.data + location) = 0b0111110111110101;

						float correct_result = ( ( Oc - 0b011111 ) * Gc - ( 0b01111101 - Of ) * Gf ) * G2 * ADU_2e_conv;

						BOOST_REQUIRE_NO_THROW(percival_ADC_decode_combined_pipeline(sample_frame, reset_frame, output_frame, calib_params, TEST_FRAME_WIDTH));
						BOOST_REQUIRE_CLOSE(correct_result, *(output_frame.data + location), 0.01);
					}

		BOOST_AUTO_TEST_CASE ( end_of_array_element_should_give_correct_result ){
						unsigned int location = TEST_FRAME_WIDTH * TEST_FRAME_HEIGHT - 1;
						/* calibration data */
						float Gc, Gf, Oc, Of;
						Gc = 0.5; Gf = 1; Oc = 2.3; Of = 2.9;

						float G1, G2, G3, G4;
						G1 = 1.1; G2 = 2.7; G3 = 3.5; G4 = 5.3;

						float ADU_2e_conv = 7.1;
						float dark_image = 11.9;

						*(calib_params.Gain_lookup_table1.data + location) = G1;
						*(calib_params.Gain_lookup_table2.data + location) = G2;
						*(calib_params.Gain_lookup_table3.data + location) = G3;
						*(calib_params.Gain_lookup_table4.data + location) = G4;

						*(calib_params.Gc.data + 7 * TEST_FRAME_HEIGHT - 1) = Gc;
						*(calib_params.Gf.data + 7 * TEST_FRAME_HEIGHT - 1) = Gf;
						*(calib_params.Oc.data + 7 * TEST_FRAME_HEIGHT - 1) = Oc;
						*(calib_params.Of.data + 7 * TEST_FRAME_HEIGHT - 1) = Of;

						*(calib_params.ADU_to_electrons_conversion.data + location) = ADU_2e_conv;

						*(sample_frame.data + location) = 0b0111110111110101;	/* No CDS_subtraction needed 0b011111 01111101 01 */
						*(reset_frame.data + location) = 0b0111110111110101;

						float correct_result = ( ( Oc - 0b011111 ) * Gc - ( 0b01111101 - Of ) * Gf ) * G2 * ADU_2e_conv;

						BOOST_REQUIRE_NO_THROW(percival_ADC_decode_combined_pipeline(sample_frame, reset_frame, output_frame, calib_params, TEST_FRAME_WIDTH));
						BOOST_REQUIRE_CLOSE(correct_result, *(output_frame.data + location), 0.01);
					}

		BOOST_AUTO_TEST_CASE ( should_only_apply_CDS_subtraction_for_sample_pixel_with_gain_00b ){
						unsigned int location = 0;
						/* calibration data */
						float Gc, Gf, Oc, Of;
						Gc = 0.5; Gf = 1; Oc = 2.3; Of = 2.9;

						float G1, G2, G3, G4;
						G1 = 1.1; G2 = 2.7; G3 = 3.5; G4 = 5.3;

						float ADU_2e_conv = 7.1;
						float dark_image = 11.9;

						*(calib_params.Gain_lookup_table1.data + location) = G1;
						*(calib_params.Gain_lookup_table2.data + location) = G2;
						*(calib_params.Gain_lookup_table3.data + location) = G3;
						*(calib_params.Gain_lookup_table4.data + location) = G4;

						*(calib_params.Gc.data) = Gc;
						*(calib_params.Gf.data) = Gf;
						*(calib_params.Oc.data) = Oc;
						*(calib_params.Of.data) = Of;

						*(calib_params.ADU_to_electrons_conversion.data + location) = ADU_2e_conv;

						*(sample_frame.data + location) = 0b0111110111110100;	/* No CDS_subtraction needed 0b011111 01111101 01 */
						*(reset_frame.data + location) = 0b0011110111010111;

						float sample_result = ( ( Oc - 0b011111 ) * Gc - ( 0b01111101 - Of ) * Gf ) * G1 * ADU_2e_conv;
						float reset_result = ( ( Oc - 0b001111 ) * Gc - ( 0b01110101 - Of ) * Gf ) * G4 * ADU_2e_conv;
						float correct_result = sample_result - reset_result;

						BOOST_REQUIRE_NO_THROW(percival_ADC_decode_combined_pipeline(sample_frame, reset_frame, output_frame, calib_params, TEST_FRAME_WIDTH));
						BOOST_REQUIRE_CLOSE(correct_result, *(output_frame.data + location), 0.01);
					}


BOOST_AUTO_TEST_SUITE_END()
