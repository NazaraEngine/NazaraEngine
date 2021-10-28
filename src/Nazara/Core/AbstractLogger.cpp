// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <sstream>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
		const char* errorType[] = {
			"Assert failed: ",  // ErrorType::AssertFailed
			"Internal error: ", // ErrorType::Internal
			"Error: ",          // ErrorType::Normal
			"Warning: "         // ErrorType::Warning
		};

		static_assert(sizeof(errorType) / sizeof(const char*) == ErrorTypeCount, "Error type array is incomplete");
	}

	/*!
	* \ingroup core
	* \class Nz::AbstractLogger
	* \brief Core class that represents the behaviour of the log classes
	*
	* \remark This class is abstract
	*/
	AbstractLogger::~AbstractLogger() = default;

	/*!
	* \brief Writes the error in StringStream
	*
	* \param type Enumeration of type ErrorType
	* \param error String describing the error
	* \param line Line number in the file
	* \param file Filename
	* \param function Name of the function throwing the error
	*/
	void AbstractLogger::WriteError(ErrorType type, const std::string_view& error, unsigned int line, const char* file, const char* function)
	{
		std::ostringstream ss;
		ss << errorType[UnderlyingCast(type)] << error;

		if (line != 0 && file && function)
			ss << " (" << file << ':' << line << ": " << function << ')';

		Write(ss.str());
	}
}
