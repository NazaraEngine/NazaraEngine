// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <cstdlib>
#include <stdexcept>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <windows.h>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <cstring>
	#include <errno.h>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	UInt32 Error::GetFlags()
	{
		return s_flags;
	}

	String Error::GetLastError(const char** file, unsigned int* line, const char** function)
	{
		if (file)
			*file = s_lastErrorFile;

		if (line)
			*line = s_lastErrorLine;

		if (function)
			*function = s_lastErrorFunction;

		return s_lastError;
	}

	unsigned int Error::GetLastSystemErrorCode()
	{
		#if defined(NAZARA_PLATFORM_WINDOWS)
		return ::GetLastError();
		#elif defined(NAZARA_PLATFORM_POSIX)
		return errno;
		#else
			#error GetLastSystemErrorCode is not implemented on this platform
		return 0;
		#endif
	}

	String Error::GetLastSystemError(unsigned int code)
	{
		#if defined(NAZARA_PLATFORM_WINDOWS)
		wchar_t* buffer = nullptr;

		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
					  nullptr,
					  code,
					  0,
					  reinterpret_cast<LPWSTR>(&buffer),
					  0,
					  nullptr);

		String error(String::Unicode(buffer));
		LocalFree(buffer);

		error.Trim(); // Pour une raison inconnue, Windows met deux-trois retours à la ligne après le message

		return error;
		#elif defined(NAZARA_PLATFORM_POSIX)
		return std::strerror(code);
		#else
			#error GetLastSystemError is not implemented on this platform

		return String("GetLastSystemError is not implemented on this platform");
		#endif
	}

	void Error::SetFlags(UInt32 flags)
	{
		s_flags = flags;
	}

	void Error::Trigger(ErrorType type, const String& error)
	{
		if (type == ErrorType_AssertFailed || (s_flags & ErrorFlag_Silent) == 0 || (s_flags & ErrorFlag_SilentDisabled) != 0)
			NazaraLog->WriteError(type, error);

		s_lastError = error;
		s_lastErrorFile = "";
		s_lastErrorFunction = "";
		s_lastErrorLine = 0;

		#if NAZARA_CORE_EXIT_ON_ASSERT_FAILURE
		if (type == ErrorType_AssertFailed)
			std::abort();
		#endif

		if (type == ErrorType_AssertFailed || (type != ErrorType_Warning &&
			(s_flags & ErrorFlag_ThrowException) != 0 && (s_flags & ErrorFlag_ThrowExceptionDisabled) == 0))
			throw std::runtime_error(error);
	}

	void Error::Trigger(ErrorType type, const String& error, unsigned int line, const char* file, const char* function)
	{
		if (type == ErrorType_AssertFailed || (s_flags & ErrorFlag_Silent) == 0 || (s_flags & ErrorFlag_SilentDisabled) != 0)
			NazaraLog->WriteError(type, error, line, file, function);

		s_lastError = error;
		s_lastErrorFile = file;
		s_lastErrorFunction = function;
		s_lastErrorLine = line;

		#if NAZARA_CORE_EXIT_ON_ASSERT_FAILURE
		if (type == ErrorType_AssertFailed)
			std::abort();
		#endif

		if (type == ErrorType_AssertFailed || (type != ErrorType_Warning &&
			(s_flags & ErrorFlag_ThrowException) != 0 && (s_flags & ErrorFlag_ThrowExceptionDisabled) == 0))
			throw std::runtime_error(error);
	}

	UInt32 Error::s_flags = ErrorFlag_None;
	String Error::s_lastError;
	const char* Error::s_lastErrorFunction = "";
	const char* Error::s_lastErrorFile = "";
	unsigned int Error::s_lastErrorLine = 0;
}
