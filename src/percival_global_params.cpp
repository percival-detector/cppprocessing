/*
 * percival_global_params.cpp
 *
 *  Created on: 31 Jul 2015
 *      Author: pqm78245
 */
#include <cstdlib>
#include "percival_processing.h"

//image height, image width, total frame number, default file locations, calibration height, calibration width, rotated?

bool percival_global_params::is_initialised = false;
bool percival_global_params::is_initialised_every_member[255] = {};
std::string percival_global_params::default_path_name;
std::string percival_global_params::default_data_set_name;
bool percival_global_params::is_transposed_calib_params;
unsigned int percival_global_params::calib_params_height;
unsigned int percival_global_params::calib_params_width;
std::string percival_global_params::default_location_Gc;
std::string percival_global_params::default_location_Gf;
std::string percival_global_params::default_location_Oc;
std::string percival_global_params::default_location_Of;
std::string percival_global_params::default_location_ADU_to_electrons_conversion;

std::string percival_global_params::default_location_Gain_lookup_table1;
std::string percival_global_params::default_location_Gain_lookup_table2;
std::string percival_global_params::default_location_Gain_lookup_table3;
std::string percival_global_params::default_location_Gain_lookup_table4;

std::string percival_global_params::default_calib_params_dataset_name;


percival_global_params::percival_global_params(const std::string & master_param_file){
		percival_global_params::initialisation(master_param_file);
}

percival_global_params::percival_global_params(){
	if(!percival_global_params::is_initialised){
	std::string default_master_param_file = "./data/test_param_file.txt";
	percival_global_params::initialisation(default_master_param_file);
	}
}

void percival_global_params::initialisation(const std::string & master_param_file){
	percival_global_params::load_master_param_file(master_param_file);
	if(!percival_global_params::check_initialisation()){
		std::cerr << "Key entries missing in the configuration file. Initialisation Incomplete." << std::endl;
	}
}

bool percival_global_params::load_master_param_file(const std::string & master_param_file){
	std::string line;

	std::ifstream myfile (master_param_file.c_str());

	//std::cout<<"I am in here!" <<std::endl;

	if (myfile.is_open())
	{
		while (! myfile.eof() )
		{
			getline (myfile,line);
			//std::cout << line << std::endl;

			std::string::size_type colon_position = line.find(":");
			if(colon_position!=std::string::npos && (line.find_first_not_of(" ", colon_position+1)!=std::string::npos)){
				if(line.find("is_transposed_calib_params") != std::string::npos){
					percival_global_params::is_transposed_calib_params = (line.find("yes") != std::string::npos);
					is_initialised_every_member[0] = true;
				}
				//
				std::string::size_type quotation_begin = line.find_first_of("\"")+1;
				std::string::size_type quotation_end = line.find_last_of("\"");

				//
				if((line.find("\"")!=std::string::npos) && (line.rfind("\"")!=std::string::npos)){
					if(line.find("default_path_name") != std::string::npos){
						percival_global_params::default_path_name = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[1] = true;
					}
					if(line.find("default_data_set_name") != std::string::npos){
						percival_global_params::default_data_set_name = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[2] = true;
					}
					if(line.find("default_location_Gc") != std::string::npos){
						percival_global_params::default_location_Gc = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[3] = true;
					}
					if(line.find("default_location_Gf") != std::string::npos){
						percival_global_params::default_location_Gf = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[4] = true;
					}
					if(line.find("default_location_Oc") != std::string::npos){
						percival_global_params::default_location_Oc = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[5] = true;
					}
					if(line.find("default_location_Of") != std::string::npos){
						percival_global_params::default_location_Of = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[6] = true;
					}
					if(line.find("default_calib_params_dataset_name") != std::string::npos){
						percival_global_params::default_calib_params_dataset_name = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[7] = true;
					}
					if(line.find("default_location_ADU_to_electrons_conversion") != std::string::npos){
						percival_global_params::default_location_ADU_to_electrons_conversion = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[8] = true;
					}
					if(line.find("default_location_Gain_lookup_table1") != std::string::npos){
						percival_global_params::default_location_Gain_lookup_table1 = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[9] = true;
					}
					if(line.find("default_location_Gain_lookup_table2") != std::string::npos){
						percival_global_params::default_location_Gain_lookup_table2 = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[10] = true;
					}
					if(line.find("default_location_Gain_lookup_table3") != std::string::npos){
						percival_global_params::default_location_Gain_lookup_table3 = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[11] = true;
					}
					if(line.find("default_location_Gain_lookup_table4") != std::string::npos){
						percival_global_params::default_location_Gain_lookup_table4 = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[12] = true;
					}

				}
			}
		}

		myfile.close();
		return 1;
	}
	else {
		std::cerr << "Cannot open file " << master_param_file << std:: endl;
		return 0;
	}
}

bool percival_global_params::check_initialisation(){
	if( (percival_global_params::is_initialised_every_member[3] == false) ||
			(percival_global_params::is_initialised_every_member[4] == false)||
				(percival_global_params::is_initialised_every_member[5] == false)||
					(percival_global_params::is_initialised_every_member[6] == false)||
						(percival_global_params::is_initialised_every_member[7] == false)||
							(percival_global_params::is_initialised_every_member[8] == false))||
								(percival_global_params::is_initialised_every_member[9] == false)||
									(percival_global_params::is_initialised_every_member[10] == false)||
										(percival_global_params::is_initialised_every_member[11] == false))||
											(percival_global_params::is_initialised_every_member[12] == false))
	{
		return 0;
	}
	percival_global_params::is_initialised = true;
	return 1;
}

bool percival_global_params::initialised(){
	return percival_global_params::is_initialised;
}
