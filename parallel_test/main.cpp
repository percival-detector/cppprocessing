/*
 * main.cpp
 *
 *  Created on: 18 Aug 2015
 *      Author: pqm78245
 */
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
#include "percival_parallel.h"

#include "tbb/task_scheduler_init.h"

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
	 * main use_meaningless_image_yes, width height repeat test_params_file
	 * main no commandline input, use default test image.
	 *
	 * Maximum number of tokens
	 * Number of threads
	 * grain_size
	 *
	 *
	 */

	percival_frame<unsigned short int> sample_frame;
	percival_frame<unsigned short int> reset_frame;
	percival_frame<float> ADC_decoded_sample_frame;
	percival_frame<float> ADC_decoded_reset_frame;
	percival_frame<float> CDS_frame;
	percival_frame<float> electron_corrected_frame;


	percival_frame<unsigned short int> sample_coarse_frame;
	percival_frame<unsigned short int> sample_fine_frame;
	percival_frame<unsigned short int> sample_gain_frame;

	percival_frame<unsigned short int> reset_coarse_frame;
	percival_frame<unsigned short int> reset_fine_frame;
	percival_frame<unsigned short int> reset_gain_frame;

	percival_frame<float> calibrated_sample_frame;
	percival_frame<float> calibrated_reset_frame;


//default location
	std::string path_name     = "./data/KnifeQuadBPos1_2_21_int16.h5";
	std::string data_set_name = "KnifeQuadBPos1/10/Sample";

	std::string top_level_data_set_name  = "KnifeQuadBPos1/";

	std::string sample_data_set_name = "KnifeQuadBPos1/10/Sample";
	std::string reset_data_set_name = "KnifeQuadBPos1/9/Reset";


	std::string config_file = "./data/test_param_file.txt";

	int width, height;
	int repeat=100;
	unsigned int grain_size = 3528;
	unsigned int execute_function = 1;
	unsigned int max_tokens = 20;
	unsigned int max_threads = 20;
