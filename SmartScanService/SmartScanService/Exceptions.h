/*
Custom exceptions for the Smart Scan Service
Written by Eduard Cazacu in December 2020
*/
#pragma once

#include <exception>
#include <string>

namespace SmartScan 
{
    // Custom exceptions for the Smart Scan Service class.
	class ex_smartScan : public std::exception 
    {
	public:
		ex_smartScan(const char* what_arg, const char* function, const char* file) :
			what_arg{ what_arg }, function{ function }, file{ file }
		{
		}

		const char* get_file() {
			return this->file;
		}

		const char* get_function() {
			return this->function;
		}

		const char* what() {
			return this->what_arg;
		}
	private:
		const char* what_arg;
		const char* function;
		const char* file;
	};

    // Custom exceptions for the Scan class.
	class ex_acq : public std::exception 
    {
	public:
		ex_acq(const char* what_arg, const char* function, const char* file) :
			what_arg{ what_arg }, function{ function }, file{ file }
		{
		}

		const char* get_file() {
			return this->file;
		}

		const char* get_function() {
			return this->function;
		}

		const char* what() {
			return this->what_arg;
		}
	private:
		const char* what_arg;
		const char* function;
		const char* file;
	};
	
    // Custom exceptions for the Scan class.
	class ex_scan : public std::exception 
    {
	public:
		ex_scan(const char* what_arg, const char* function, const char* file) :
			what_arg{ what_arg }, function{ function }, file{ file }
		{
		}

		const char* get_file() {
			return this->file;
		}

		const char* get_function() {
			return this->function;
		}

		const char* what() {
			return this->what_arg;
		}
	private:
		const char* what_arg;
		const char* function;
		const char* file;
	};

    // Custom exceptions for the Trackstar class.
	class ex_trakStar : public std::exception 
    {
	public:
		ex_trakStar(const char* what_arg, const char* function, const char* file) :
			what_arg{ what_arg }, function{ function }, file{ file }
		{
		}

		ex_trakStar(const std::string& what_arg, const char* function, const char* file) :
			what_arg{ what_arg.c_str() }, function{ function }, file{ file }
		{
		}

		const char* get_file() {
			return this->file;
		}

		const char* get_function() {
			return this->function;
		}

		const char* what() {
			return this->what_arg.c_str();
		}
	private:
		const std::string what_arg;
		const char* function;
		const char* file;
	};
	class ex_export : public std::exception 
    {
	public:
		ex_export(const char* what_arg, const char* function, const char* file) :
			what_arg{ what_arg }, function{ function }, file{ file }
		{
		}

		ex_export(const std::string& what_arg, const char* function, const char* file) :
			what_arg{ what_arg.c_str() }, function{ function }, file{ file }
		{
		}

		const char* get_file() {
			return this->file;
		}

		const char* get_function() {
			return this->function;
		}

		const char* what() {
			return this->what_arg.c_str();
		}
	private:
		const std::string what_arg;
		const char* function;
		const char* file;
	};
}
