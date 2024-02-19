// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/StdLogger.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <cstdio>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		constexpr EnumArray<ErrorType, std::string_view> s_errorTypes = {
			"Assert failed: ",  // ErrorType::AssertFailed
			"Internal error: ", // ErrorType::Internal
			"Error: ",          // ErrorType::Normal
			"Warning: "         // ErrorType::Warning
		};
	}

	/*!
	* \ingroup core
	* \class Nz::StdLogger
	* \brief Core class that represents a logger writing to standard output (stdout, stderr)
	*/

	StdLogger::~StdLogger() = default;

	/*!
	* \brief Enables replication to standard output
	*
	* Does nothing, as the std logger always write to standard output
	*
	* \param enable Unused argument
	*/

	void StdLogger::EnableStdReplication(bool enable)
	{
		NazaraUnused(enable);
		// We always replicate to std, that's our purpose
	}

	/*!
	* \brief Gets the standard output replication status
	* \return Always returns true
	*/

	bool StdLogger::IsStdReplicationEnabled() const
	{
		return true;
	}

	/*!
	* \brief Writes to the console
	*
	* \param string The log to write to the console
	*
	* \see WriteError
	*/

	void StdLogger::Write(std::string_view string)
	{
		fwrite(string.data(), sizeof(char), string.size(), stdout);
		fputc('\n', stdout);
	}

	/*!
	* \brief Writes an error to the console
	*
	* \param type The error type
	* \param error The error text
	* \param line The line the error occurred
	* \param file The file the error occurred
	* \param function The function the error occurred
	*
	* \see Write
	*/

	void StdLogger::WriteError(ErrorType type, std::string_view error, unsigned int line, const char* file, const char* function)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		fprintf(stderr, "%s: ", s_errorTypes[type].data());
		fwrite(error.data(), sizeof(char), error.size(), stdout);

		if (line != 0 && file && function)
			fprintf(stderr, " (in %s at %s:%d)", function, file, line);

		fputc('\n', stderr);
	}
}
