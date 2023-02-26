// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Android/AndroidLogger.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Utils/StackArray.hpp>
#include <android/log.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		android_LogPriority s_logPriority[] = {
			ANDROID_LOG_FATAL,
			ANDROID_LOG_ERROR,
			ANDROID_LOG_ERROR,
			ANDROID_LOG_WARN
		};

		static_assert(sizeof(s_logPriority) / sizeof(android_LogPriority) == ErrorTypeCount, "Log priority array is incomplete");

		const char* s_errorType[] = {
			"Assert failed",	// ErrorType::AssertFailed
			"Internal error",	// ErrorType::Internal
			"Error",		    // ErrorType::Normal
			"Warning"		    // ErrorType::Warning
		};

		static_assert(sizeof(s_errorType) / sizeof(const char*) == ErrorTypeCount, "Error type array is incomplete");
	}

	AndroidLogger::~AndroidLogger() = default;

	void AndroidLogger::EnableStdReplication(bool /*enable*/)
	{
	}

	bool AndroidLogger::IsStdReplicationEnabled() const
	{
		return false;
	}

	void AndroidLogger::Write(std::string_view string)
	{
		// Ensure \0 at the end of string
		StackArray<char> nullTerminatedStr = NazaraStackArrayNoInit(char, string.size() + 1);
		std::memcpy(nullTerminatedStr.data(), string.data(), string.size());
		nullTerminatedStr[string.size()] = '\0';

		__android_log_write(ANDROID_LOG_INFO, "NazaraApp", nullTerminatedStr.data());
	}

	void AndroidLogger::WriteError(ErrorType type, std::string_view error, unsigned int line, const char* file, const char* function)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		// Ensure \0 at the end of string
		StackArray<char> nullTerminatedError = NazaraStackArrayNoInit(char, error.size() + 1);
		std::memcpy(nullTerminatedError.data(), error.data(), error.size());
		nullTerminatedError[error.size()] = '\0';

		if (line != 0 && file && function)
			__android_log_print(s_logPriority[UnderlyingCast(type)], "NazaraApp", "%s: %s", s_errorType[UnderlyingCast(type)], nullTerminatedError.data());
		else
			__android_log_print(s_logPriority[UnderlyingCast(type)], "NazaraApp", "%s: %s (in %s at %s:%d)", s_errorType[UnderlyingCast(type)], nullTerminatedError.data(), function, file, line);
	}
}
