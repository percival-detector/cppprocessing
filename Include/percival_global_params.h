/*
 * percival_global_params.h
 *
 *  Created on: 31 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_GLOBAL_PARAMS_H_
#define INCLUDE_PERCIVAL_GLOBAL_PARAMS_H_

#include "percival_processing.h"
#include "percival_calib_params.h"
#include <fstream>
#include <iostream>

class percival_global_params{
	private:
		static bool is_initialised;
		static bool is_initialised_every_member[255];
		static int current_frame;

		//"KnifeQuadBPos1/";, rather than "KnifeQuadBPos1/!X!/Sample";
		static std::string top_level_data_set_name;

	public:
		//frame and file properties
		static unsigned int total_number_of_frames;

		static unsigned int frame_height;
		static unsigned int frame_width;

		static std::string default_path_name;
		static std::string default_data_set_name;			//make this private

		//calibration parameter properties
		static bool is_transposed_calib_params;
		static unsigned int calib_params_height;
		static unsigned int calib_params_width;

		static std::string default_location_Gc;
		static std::string default_location_Gf;
		static std::string default_location_Oc;
		static std::string default_location_Of;

		static std::string default_calib_params_dataset_name;

		percival_global_params();
		percival_global_params(const std::string & master_param_file);


		void initialisation(const std::string & master_param_file);
		bool load_master_param_file(const std::string & master_param_file);
		bool check_initialisation();

		char* get_current_sample_frame();
		char* get_current_reset_frame();
};


//image height, image width, total frame number, default file locations, calibration height, calibration width, rotated?

#endif /* INCLUDE_PERCIVAL_GLOBAL_PARAMS_H_ */
