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

const char* path_name = "./data/KnifeQuadBPos1_2_21_int16.h5";
const char* data_set_name = "KnifeQuadBPos1/10/Sample";

int main(int argn, char* argv[]){
	percival_frame<short> src_frame;
	percival_frame<float> des_frame;
//used for profiling
	int repeat, width;
	int height = 160;

	if(argn == 2){
		sscanf(argv[1], "%d", &width);
		repeat = 10;
	}
	if(argn == 3){
		sscanf(argv[1], "%d", &width);
		sscanf(argv[2], "%d", &repeat);
	}

	src_frame.set_frame_size(height, width);

	for(int i = 0; i < width * height; i++  )
		*(src_frame.data + i) = i % 32767;

	//percival_HDF5_loader(path_name,data_set_name, src_frame);
	des_frame.set_frame_size(height, width);
	percival_calib_params params;

	for(int i = 0; i < repeat; i++){
		percival_ADC_decode(src_frame, des_frame);
	}

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

