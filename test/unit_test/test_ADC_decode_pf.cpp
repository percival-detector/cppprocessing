///*
// * test_ADC_decode_pf.cpp
// *
// *  Created on: 10 Jul 2015
// *      Author: pqm78245
// */
//
//#include "percival_processing.h"
//#include "percival_load_calib_params.h"
//#include "percival_parallel.h"
//
//#include <boost/test/unit_test.hpp>
//
//const int TEST_FRAME_HEIGHT 	= 	160;		//cannot be 10
//const int TEST_FRAME_WIDTH		=	210;		//cannot be 10
//
//
//
//class fixture_frame_pf{
//public:
//	percival_frame_mem<unsigned short int> tmp1;
//	percival_frame_mem<float> tmp2;
//	percival_frame_mem<float> tmp3;
//
//	percival_frame_mem<float> tmp4;
//	percival_frame_mem<float> tmp5;
//	percival_frame_mem<float> tmp6;
//	percival_frame_mem<float> tmp7;
//	percival_frame_mem<float> tmp8;
//	percival_frame_mem<float> tmp9;
//	percival_frame_mem<float> tmp10;
//	percival_frame_mem<float> tmp11;
//
//	percival_calib_params calib_params;
//	percival_frame<unsigned short int> src_frame;
//	percival_frame<float> des_frame;
//	percival_frame<float> output_frame;
//
//	fixture_frame_pf():
//		tmp1(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
//		tmp2(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
//		tmp3(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
//		tmp4(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
//		tmp5(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
//		tmp6(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
//		tmp7(TEST_FRAME_HEIGHT, TEST_FRAME_WIDTH),
//		tmp8(TEST_FRAME_HEIGHT, 7),
//		tmp9(TEST_FRAME_HEIGHT, 7),
//		tmp10(TEST_FRAME_HEIGHT, 7),
//		tmp11(TEST_FRAME_HEIGHT, 7),
//		src_frame(tmp1),
//		des_frame(tmp2),
//		output_frame(tmp3)
//
//	{
//		calib_params.Gain_lookup_table1 = tmp4;
//		calib_params.Gain_lookup_table2 = tmp5;
//		calib_params.Gain_lookup_table3 = tmp6;
//		calib_params.Gain_lookup_table4 = tmp7;
//		calib_params.Gc = tmp8;
//		calib_params.Gf = tmp9;
//		calib_params.Oc = tmp10;
//		calib_params.Of = tmp11;
//	}
//};
//
//BOOST_FIXTURE_TEST_SUITE (percival_ADC_decode_pf_test,fixture_frame_pf)
//
////RANGE TESTS
//	BOOST_AUTO_TEST_CASE (should_throw_exception_when_output_and_input_dimensions_mismatch){
//		percival_frame<float> wrong_size_frame;
//		wrong_size_frame.set_frame_size(10,10);
//		BOOST_REQUIRE_THROW(percival_ADC_decode_pf(src_frame,wrong_size_frame, calib_params), dataspace_exception);
//	}
//
//	BOOST_AUTO_TEST_CASE (should_throw_exception_when_calibration_arrays_and_input_dimensions_mismatch){
//		calib_params.Gc.set_frame_size(10, 7);
//		BOOST_REQUIRE_THROW(percival_ADC_decode_pf(src_frame,des_frame, calib_params), dataspace_exception);
//
//		calib_params.Gc.set_frame_size(TEST_FRAME_HEIGHT, 10);
//		BOOST_REQUIRE_THROW(percival_ADC_decode_pf(src_frame,des_frame, calib_params), dataspace_exception);
//
//		calib_params.Gain_lookup_table1.set_frame_size(10,10);
//		BOOST_REQUIRE_THROW(percival_ADC_decode_pf(src_frame,des_frame, calib_params), dataspace_exception);
//	}
////COMPUTATION TESTS
//	BOOST_AUTO_TEST_CASE (try_correct_values){
//			float Gc, Gf, Oc, Of;
//			Gc = 0.5; Gf = 1; Oc = 2.3; Of = 2.9;
//			float G1, G2, G3, G4;
//			G1 = 1.1; G2 = 2.7; G3 = 3.5; G4 = 5.3;
//			float ADU_2e_conv = 7.1;
//
//			*(calib_params.Gain_lookup_table1.data) = G1;
//			*(calib_params.Gain_lookup_table2.data) = G2;
//			*(calib_params.Gain_lookup_table3.data) = G3;
//			*(calib_params.Gain_lookup_table4.data) = G4;
//			*(calib_params.Gc.data) = Gc;
//			*(calib_params.Gf.data) = Gf;
//			*(calib_params.Oc.data) = Oc;
//			*(calib_params.Of.data) = Of;
//
//			*(src_frame.data) = 0b0111110111110101;
//			float correct_result = ((0b11111 - Oc)/Gc + (Of - 0b1111101)) * G1;
//			BOOST_REQUIRE_NO_THROW(percival_ADC_decode_pf(src_frame, output_frame, calib_params));
//			BOOST_REQUIRE_CLOSE(correct_result, *(output_frame.data), 0.01);
//		}
//
//BOOST_AUTO_TEST_SUITE_END()
//
