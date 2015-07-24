/*
 * percival_calib_params.cpp
 *
 *  Created on: 20 Jul 2015
 *      Author: pqm78245
 */

#include"percival_calib_params.h"

const std::string default_path_name_Gc = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_CoarseGainArray.h5";
const std::string default_path_name_Oc = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_CoarseOffsetArray.h5";
const std::string default_path_name_Gf = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_FineGainArray.h5";
const std::string default_path_name_Of = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_FineOffsetArray.h5";

//const std::string default_path_name_Gc = "./data/test_HDF5.h5";
const std::string default_data_set_name = "/data/data";
//const std::string default_data_set_name_Gc = "float_sample_frame";


bool percival_calib_params::is_initialised = 0;
percival_frame<float> percival_calib_params::Gc;
percival_frame<float> percival_calib_params::Oc;
percival_frame<float> percival_calib_params::Gf;
percival_frame<float> percival_calib_params::Of;

percival_calib_params::percival_calib_params(){
	if(!percival_calib_params::is_initialised){

		percival_HDF5_loader(default_path_name_Gc, default_data_set_name, percival_calib_params::Gc);
		percival_HDF5_loader(default_path_name_Oc, default_data_set_name, percival_calib_params::Oc);
		percival_HDF5_loader(default_path_name_Gf, default_data_set_name, percival_calib_params::Gf);
		percival_HDF5_loader(default_path_name_Of, default_data_set_name, percival_calib_params::Of);

		percival_calib_params::is_initialised = true;

	}
}

percival_calib_params::percival_calib_params(const std::string & name){

}

