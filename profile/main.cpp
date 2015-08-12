/*
 * main.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */
#include "percival_processing.h"
#include "percival_load_calib_params.h"
#include "percival_HDF5_loader.h"
#include "percival_HDF5_writer.h"
#include<string>
#include<cstdio>
#include<iostream>

//todo there is a problem in converting from combined hdf datatype to int 16 hdf5 file
//particularly, gain bits in Hazem's source range from 1 - 4. In Arvindar's code, it was assumed that the gain bits range from 0 - 3
//this problem is significant when the actual gain bit goes beyond 3 such that the fine bits are affected.
//urgent need correction!!!

int main(int argn, char* argv[]){
	/*
	 * Commandline parameters:
	 *
	 * main use_meaningless_image_no, path_name, dataset_name, repeat
	 * main use_meaningless_image_yes, width repeat
	 * main no commandline input, use default test image.
	 *
	 */

	percival_frame<short> sample_frame;
	percival_frame<short> reset_frame;
	percival_frame<float> ADC_decoded_sample_frame;
	percival_frame<float> ADC_decoded_reset_frame;
	percival_frame<float> CDS_frame;
	percival_frame<float> electron_corrected_frame;

//default location
	std::string path_name     = "./data/KnifeQuadBPos1_2_21_int16.h5";
	std::string data_set_name = "KnifeQuadBPos1/10/Sample";

	std::string top_level_data_set_name  = "KnifeQuadBPos1/";

	std::string sample_data_set_name = "KnifeQuadBPos1/10/Sample";
	std::string reset_data_set_name = "KnifeQuadBPos1/9/Reset";


	std::string config_file = "./data/test_param_file.txt";

	int width, height;
	int repeat=1000;

//used for profiling
	height = 160;		//fixed
	bool use_meaningless_image = false;

	if(argn >= 2){
		sscanf(argv[1], "%d", &use_meaningless_image);
		if(use_meaningless_image){
			sscanf(argv[2], "%d", &width);
			if(argn >= 4)
				sscanf(argv[3], "%d", &height);
			if(argn == 6){
				sscanf(argv[4], "%d", &repeat);
				std::cout << "I am here." << std::endl;
				char tmp3[255];
				sscanf(argv[5], "%s", tmp3);
				config_file = tmp3;
			}
		}//using computer generated data
		else{
			if(argn >= 3){
				char tmp1[255];
				char tmp2[255];
				sscanf(argv[2], "%s", tmp1);
				sscanf(argv[3], "%s", tmp2);
				path_name = tmp1;
				top_level_data_set_name = tmp2;
				if(argn >= 5)
					sscanf(argv[4], "%d", &repeat);
			}
		}// using meaningful data to test
	}

	percival_global_params global_params(config_file);
	percival_calib_params calib_params;
	percival_load_calib_params(calib_params, global_params);

	if(use_meaningless_image){
		sample_frame.set_frame_size(height, width);
		reset_frame.set_frame_size(height, width);

		for(int i = 0; i < width * height; i++  ){
			*(sample_frame.data + i) = i % 32767;
			*(reset_frame.data + i) = i % 32766;
		}
	}else{

		try{
			percival_HDF5_loader(path_name.c_str(), (top_level_data_set_name + "10/Sample").c_str(), sample_frame);
			percival_HDF5_loader(path_name.c_str(), (top_level_data_set_name + "9/Reset").c_str(), reset_frame);
		}
		catch(file_exception & e){
			std::cerr << e.what() << std::endl;
		}
	}

	width = sample_frame.width;
	height = sample_frame.height;

	reset_frame.set_frame_size(height, width);
	ADC_decoded_sample_frame.set_frame_size(height, width);
	ADC_decoded_reset_frame.set_frame_size(height, width);
	CDS_frame.set_frame_size(height, width);
	electron_corrected_frame.set_frame_size(height, width);


	for(int i = 0; i < repeat; i++){
		percival_ADC_decode(sample_frame, ADC_decoded_sample_frame, calib_params);
		percival_ADC_decode(reset_frame, ADC_decoded_reset_frame, calib_params);

		percival_CDS_correction(ADC_decoded_sample_frame, ADC_decoded_reset_frame, CDS_frame);
		percival_ADU_to_electron_correction(CDS_frame, electron_corrected_frame, calib_params);

//		std::cout << i << std::endl;
	}

	std::cout << "done!" << std::endl;
//used for writing output
	percival_HDF5_writer(ADC_decoded_sample_frame,  "./12-08-2015 output.h5", "ADC_decoded_sample_frame");
	percival_HDF5_writer(ADC_decoded_reset_frame,  "./12-08-2015 output.h5", "ADC_decoded_reset_frame");
	percival_HDF5_writer(CDS_frame, "./12-08-2015 output.h5", "CDS_frame");
	percival_HDF5_writer(electron_corrected_frame,  "./12-08-2015 output.h5", "electron_corrected_frame");

//	std::cout << *(calib_params.ADU_to_electrons_conversion.data + 2) << std::endl;
	percival_HDF5_writer(calib_params.Gc,  "./12-08-2015 output.h5", "calib_params_Gc");
	percival_HDF5_writer(calib_params.Gf,  "./12-08-2015 output.h5", "calib_params_Gf");
	percival_HDF5_writer(calib_params.Oc,  "./12-08-2015 output.h5", "calib_params_Oc");
	percival_HDF5_writer(calib_params.Of,  "./12-08-2015 output.h5", "calib_params_Of");
	percival_HDF5_writer(calib_params.ADU_to_electrons_conversion,  "./12-08-2015 output.h5", "ADU_to_electrons_conversion");

	return 0;
}

