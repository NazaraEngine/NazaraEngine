// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StdLogger.hpp>
#include <cstdio>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
		const char* errorType[] = {
			"Assert failed",	// ErrorType_AssertFailed
			"Internal error",	// ErrorType_Internal
			"Error",			// ErrorType_Normal
			"Warning"			// ErrorType_Warning
		};

		static_assert(sizeof(errorType) / sizeof(const char*) == ErrorType_Max + 1, "Error type array is incomplete");
	}

	/*!
	* \class Nz::StdLogger
	* \brief Logger writing to standard output (stdout, stderr)
	*/

	StdLogger::~StdLogger() = default;

	/*!
	* \brief Enable replication to standard output
	*
	* Does nothing, as the std logger always write to standard output
	*/

	void StdLogger::EnableStdReplication(bool enable)
	{
		NazaraUnused(enable);
		// We always replicate to std, that's our purpose
	}

	/*!
	* \brief Get the standard output replication status
	*
	* Always returns true
	*/

	bool StdLogger::IsStdReplicationEnabled()
	{
		return true;
	}

	/*!
	* Write to the console
	* \param string The log to write to the console
	*
	* \see WriteError
	*/

	void StdLogger::Write(const String& string)
	{
		fputs(string.GetConstBuffer(), stdout);
		fputc('\n', stdout);
	}

	/*!
	* Write an error to the console
	* \param type The error type
	* \param error The error text
	* \param line The line the error occurred
	* \param file The file the error occurred
	* \param function The function the error occurred
	*
	* \see Write
	*/

	void StdLogger::WriteError(ErrorType type, const String& error, unsigned int line, const char* file, const char* function)
	{
		fprintf(stderr, "%s: %s", errorType[type], error.GetConstBuffer());

		if (line != 0 && file && function)
			fprintf(stderr, " (in %s at %s:%d)", function, file, line);

		fputc('\n', stderr);
	}
}
