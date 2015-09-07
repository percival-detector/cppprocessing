/*
 * percival_load_calib_params.cpp
 *
 *  Created on: 12 Aug 2015
 *      Author: pqm78245
 */

#include "percival_load_calib_params.h"
#include "percival_HDF5_loader.h"
#include <stdexcept>
//include hdf5 specific header files here later during refactoring

//these functions were originally part of the constructor of percival_calib_params. In order to separate the percival Processing library and the HDF5 library, they have been implemented here.

void percival_load_calib_params(percival_calib_params & calib_params, percival_global_params & global_params){

	if(!global_params.initialised())
		throw std::invalid_argument("global_params not initialised.");

	const char* default_path_name_Gc	= (global_params.default_location_Gc).c_str();
	const char* default_path_name_Oc	= (global_params.default_location_Oc).c_str();
	const char* default_path_name_Gf	= (global_params.default_location_Gf).c_str();
	const char* default_path_name_Of	= (global_params.default_location_Of).c_str();
	const char* default_data_set_name	= (global_params.default_calib_params_dataset_name).c_str();
	const char* default_path_name_ADU_to_electrons_conversion = (global_params.default_location_ADU_to_electrons_conversion).c_str();
	const char* default_path_name_Gain_lookup_table1	= (global_params.default_location_Gain_lookup_table1).c_str();
	const char* default_path_name_Gain_lookup_table2	= (global_params.default_location_Gain_lookup_table2).c_str();
	const char* default_path_name_Gain_lookup_table3	= (global_params.default_location_Gain_lookup_table3).c_str();
	const char* default_path_name_Gain_lookup_table4	= (global_params.default_location_Gain_lookup_table4).c_str();

	bool transposed = global_params.is_transposed_calib_params;

	percival_frame_mem<float> tmp1;
	percival_frame_mem<float> tmp2;
	percival_frame_mem<float> tmp3;
	percival_frame_mem<float> tmp4;
	percival_frame_mem<float> tmp5;
	percival_frame_mem<float> tmp6;
	percival_frame_mem<float> tmp7;
	percival_frame_mem<float> tmp8;
	percival_frame_mem<float> tmp9;

	percival_frame_mem<float> tmp10(3717, 8);
	percival_frame_mem<float> tmp11(3717, 8);
	percival_frame_mem<float> tmp12(3717, 8);
	percival_frame_mem<float> tmp13(3717, 8);
	percival_frame_mem<float> tmp14(3717, 3528 + 3528/7);
	percival_frame_mem<float> tmp15(3717, 3528 + 3528/7);
	percival_frame_mem<float> tmp16(3717, 3528 + 3528/7);
	percival_frame_mem<float> tmp17(3717, 3528 + 3528/7);
	percival_frame_mem<float> tmp18(3717, 3528 + 3528/7);


	tmp1.automatic_empty = false;
	tmp2.automatic_empty = false;
	tmp3.automatic_empty = false;
	tmp4.automatic_empty = false;
	tmp5.automatic_empty = false;
	tmp6.automatic_empty = false;
	tmp7.automatic_empty = false;
	tmp8.automatic_empty = false;
	tmp9.automatic_empty = false;

	tmp10.automatic_empty = false;
	tmp11.automatic_empty = false;
	tmp12.automatic_empty = false;
	tmp13.automatic_empty = false;
	tmp14.automatic_empty = false;
	tmp15.automatic_empty = false;
	tmp16.automatic_empty = false;
	tmp17.automatic_empty = false;
	tmp18.automatic_empty = false;


	percival_HDF5_loader(default_path_name_Gc, default_data_set_name, tmp1, transposed);
	percival_HDF5_loader(default_path_name_Oc, default_data_set_name, tmp2, transposed);
	percival_HDF5_loader(default_path_name_Gf, default_data_set_name, tmp3, transposed);
	percival_HDF5_loader(default_path_name_Of, default_data_set_name, tmp4, transposed);
	percival_HDF5_loader(default_path_name_ADU_to_electrons_conversion, default_data_set_name, tmp5, transposed);

	percival_HDF5_loader(default_path_name_Gain_lookup_table1, default_data_set_name, tmp6, transposed);
	percival_HDF5_loader(default_path_name_Gain_lookup_table2, default_data_set_name, tmp7, transposed);
	percival_HDF5_loader(default_path_name_Gain_lookup_table3, default_data_set_name, tmp8, transposed);
	percival_HDF5_loader(default_path_name_Gain_lookup_table4, default_data_set_name, tmp9, transposed);

	percival_calib_params::Gc = percival_align_memory(tmp1, tmp10, 7, 8);
	percival_calib_params::Oc = percival_align_memory(tmp2, tmp11, 7, 8);
	percival_calib_params::Gf = percival_align_memory(tmp3, tmp12, 7, 8);
	percival_calib_params::Of = percival_align_memory(tmp4, tmp13, 7, 8);
	percival_calib_params::ADU_to_electrons_conversion = percival_align_memory(tmp5, tmp14, 7, 8);
	percival_calib_params::Gain_lookup_table1 = percival_align_memory(tmp6, tmp15, 7, 8);
	percival_calib_params::Gain_lookup_table2 = percival_align_memory(tmp7, tmp16, 7, 8);
	percival_calib_params::Gain_lookup_table3 = percival_align_memory(tmp8, tmp17, 7, 8);
	percival_calib_params::Gain_lookup_table4 = percival_align_memory(tmp9, tmp18, 7, 8);

	percival_inverse_frame(percival_calib_params::Gc, percival_calib_params::Gc);
	percival_inverse_frame(percival_calib_params::Gf, percival_calib_params::Gf);
}

void percival_inverse_frame(
		percival_frame<float> &input,
		percival_frame<float> &output)
{
	unsigned int NoOfPixels = input.width * input.height;
	for(unsigned int i = 0; i < NoOfPixels; i++){
		*( output.data + i ) = 1 / *( input.data + i );
	}
}

/* default path included here for reference only todo: remove this in final product
const char* default_path_name_Gc = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_CoarseGainArray.h5";
const char* default_path_name_Oc = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_CoarseOffsetArray.h5";
const char* default_path_name_Gf = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_FineGainArray.h5";
const char* default_path_name_Of = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_FineOffsetArray.h5";
const char* default_path_name_ADU_to_electrons_conversion = "./data/trPTC8a_W14_14TS1BSI_TChip-40_bias10f_2014_02_28_21-32_PTC7row_ADU2e.h5";

const char* default_data_set_name = "/data/data";
*/