//used for profiling
	height = 160;		//fixed
	bool use_meaningless_image = false;

	if(argn >= 2){
		sscanf(argv[1], "%d", &use_meaningless_image);
		if(use_meaningless_image){
			sscanf(argv[2], "%d", &width);
			if(argn >= 4)
				sscanf(argv[3], "%d", &height);
			if(argn >= 6){
				sscanf(argv[4], "%d", &repeat);
				char tmp3[255];
				sscanf(argv[5], "%s", tmp3);
				config_file = tmp3;
			}
			if(argn >= 7){
				sscanf(argv[6], "%d", &grain_size);
				if(argn >= 8){
					sscanf(argv[7], "%d", &execute_function);
					if(argn >= 9 ){
						sscanf(argv[8], "%d", &max_tokens);
						if(argn >= 10 ){
							sscanf(argv[9], "%d", &max_threads);
						}
					}

				}
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
	percival_load_calib_params(calib_params, global_params, true);

	/*settin the number of threads to use*/
	tbb::task_scheduler_init init();

	if(use_meaningless_image){
		/*14 images per iteration*/
		percival_frame_mem<unsigned short int>* sample_frame_stack= new percival_frame_mem<unsigned short int>[repeat];
		percival_frame_mem<unsigned short int>* reset_frame_stack= new percival_frame_mem<unsigned short int>[repeat];
		percival_frame_mem<float>* ADC_decoded_sample_frame_stack= new percival_frame_mem<float>[repeat];
		percival_frame_mem<float>* ADC_decoded_reset_frame_stack= new percival_frame_mem<float>[repeat];

		percival_frame<unsigned short int>* sample_frame_stack_pipe = new percival_frame<unsigned short int>[repeat];
		percival_frame<unsigned short int>* reset_frame_stack_pipe = new percival_frame<unsigned short int>[repeat];
		percival_frame<float>* ADC_decoded_sample_frame_stack_pipe = new percival_frame<float>[repeat];
		percival_frame<float>* ADC_decoded_reset_frame_stack_pipe = new percival_frame<float>[repeat];

		percival_frame_mem<float>* CDS_frame_stack= new percival_frame_mem<float>[repeat];
		percival_frame_mem<float>* electron_corrected_frame_stack= new percival_frame_mem<float>[repeat];


		percival_frame_mem<unsigned short int>* sample_coarse_frame_stack= new percival_frame_mem<unsigned short int>[repeat];
		percival_frame_mem<unsigned short int>* sample_fine_frame_stack= new percival_frame_mem<unsigned short int>[repeat];
		percival_frame_mem<unsigned short int>* sample_gain_frame_stack= new percival_frame_mem<unsigned short int>[repeat];

		percival_frame_mem<unsigned short int>* reset_coarse_frame_stack= new percival_frame_mem<unsigned short int>[repeat];
		percival_frame_mem<unsigned short int>* reset_fine_frame_stack= new percival_frame_mem<unsigned short int>[repeat];
		percival_frame_mem<unsigned short int>* reset_gain_frame_stack= new percival_frame_mem<unsigned short int>[repeat];

		percival_frame_mem<float>* calibrated_sample_frame_stack= new percival_frame_mem<float>[repeat];
		percival_frame_mem<float>* calibrated_reset_frame_stack= new percival_frame_mem<float>[repeat];


		for(int j = 0; j < repeat; j ++){
			 (*(sample_frame_stack + j)).set_frame_size(height, width);
			 (*(reset_frame_stack + j)).set_frame_size(height, width);
			 (*(ADC_decoded_sample_frame_stack + j)).set_frame_size(height, width);
			 (*(ADC_decoded_reset_frame_stack + j)).set_frame_size(height, width);

			 *(sample_frame_stack_pipe + j) = *(sample_frame_stack + j);
			 *(ADC_decoded_sample_frame_stack_pipe + j) = *(ADC_decoded_sample_frame_stack + j);
			 *(reset_frame_stack_pipe + j) = *(reset_frame_stack + j);
			 *(ADC_decoded_reset_frame_stack_pipe + j) = *(ADC_decoded_reset_frame_stack + j);

			 (*(CDS_frame_stack + j)).set_frame_size(height, width);
			 (*(	electron_corrected_frame_stack + j)).set_frame_size(height, width);

			 (*(	sample_coarse_frame_stack + j)).set_frame_size(height, width);
			 (*(	sample_fine_frame_stack + j)).set_frame_size(height, width);
			 (*(	sample_gain_frame_stack + j)).set_frame_size(height, width);

			 (*(	reset_coarse_frame_stack + j)).set_frame_size(height, width);
			 (*(	reset_fine_frame_stack + j)).set_frame_size(height, width);
			 (*(	reset_gain_frame_stack + j)).set_frame_size(height, width);

			 (*(	calibrated_sample_frame_stack + j)).set_frame_size(height, width);
			 (*(	calibrated_reset_frame_stack + j)).set_frame_size(height, width);


			/*unit functions*/
//			percival_unit_ADC_decode(sample_frame, sample_coarse_frame, sample_fine_frame,sample_gain_frame);
//			percival_unit_ADC_decode(reset_frame, reset_coarse_frame, reset_fine_frame,reset_gain_frame);
//
//			percival_unit_ADC_calibration(sample_coarse_frame, sample_fine_frame, calibrated_sample_frame, calib_params);
//			percival_unit_ADC_calibration(reset_coarse_frame, reset_fine_frame, calibrated_reset_frame, calib_params);
//
//			percival_unit_gain_multiplication(sample_frame, calibrated_sample_frame, ADC_decoded_sample_frame, calib_params);
//			percival_unit_gain_multiplication(reset_frame, calibrated_reset_frame, ADC_decoded_reset_frame, calib_params);
//
//			percival_ADC_decode_pipe(sample_frame, ADC_decoded_sample_frame ,calib_params, sample_gain_frame, sample_fine_frame, sample_coarse_frame, calibrated_sample_frame);
//			percival_ADC_decode_pipe(reset_frame, ADC_decoded_reset_frame ,calib_params, reset_gain_frame, reset_fine_frame, reset_coarse_frame, calibrated_reset_frame);
//

		}

		for(unsigned int k = 0; k < 10; ++k){
			for(int j = 0; j < repeat; j ++){	/*assign values first so that they do not remain in the cache*/
				sample_frame = *(sample_frame_stack + j);
				reset_frame = *(reset_frame_stack + j);

				for(int i = 0; i < width * height; i++  ){
					*(sample_frame.data + i) = i + (j + 3 + k) % 32767;		/*arbitrarily generate source data */
					*(reset_frame.data + i) = i + (j + 5 + k) % 32767;
				}
			}
			/*then do the computation*/
			for(int j = 0; j < repeat; j ++){
				sample_frame = *(sample_frame_stack + j);
				reset_frame = *(reset_frame_stack + j);

				ADC_decoded_sample_frame =*( ADC_decoded_sample_frame_stack + j);
				ADC_decoded_reset_frame =*( ADC_decoded_reset_frame_stack + j);
				CDS_frame = *(CDS_frame_stack + j);
				electron_corrected_frame = *(electron_corrected_frame_stack + j);

				sample_coarse_frame = *(sample_coarse_frame_stack + j);
				sample_fine_frame = *(sample_fine_frame_stack + j);
				sample_gain_frame = *(sample_gain_frame_stack + j);

				reset_coarse_frame = *(reset_coarse_frame_stack + j);
				reset_fine_frame = *( reset_fine_frame_stack+ j);
				reset_gain_frame = *(reset_gain_frame_stack + j);

				calibrated_sample_frame = *(calibrated_sample_frame_stack + j);
				calibrated_reset_frame = *(calibrated_reset_frame_stack + j);
				/*functions to run*/
//				percival_ADC_decode_pf(sample_frame, ADC_decoded_sample_frame ,calib_params);
//				percival_ADC_decode_pf(reset_frame, ADC_decoded_reset_frame ,calib_params);
//				std::cout << j << " " << *(ADC_decoded_sample_frame.data + 1000) << std::endl;
//				percival_ADC_decode(sample_frame, ADC_decoded_sample_frame ,calib_params);
//				percival_ADC_decode(reset_frame, ADC_decoded_reset_frame ,calib_params);

//				percival_ADC_decode_pf_combined_tbb_pipeline1(sample_frame, ADC_decoded_sample_frame ,calib_params, grain_size);
//				percival_ADC_decode_pf_combined_tbb_pipeline1(reset_frame, ADC_decoded_reset_frame ,calib_params, grain_size);
//				percival_CDS_correction(ADC_decoded_sample_frame, ADC_decoded_reset_frame, electron_corrected_frame);
				if(execute_function)
					percival_ADC_decode_combined_pipeline(sample_frame, reset_frame, CDS_frame, calib_params, grain_size, max_tokens);
//				percival_ADC_decode_pf_unit_combined_tbb_pipeline1(sample_frame, ADC_decoded_sample_frame ,calib_params, sample_gain_frame, sample_fine_frame, sample_coarse_frame, calibrated_sample_frame, 3528/7);
//				percival_ADC_decode_pf_unit_combined_tbb_pipeline1(reset_frame, ADC_decoded_reset_frame ,calib_params, reset_gain_frame, reset_fine_frame, reset_coarse_frame, calibrated_reset_frame, 3528/7);
			}
//			percival_ADC_decode_pf_combined_tbb_pipeline_stream(sample_frame_stack_pipe, ADC_decoded_sample_frame_stack_pipe ,calib_params, 2, repeat);
//			percival_ADC_decode_pf_combined_tbb_pipeline_stream(reset_frame_stack_pipe, ADC_decoded_reset_frame_stack_pipe ,calib_params, 2, repeat);
		}
	}else{
		try{
//			percival_HDF5_loader(path_name.c_str(), (top_level_data_set_name + "10/Sample").c_str(), sample_frame);
//			percival_HDF5_loader(path_name.c_str(), (top_level_data_set_name + "9/Reset").c_str(), reset_frame);
		}
		catch(file_exception & e){
			std::cerr << e.what() << std::endl;
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
		}
	////used for writing output
	//	percival_HDF5_writer(ADC_decoded_sample_frame,  "./12-08-2015 output.h5", "ADC_decoded_sample_frame");
	//	percival_HDF5_writer(ADC_decoded_reset_frame,  "./12-08-2015 output.h5", "ADC_decoded_reset_frame");
	//	percival_HDF5_writer(CDS_frame, "./12-08-2015 output.h5", "CDS_frame");
	//	percival_HDF5_writer(electron_corrected_frame,  "./12-08-2015 output.h5", "electron_corrected_frame");
	//
	////	std::cout << *(calib_params.ADU_to_electrons_conversion.data + 2) << std::endl;
	//	percival_HDF5_writer(calib_params.Gc,  "./12-08-2015 output.h5", "calib_params_Gc");
	//	percival_HDF5_writer(calib_params.Gf,  "./12-08-2015 output.h5", "calib_params_Gf");
	//	percival_HDF5_writer(calib_params.Oc,  "./12-08-2015 output.h5", "calib_params_Oc");
	//	percival_HDF5_writer(calib_params.Of,  "./12-08-2015 output.h5", "calib_params_Of");
	//	percival_HDF5_writer(calib_params.ADU_to_electrons_conversion,  "./12-08-2015 output.h5", "ADU_to_electrons_conversion");

	}
	std::cout << "Done!" << std::endl;
	return 0;
}


