/*
 * percival_calib_params.cpp
 *
 *  Created on: 20 Jul 2015
 *      Author: pqm78245
 */

#include"percival_calib_params.h"

const char* default_path_name_Gc = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_CoarseGainArray.h5";
const char* default_path_name_Oc = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_CoarseOffsetArray.h5";
const char* default_path_name_Gf = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_FineGainArray.h5";
const char* default_path_name_Of = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_FineOffsetArray.h5";
const char* default_path_name_ADU_to_electrons_conversion = "./data/trPTC8a_W14_14TS1BSI_TChip-40_bias10f_2014_02_28_21-32_PTC7row_ADU2e.h5";

const char* default_data_set_name = "/data/data";
bool transposed = true;

bool percival_calib_params::is_initialised = 0;
percival_frame<float> percival_calib_params::Gc;
percival_frame<float> percival_calib_params::Oc;
percival_frame<float> percival_calib_params::Gf;
percival_frame<float> percival_calib_params::Of;
percival_frame<float> percival_calib_params::ADU_to_electrons_conversion;		//32 bit floating point

percival_calib_params::percival_calib_params(){
	if(!percival_calib_params::is_initialised){
		initialisation( default_path_name_Gc,
							 default_path_name_Oc,
							 default_path_name_Gf,
							 default_path_name_Of,
							 default_path_name_ADU_to_electrons_conversion,
							 default_data_set_name,
							 transposed);
	}
}
percival_calib_params::percival_calib_params(percival_global_params & master_file){
		default_path_name_Gc	= (master_file.default_location_Gc).c_str();
		default_path_name_Oc	= (master_file.default_location_Oc).c_str();
		default_path_name_Gf	= (master_file.default_location_Gf).c_str();
		default_path_name_Of	= (master_file.default_location_Of).c_str();
		default_data_set_name	= (master_file.default_calib_params_dataset_name).c_str();
		default_path_name_ADU_to_electrons_conversion = (master_file.default_location_ADU_to_electrons_conversion).c_str();
		transposed = master_file.is_transposed_calib_params;	//
		initialisation( default_path_name_Gc,
							 default_path_name_Oc,
							 default_path_name_Gf,
							 default_path_name_Of,
							 default_path_name_ADU_to_electrons_conversion,
							 default_data_set_name,
							 transposed);
}

void percival_calib_params::initialisation(const char* default_path_name_Gc,
											const char* default_path_name_Oc,
											const char* default_path_name_Gf,
											const char* default_path_name_Of,
											const char* default_path_name_ADU_to_electrons_conversion,
											const char* default_data_set_name,
											bool transposed){

	percival_HDF5_loader(default_path_name_Gc, default_data_set_name, percival_calib_params::Gc, transposed);
	percival_HDF5_loader(default_path_name_Oc, default_data_set_name, percival_calib_params::Oc, transposed);
	percival_HDF5_loader(default_path_name_Gf, default_data_set_name, percival_calib_params::Gf, transposed);
	percival_HDF5_loader(default_path_name_Of, default_data_set_name, percival_calib_params::Of, transposed);
	percival_HDF5_loader(default_path_name_ADU_to_electrons_conversion, default_data_set_name, percival_calib_params::ADU_to_electrons_conversion, transposed);

	percival_calib_params::is_initialised = true;
}
