// Custom exceptions for all the SmartScan classes for easier debugging. 
// Written by Eduard Cazacu in December 2020

#pragma once

#include <exception>
#include <string>

namespace SmartScan  {
    // Custom exceptions for the Smart Scan Service class.
	class ex_smartScan : public std::exception {
	public:
		// Constructor. Creates an ex_smartscan exception.
		ex_smartScan(const char* what_arg, const char* function, const char* file) : what_arg{ what_arg }, function{ function }, file{ file } {

		}

		// Returns a character array containing why the exception was thrown.
		const char* what() {
			return this->what_arg;
		}

		// Returns a character array containing in which function the exception was thrown.
		const char* get_function() {
			return this->function;
		}

		// Returns a character array containing in which file the exception was thrown.
		const char* get_file() {
			return this->file;
		}
	private:
		const char* what_arg;				// Character array containing why the exception was thrown.
		const char* function;				// Character array containing in which function the exception was thrown.
		const char* file;					// Character array containing in which file the exception was thrown.
	};

    // Custom exceptions for the Data acquisition class.
	class ex_acq : public std::exception  {
	public:
		// Constructor. Creates an ex_acq exception.
		ex_acq(const char* what_arg, const char* function, const char* file)
			: what_arg{ what_arg }, function{ function }, file{ file } {

		}

		// Returns a character array containing why the exception was thrown.
		const char* what() {
			return this->what_arg;
		}

		// Returns a character array containing in which function the exception was thrown.
		const char* get_function() {
			return this->function;
		}

		// Returns a character array containing in which file the exception was thrown.
		const char* get_file() {
			return this->file;
		}
	private:
		const char* what_arg;				// Character array containing why the exception was thrown.
		const char* function;				// Character array containing in which function the exception was thrown.
		const char* file;					// Character array containing in which file the exception was thrown.
	};
	
    // Custom exception for the Scan class.
	class ex_scan : public std::exception  {
	public:
		// Constructor. Creates an ex_scan exception.
		ex_scan(const char* what_arg, const char* function, const char* file)
			: what_arg{ what_arg }, function{ function }, file{ file } {

		}

		// Returns a character array containing why the exception was thrown.
		const char* what() {
			return this->what_arg;
		}

		// Returns a character array containing in which function the exception was thrown.
		const char* get_function() {
			return this->function;
		}

		// Returns a character array containing in which file the exception was thrown.
		const char* get_file() {
			return this->file;
		}
	private:
		const char* what_arg;				// Character array containing why the exception was thrown.
		const char* function;				// Character array containing in which function the exception was thrown.
		const char* file;					// Character array containing in which file the exception was thrown.
	};

    // Custom exceptions for the Trackstar class.
	class ex_trakStar : public std::exception  {
	public:
		// Constructor. Creates an ex_trakStar exception.
		ex_trakStar(const char* what_arg, const char* function, const char* file)
			: what_arg{ what_arg }, function{ function }, file{ file } {

		}

		// Returns a character array containing why the exception was thrown.
		const char* what()  {
			return this->what_arg;
		}

		// Returns a character array containing in which function the exception was thrown.
		const char* get_function()  {
			return this->function;
		}

		// Returns a character array containing in which file the exception was thrown.
		const char* get_file()  {
			return this->file;
		}
	private:
		const char* what_arg;				// Character array containing why the exception was thrown.
		const char* function;				// Character array containing in which function the exception was thrown.
		const char* file;					// Character array containing in which file the exception was thrown.
	};

	// Custom exception for the CSVexport class.
	class ex_export : public std::exception  {
	public:
		// Constructor. Creates an ex_export exception.
		ex_export(const char* what_arg, const char* function, const char* file)
			: what_arg{ what_arg }, function{ function }, file{ file } {

		}

		// Returns a character array containing why the exception was thrown.
		const char* what() {
			return this->what_arg;
		}

		// Returns a character array containing in which function the exception was thrown.
		const char* get_function() {
			return this->function;
		}

		// Returns a character array containing in which file the exception was thrown.
		const char* get_file() {
			return this->file;
		}

	private:
		const char* what_arg;				// Character array containing why the exception was thrown.
		const char* function;				// Character array containing in which function the exception was thrown.
		const char* file;					// Character array containing in which file the exception was thrown.
	};
}