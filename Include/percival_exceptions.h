/*
 * percival_exceptions.h
 *
 *  Created on: 12 Aug 2015
 *      Author: pqm78245
 */

#ifndef INCLUDE_PERCIVAL_EXCEPTIONS_H_
#define INCLUDE_PERCIVAL_EXCEPTIONS_H_

#include <exception>
#include <string>


class file_exception : public std::exception{
private:
	std::string err_msg;
	std::string file_name;
public:
	file_exception(const char *file_name, const char *msg);
	~file_exception() throw();
	const char *what() const throw();
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
	datatype_exception(const int size, const char* type, const bool sign, const char *msg);
	datatype_exception(const char *msg);

	~datatype_exception() throw();
	const char *what() const throw();
};

class dataspace_exception : public std::exception{
private:
	std::string err_msg;

public:
	dataspace_exception(const char *msg);
	~dataspace_exception() throw();
	const char *what() const throw();
};









#endif /* INCLUDE_PERCIVAL_EXCEPTIONS_H_ */
