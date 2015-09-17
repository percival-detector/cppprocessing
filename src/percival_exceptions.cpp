/*
 * percival_exceptions.cpp
 *
 *  Created on: 12 Aug 2015
 *      Author: pqm78245
 */

#include "percival_exceptions.h"
#include <sstream>
#include <iostream>

file_exception ::file_exception(const char *file_name, const char *msg) : err_msg(msg), file_name(file_name) {};
file_exception ::~file_exception() throw() {};
const char *file_exception ::what() const throw() { std::string message = "\"" + file_name + "\"" + err_msg; return message.c_str(); };

datatype_exception::datatype_exception(const int size, const char* type, const bool sign, const char *msg) :
				size(size),
				type(type),
				sign(sign),
				err_msg(msg)
{
	if(sign == true)
		sign_string = "signed";
	else
		sign_string = "unsigned";

	std::ostringstream str_convert;
	str_convert << (long long int)(size * 8 );
	size_string = str_convert.str();

	return_message = err_msg
			+ "\nActual input datatype: "
			+  size_string + " bits " + sign_string + " "+ type + ".\n"
			+ "Datatypes permitted are:\n"
			+ "H5T_STD_U16LE\nH5T_STD_U32LE\nH5T_IEEE_F32LE\nH5T_IEEE_F64LE\n";
};

datatype_exception::datatype_exception(const char *msg) : return_message(msg){};

datatype_exception::~datatype_exception() throw() {};
const char *datatype_exception::what() const throw() {return return_message.c_str();};


dataspace_exception::dataspace_exception(const char *msg) : err_msg(msg) {};
dataspace_exception::~dataspace_exception() throw() {};
const char *dataspace_exception::what() const throw() {return err_msg.c_str();};
