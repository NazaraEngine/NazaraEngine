// Copyright (C) 2015 Jérôme Leclercq
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
			"Assert failed: ",	// ErrorType_AssertFailed
			"Internal error: ",	// ErrorType_Internal
			"Error: ",			// ErrorType_Normal
			"Warning: "			// ErrorType_Warning
		};

		static_assert(sizeof(errorType) / sizeof(const char*) == ErrorType_Max + 1, "Error type array is incomplete");
	}

	AbstractLogger::~AbstractLogger() = default;

	void AbstractLogger::WriteError(ErrorType type, const String& error, unsigned int line, const char* file, const char* function)
	{
		StringStream stream;
		stream << errorType[type] << error;

		if (line != 0 && file && function)
			stream << " (" << file << ':' << line << ": " << function << ')';
	}
}
