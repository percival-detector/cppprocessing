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
	 * main no commandline input, all default
	 *
	 */

	percival_frame<short> src_frame;
	percival_frame<float> des_frame;

//default location
	char path_name[255]     = "./data/KnifeQuadBPos1_2_21_int16.h5";
	char data_set_name[255] = "KnifeQuadBPos1/10/Sample";

	int width, height;
	int repeat=10;

//used for profiling
	height = 160;		//fixed
	bool use_meaningless_image = true;

	if(argn == 1){
		//default values
		width = 1024;
		src_frame.set_frame_size(height, width);
	}	//no commandline parameter
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

				try{
					percival_HDF5_loader(path_name, data_set_name, src_frame);
				}
				catch(file_exception & e){
					std::cerr << e.what() << std::endl;
				}
				std::cout << "Here" << std::endl;
				if(argn == 5)
					sscanf(argv[4], "%d", &repeat);
			}
		}// using meaningful data to test
	}


//	std::cout << argn << std::endl;
//	std::cout << src_frame.width << std::endl;
//	std::cout << repeat << std::endl;
//	std::cout << height << std::endl;
//	std::cout << argn << std::endl;

	percival_global_params global_params("/home/pqm78245/percivalProcessing/cppProcessing2.0/test_param_file.txt");

	if(use_meaningless_image){
		for(int i = 0; i < width * height; i++  )
			*(src_frame.data + i) = i % 32767;
	}

	percival_calib_params params;
	for(int i = 0; i < repeat; i++){
		percival_ADC_decode(src_frame, des_frame);

	}
	std::cout << std::stoi(" 124 ") << std::endl;

	std::cout << "done!" << std::endl;


//used for writing output
//	percival_HDF5_loader(path_name,data_set_name, src_frame);
//	des_frame.set_frame_size(src_frame.height, src_frame.width);
//	percival_ADC_decode(src_frame, des_frame);
//
//	std::string FILE_NAME = "./data/Output.h5";
//	H5File write_to_file(FILE_NAME,H5F_ACC_TRUNC);
//
//	hsize_t dims[2];
//	dims[0] = des_frame.height;	dims[1] = des_frame.width ;
//	DataSpace int_dataspace(2,dims);
//
//	DataSet dataset_int = write_to_file.createDataSet("Output", PredType::IEEE_F32LE, int_dataspace);
//	dataset_int.write(des_frame.data, PredType::IEEE_F32LE);
//	dataset_int.close();
//	write_to_file.close();
	return 0;
}

