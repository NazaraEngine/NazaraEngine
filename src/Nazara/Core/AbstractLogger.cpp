// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/AbstractLogger.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <sstream>

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
	void AbstractLogger::WriteError(ErrorType type, std::string_view error, unsigned int line, const char* file, const char* function)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		std::ostringstream ss;
		ss << s_errorTypes[type] << error;

		if (line != 0 && file && function)
			ss << " (" << file << ':' << line << ": " << function << ')';

		Write(ss.str());
	}
}
