/*
 * percival_global_params.cpp
 *
 *  Created on: 31 Jul 2015
 *      Author: pqm78245
 */
#include <cstdlib>

#include <percival_global_params.h>
#include <percival_processing.h>

//image height, image width, total frame number, default file locations, calibration height, calibration width, rotated?

bool percival_global_params::is_initialised = false;
bool percival_global_params::is_initialised_every_member[255] = {};
unsigned int percival_global_params::total_number_of_frames;
unsigned int percival_global_params::frame_height;
unsigned int percival_global_params::frame_width;
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
				if(line.find("total_number_of_frames") != std::string::npos){
					percival_global_params::total_number_of_frames = static_cast<unsigned int>(std::atoi(line.substr(colon_position + 1).c_str()));
					is_initialised_every_member[0] = true;
				}
				if(line.find("frame_height") != std::string::npos){
					percival_global_params::frame_height = static_cast<unsigned int>(std::atoi(line.substr(colon_position + 1).c_str()));
					is_initialised_every_member[1] = true;
				}
				if(line.find("frame_width") != std::string::npos){
					percival_global_params::frame_width = static_cast<unsigned int>(std::atoi(line.substr(colon_position + 1).c_str()));
					is_initialised_every_member[2] = true;
				}
				if(line.find("calib_params_height") != std::string::npos){
					percival_global_params::calib_params_height = static_cast<unsigned int>(std::atoi(line.substr(colon_position + 1).c_str()));
					is_initialised_every_member[3] = true;
				}
				if(line.find("calib_params_width") != std::string::npos){
					percival_global_params::calib_params_width = static_cast<unsigned int>(std::atoi(line.substr(colon_position + 1).c_str()));
					is_initialised_every_member[4] = true;
				}

				if(line.find("is_transposed_calib_params") != std::string::npos){
					percival_global_params::is_transposed_calib_params = (line.find("yes") != std::string::npos);
					is_initialised_every_member[5] = true;
				}
				//
				std::string::size_type quotation_begin = line.find_first_of("\"")+1;
				std::string::size_type quotation_end = line.find_last_of("\"");

				//
				if((line.find("\"")!=std::string::npos) && (line.rfind("\"")!=std::string::npos)){
					if(line.find("default_path_name") != std::string::npos){
						percival_global_params::default_path_name = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[6] = true;
					}
					if(line.find("default_data_set_name") != std::string::npos){
						percival_global_params::default_data_set_name = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[7] = true;
					}
					if(line.find("default_location_Gc") != std::string::npos){
						percival_global_params::default_location_Gc = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[8] = true;
					}
					if(line.find("default_location_Gf") != std::string::npos){
						percival_global_params::default_location_Gf = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[9] = true;
					}
					if(line.find("default_location_Oc") != std::string::npos){
						percival_global_params::default_location_Oc = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[10] = true;
					}
					if(line.find("default_location_Of") != std::string::npos){
						percival_global_params::default_location_Of = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[11] = true;
					}
					if(line.find("default_calib_params_dataset_name") != std::string::npos){
						percival_global_params::default_calib_params_dataset_name = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[12] = true;
					}
					if(line.find("default_location_ADU_to_electrons_conversion") != std::string::npos){
						percival_global_params::default_location_ADU_to_electrons_conversion = line.substr(quotation_begin,quotation_end-quotation_begin);
						is_initialised_every_member[13] = true;
					}

				}
			}
			//			std::cout << std::stoi(" 124 ") << std::endl;
			//			std::cout << percival_global_params::total_number_of_frames << std::endl;

//			std::cout << total_number_of_frames << std::endl;
//			std::cout << frame_height << std::endl;
//			std::cout << frame_width << std::endl;
//			std::cout << default_path_name << std::endl;
//			std::cout << default_data_set_name << std::endl;
//			std::cout << is_transposed_calib_params << std::endl;
//			std::cout << calib_params_height << std::endl;
//			std::cout << calib_params_width << std::endl;
//			std::cout << default_location_Gc << std::endl;
//			std::cout << default_location_Gf << std::endl;
//			std::cout << default_location_Oc << std::endl;
//			std::cout << default_location_Of << std::endl;
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

//total_number_of_frames
//is_initialised_every_member[0] = true;
//frame_height
//is_initialised_every_member[1] = true;
//frame_width
//is_initialised_every_member[2] = true;
//calib_params_height
//is_initialised_every_member[3] = true;
//calib_params_width
//is_initialised_every_member[4] = true;
//is_transposed_calib_params
//is_initialised_every_member[5] = true;
//
//default_path_name
//is_initialised_every_member[6] = true;
//default_data_set_name
//is_initialised_every_member[7] = true;
//default_location_Gc
//is_initialised_every_member[8] = true;
//default_location_Gf
//is_initialised_every_member[9] = true;
//default_location_Oc
//is_initialised_every_member[10] = true;
//default_location_Of
//is_initialised_every_member[11] = true;

	if((percival_global_params::is_initialised_every_member[6] == false) ||
			(percival_global_params::is_initialised_every_member[7] == false) ||
				(percival_global_params::is_initialised_every_member[8] == false) ||
					(percival_global_params::is_initialised_every_member[9] == false) ||
						(percival_global_params::is_initialised_every_member[10] == false) ||
							(percival_global_params::is_initialised_every_member[11] == false)||
								(percival_global_params::is_initialised_every_member[12] == false)||
									(percival_global_params::is_initialised_every_member[13] == false)){
		return 0;
	}
//	else if((percival_global_params::is_initialised_every_member[0] == false) ||
//					(percival_global_params::is_initialised_every_member[1] == false)){
//
//
//	}else if((percival_global_params::is_initialised_every_member[3] == false) ||
//				(percival_global_params::is_initialised_every_member[4] == false)){
//
//	}else if(percival_global_params::is_initialised_every_member[5] == false){
//
//	}
	percival_global_params::is_initialised = true;
	return 1;
}

bool percival_global_params::initialised(){
	return percival_global_params::is_initialised;
}
