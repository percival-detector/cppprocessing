/*
 * percival_calib_params.cpp
 *
 *  Created on: 20 Jul 2015
 *      Author: pqm78245
 */

#include"percival_calib_params.h"

const std::string default_path_name_Gc = "./data/ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_CoarseGainArray.h5";
const std::string default_data_set_name_Gc = "data/data";

class percival_calib_params{
	private:
		static bool is_initialised;
	public:
		static percival_frame<float> Gc;
		//static percival_frame<float> Oc;
		//static percival_frame<float> Gf;
		//static percival_frame<float> Of;

		percival_calib_params();
		percival_calib_params(const std::string & name);
};

bool percival_calib_params::is_initialised = 0;
percival_frame<float> percival_calib_params::Gc;

percival_calib_params::percival_calib_params(){
	if(!percival_calib_params::is_initialised){
		percival_HDF5_loader(default_path_name_Gc, default_data_set_name_Gc, percival_calib_params::Gc);
		percival_calib_params::is_initialised = true;
	}
}

percival_calib_params::percival_calib_params(const std::string & name){

}

