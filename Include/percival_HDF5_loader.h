/*
 * percival_HDF5_loader.h
 *
 *  Created on: 14 Jul 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_HDF5_LOADER_H_
#define INCLUDE_PERCIVAL_HDF5_LOADER_H_

#include "percival_processing.h"

#include "hdf5.h"
#include <stdexcept>
#include <exception>
#include <string>
#include <typeinfo>
#include <iostream>

class file_exception : public std::exception{
private:
	std::string err_msg;
	std::string file_name;
public:
	file_exception(const char *file_name, const char *msg) : file_name(file_name), err_msg(msg) {};
	~file_exception() throw() {};
	const char *what() const throw() { std::string message = "\"" + file_name + "\"" + err_msg; return message.c_str(); };
};

class datatype_exception : public std::exception{
private:
	std::string err_msg;
	std::string type;
	std::string sign_string;
	std::string return_message;
	std::string size_string;
	int size;
	bool sign;


public:
	datatype_exception(const int size, const char* type, const bool sign, const char *msg) :
		size(size),
		type(type),
		sign(sign),
		err_msg(msg)
{
		if(sign == true)
			sign_string = "signed";
		else
			sign_string = "unsigned";

		size_string = std::to_string((long long int)(size * 8 ));

		return_message = err_msg
				+ "\nActual input datatype: "
				+  size_string + " bits " + sign_string + " "+ type + ".\n"
				+ "Datatypes permitted are:\n"
				+ "H5T_STD_U16LE\nH5T_STD_U32LE\nH5T_IEEE_F32LE\nH5T_IEEE_F64LE\n";
};

	datatype_exception(const char *msg) : return_message(msg){};

	~datatype_exception() throw() {};
	const char *what() const throw() {return return_message.c_str();};
};

class dataspace_exception : public std::exception{
private:
	std::string err_msg;

public:
	dataspace_exception(const char *msg) : err_msg(msg) {};
	~dataspace_exception() throw() {};
	const char *what() const throw() {return err_msg.c_str();};
};

template<typename T>
void percival_HDF5_loader(
		const char * path_name,
		const char * data_set_name,
		percival_frame<T> & buffer_frame,
		bool print_error = 0			//default 0
){
	herr_t status;
	hid_t file_id, dataset_id, dataspace_id, datatype_id;

	if(!print_error)
	/* Turn off error handling */
	  H5Eset_auto(H5E_DEFAULT, NULL, NULL);

	/*
	 *
	 * open HDF5 files and datasets
	 *
	 */
	file_id = H5Fopen(path_name,H5F_ACC_RDONLY, H5P_DEFAULT);
	if(file_id<0){
		H5close();
		throw file_exception{path_name, " does not exist or is not an HDF5 file. Failed to open."};
	}
	//should_throw_exception_if_dataset_does_not_exist
	dataset_id = H5Dopen2(file_id, data_set_name, H5P_DEFAULT);
	if(dataset_id<0){
		H5close();
		throw file_exception{data_set_name, " dataset does not exist. Failed to open."};
	}

	dataspace_id = H5Dget_space(dataset_id);
	if(dataspace_id<0){
		H5close();
		throw file_exception{path_name, " Fail to get dataspace."};
	}
	/*
	 *
	 *
	 * examining data properties, dimension, type, size, order etc
	 *
	 */
	int rank;
	rank = H5Sget_simple_extent_ndims(dataspace_id);
	if(rank != 2){
		H5close();
		throw dataspace_exception{"Dimension != 2."};
	}
	hsize_t* current_dims, *maximum_dims;

	current_dims = new hsize_t[rank];
	maximum_dims = new hsize_t[rank];

	//Alternative C syntax
	//current_dims = (hsize_t*)malloc(rank*sizeof(hsize_t));
	//maximum_dims = (hsize_t*)malloc(rank*sizeof(hsize_t));

	H5Sget_simple_extent_dims(dataspace_id, current_dims, maximum_dims);
	buffer_frame.set_frame_size(current_dims[0], current_dims[1]);
	//
	//
	delete [] current_dims;
	delete [] maximum_dims;

	datatype_id = H5Dget_type(dataset_id);

	int size = H5Tget_size(datatype_id);
	//
	//	if(size != sizeof(T))
	//		throw datatype_exception{};

	if(H5Tget_order(datatype_id)==H5T_ORDER_BE){
		H5close();
		throw datatype_exception{"Only little endian data is permitted."};
	}

	hid_t memtype_id;
	H5T_class_t data_class = H5Tget_class(datatype_id);
	H5T_sign_t sign = H5Tget_sign(datatype_id);


	if(data_class == H5T_INTEGER){
		if((size == 2)&& (typeid(T) == typeid(short int)))
			memtype_id = H5T_STD_U16LE;
		else if((size == 4) && (typeid(T) == typeid(int))){ //&& (sign == 0)
			memtype_id = H5T_STD_U32LE;

		}
		else{
			H5close();
			throw datatype_exception{size, "int" , sign, "Invalid input datatype or wrong destination datatype."};
		}
	}
	else if(data_class == H5T_FLOAT){
		if( (size == 4) && (typeid(T) == typeid(float)))
			memtype_id = H5T_IEEE_F32LE;
		else if((size == 8) && (typeid(T) == typeid(double)))
			memtype_id = H5T_IEEE_F64LE;
		else{
			H5close();
			throw datatype_exception{size, "float" , sign, "Invalid input datatype or wrong destination datatype."};
		}
	}
	else{
		throw datatype_exception{"Invalid input datatype or wrong destination datatype."};
		H5close();
	}

	//only little endian, unsigned int, unsigned short int, single and double float are accepted

	status = H5Dread (dataset_id, memtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer_frame.data);	//todo change these four read statement into one

	if(status < 0){
		H5close();
		throw file_exception{path_name, " cannot be read."};
	}

	H5close();
}

#endif /* INCLUDE_PERCIVAL_HDF5_LOADER_H_ */



//Specification
//	1. should take in
//		- path to HDF5 files
//		- pointer to a plain percival_frame buffer
//		- possibly other options for file.read()
//		- a property struct detailing the metadata of the HDF5 file
//	2. should write to the plain image buffer
//		- integrity of the data should be preserved
//	3. should write to the property struct
//		- should preserve data integrity
//	4. should close HDF5 file properly
//	5. should automatically check HDF5 image size and set input percival_frame to be of the same size
