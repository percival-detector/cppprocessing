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



int main(int argn, char* argv[]){
	/*
	 * Commandline parameters:
	 *
	 * main use_meaningless_image_no, path_name, dataset_name, repeat
	 * main use_meaningless_image_yes, width repeat
	 * main no commandline input, use default test image.
	 *
	 */

	percival_frame<short> src_frame;
	percival_frame<float> des_frame;

//default location
	char path_name[255]     = "./data/KnifeQuadBPos1_2_21_int16.h5";
	char data_set_name[255] = "KnifeQuadBPos1/10/Sample";
	char config_file[255] = "/home/pqm78245/percivalProcessing/cppProcessing2.0/test_param_file.txt";

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
			*(src_frame.data + i) = i % 32767;
	}else{
		try{
			percival_HDF5_loader(path_name, data_set_name, src_frame);
		}
		catch(file_exception & e){
			std::cerr << e.what() << std::endl;
		}
	}

	for(int i = 0; i < repeat; i++){
		percival_ADC_decode(src_frame, des_frame, calib_params);
		std::cout << i << std::endl;
	}
	std::cout << "done!" << std::endl;
//used for writing output
	percival_HDF5_writer(des_frame, "./generic_output.h5", "output");
	return 0;
}

