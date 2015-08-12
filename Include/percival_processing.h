/*
 * percival_processing.h
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_PROCESSING_H_
#define INCLUDE_PERCIVAL_PROCESSING_H_
//todo:find a better way for forward declaration
class percival_global_params;
struct percival_calib_params;
class file_exception;
class datatype_exception;
class dataspace_exception;


//data classes
#include "percival_frame.h"
#include "percival_calib_params.h"
#include "percival_global_params.h"

//functions
#include "percival_ADC_decode.h"
#include "percival_CDS_correction.h"
#include "percival_ADU_to_electron_correction.h"
#include "percival_HDF5_loader.h"
#include "percival_HDF5_writer.h"
#include "percival_load_calib_params.h"
//STL libraries





#endif /* INCLUDE_PERCIVAL_PROCESSING_H_ */
