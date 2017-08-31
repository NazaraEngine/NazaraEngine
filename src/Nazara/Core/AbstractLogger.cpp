// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
		const char* errorType[] = {
			"Assert failed: ",  // ErrorType_AssertFailed
			"Internal error: ", // ErrorType_Internal
			"Error: ",          // ErrorType_Normal
			"Warning: "         // ErrorType_Warning
		};

		static_assert(sizeof(errorType) / sizeof(const char*) == ErrorType_Max + 1, "Error type array is incomplete");
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

	void AbstractLogger::WriteError(ErrorType type, const String& error, unsigned int line, const char* file, const char* function)
	{
		StringStream stream;
		stream << errorType[type] << error;

		if (line != 0 && file && function)
			stream << " (" << file << ':' << line << ": " << function << ')';

		Write(stream);
	}
}
