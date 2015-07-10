/*
 * main.cpp
 *
 *  Created on: 10 Jul 2015
 *      Author: pqm78245
 */

#include "percival_ADC_decode.h"
#include "percival_frame.h"

#include "H5Cpp.h"
#include <iostream>

#ifndef H5_NO_NAMESPACE
    using namespace H5;
#endif

const H5std_string	FILE_NAME("h5tutr_dset.h5");
const H5std_string	DATASET_NAME("dset");
const int	 NX = 4;                     // dataset dimensions
const int	 NY = 6;
const int	 RANK = 2;

int main (void)
{
    // Try block to detect exceptions raised by any of the calls inside it
    try
    {
	// Turn off the auto-printing when failure occurs so that we can
	// handle the errors appropriately
	Exception::dontPrint();

	// Create a new file using the default property lists.
	H5File file(FILE_NAME, H5F_ACC_TRUNC);

	// Create the data space for the dataset.
	hsize_t dims[2];               // dataset dimensions
	dims[0] = NX;
	dims[1] = NY;
	DataSpace dataspace(RANK, dims);

	// Create the dataset.
	DataSet dataset = file.createDataSet(DATASET_NAME, PredType::STD_I32BE, dataspace);

    }  // end of try block

    // catch failure caused by the H5File operations
    catch(FileIException error)
    {
	error.printError();
	return -1;
    }

    // catch failure caused by the DataSet operations
    catch(DataSetIException error)
    {
	error.printError();
	return -1;
    }

    // catch failure caused by the DataSpace operations
    catch(DataSpaceIException error)
    {
	error.printError();
	return -1;
    }

    return 0;  // successfully terminated
}
