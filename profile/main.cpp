/*
 * main.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */
#include "percival_processing.h"

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
	char path_name[255]     = "./data/KnifeQuadBPos1_2_21_int16.h5";
	char data_set_name[255] = "KnifeQuadBPos1/10/Sample";

	char sample_data_set_name[255] = "KnifeQuadBPos1/10/Sample";
	char reset_data_set_name[255] = "KnifeQuadBPos1/9/Reset";

	char top_level_data_set_name[255]  = "KnifeQuadBPos1/";
	char config_file[255] = "./test_param_file.txt";

	int width, height;
	int repeat=10;

//used for profiling
	height = 160;		//fixed
	bool use_meaningless_image = false;

	if(argn >= 2){
		sscanf(argv[1], "%d", &use_meaningless_image);
		if(use_meaningless_image){
			sscanf(argv[2], "%d", &width);
			if(argn == 4)
				sscanf(argv[3], "%d", &repeat);
		}//using computer generated data
		else{
			if(argn >= 3){
				sscanf(argv[2], "%s", path_name);
				sscanf(argv[3], "%s", data_set_name);
				if(argn == 5)
					sscanf(argv[4], "%d", &repeat);
			}
		}// using meaningful data to test
	}

	percival_global_params global_params(config_file);
	percival_calib_params calib_params(global_params);

	if(use_meaningless_image){
		for(int i = 0; i < width * height; i++  )
			*(sample_frame.data + i) = i % 32767;
	}else{

		try{
			percival_HDF5_loader(path_name, sample_data_set_name, sample_frame);
			percival_HDF5_loader(path_name, reset_data_set_name, reset_frame);
		}
		catch(file_exception & e){
			std::cerr << e.what() << std::endl;
		}
	}


	for(int i = 0; i < repeat; i++){
		percival_ADC_decode(sample_frame, ADC_decoded_sample_frame, calib_params);
		percival_ADC_decode(reset_frame, ADC_decoded_reset_frame, calib_params);

		percival_CDS_correction(ADC_decoded_sample_frame, ADC_decoded_reset_frame, CDS_frame);
		percival_ADU_to_electron_correction(CDS_frame, electron_corrected_frame, calib_params);

		std::cout << i << std::endl;
	}

	std::cout << "done!" << std::endl;
//used for writing output
	percival_HDF5_writer(ADC_decoded_sample_frame,  "./04-08-2015 output.h5", "ADC_decoded_sample_frame");
	percival_HDF5_writer(ADC_decoded_reset_frame,  "./04-08-2015 output.h5", "ADC_decoded_reset_frame");
	percival_HDF5_writer(CDS_frame, "./04-08-2015 output.h5", "CDS_frame");
	percival_HDF5_writer(electron_corrected_frame,  "./04-08-2015 output.h5", "electron_corrected_frame");

//	std::cout << *(calib_params.ADU_to_electrons_conversion.data + 2) << std::endl;
	percival_HDF5_writer(calib_params.Gc,  "./04-08-2015 output.h5", "calib_params_Gc");
	percival_HDF5_writer(calib_params.Gf,  "./04-08-2015 output.h5", "calib_params_Gf");
	percival_HDF5_writer(calib_params.Oc,  "./04-08-2015 output.h5", "calib_params_Oc");
	percival_HDF5_writer(calib_params.Of,  "./04-08-2015 output.h5", "calib_params_Of");
	percival_HDF5_writer(calib_params.ADU_to_electrons_conversion,  "./04-08-2015 output.h5", "ADU_to_electrons_conversion");

	return 0;
}

