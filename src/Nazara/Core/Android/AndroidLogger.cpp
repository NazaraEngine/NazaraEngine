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
		constexpr EnumArray<ErrorType, android_LogPriority> s_logPriorities = {
			ANDROID_LOG_FATAL, // ErrorType::AssertFailed
			ANDROID_LOG_ERROR, // ErrorType::Internal
			ANDROID_LOG_ERROR, // ErrorType::Normal
			ANDROID_LOG_WARN   // ErrorType::Warning
		};

		constexpr EnumArray<ErrorType, std::string_view> s_errorTypes = {
			"Assert failed: ",  // ErrorType::AssertFailed
			"Internal error: ", // ErrorType::Internal
			"Error: ",          // ErrorType::Normal
			"Warning: "         // ErrorType::Warning
		};
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
			__android_log_print(s_logPriorities[type], "NazaraApp", "%s: %s", s_errorTypes[type], nullTerminatedError.data());
		else
			__android_log_print(s_logPriorities[type], "NazaraApp", "%s: %s (in %s at %s:%d)", s_errorTypes[type], nullTerminatedError.data(), function, file, line);
	}
}
