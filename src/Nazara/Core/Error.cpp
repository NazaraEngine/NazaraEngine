// Copyright (C) 2014 Jérôme Leclercq
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
#endif

#include <Nazara/Core/Debug.hpp>

void NzError::Error(nzErrorType type, const NzString& error)
{
	if ((s_flags & nzErrorFlag_Silent) == 0 || (s_flags & nzErrorFlag_SilentDisabled) != 0)
		NazaraLog->WriteError(type, error);

	s_lastError = error;
	s_lastErrorFile = "";
	s_lastErrorFunction = "";
	s_lastErrorLine = 0;

	#if NAZARA_CORE_EXIT_ON_ASSERT_FAILURE
	if (type == nzErrorType_AssertFailed)
		std::exit(EXIT_FAILURE);
	#endif

	if (type != nzErrorType_Warning && (s_flags & nzErrorFlag_ThrowException) != 0 && (s_flags & nzErrorFlag_ThrowExceptionDisabled) == 0)
		throw std::runtime_error(error);
}

void NzError::Error(nzErrorType type, const NzString& error, unsigned int line, const char* file, const char* function)
{
	if ((s_flags & nzErrorFlag_Silent) == 0 || (s_flags & nzErrorFlag_SilentDisabled) != 0)
		NazaraLog->WriteError(type, error, line, file, function);

	s_lastError = error;
	s_lastErrorFile = file;
	s_lastErrorFunction = function;
	s_lastErrorLine = line;

	if (type != nzErrorType_Warning && (s_flags & nzErrorFlag_ThrowException) != 0 && (s_flags & nzErrorFlag_ThrowExceptionDisabled) == 0)
		throw std::runtime_error(error);

	#if NAZARA_CORE_EXIT_ON_ASSERT_FAILURE
	if (type == nzErrorType_AssertFailed)
		std::exit(EXIT_FAILURE);
	#endif
}

nzUInt32 NzError::GetFlags()
{
	return s_flags;
}

NzString NzError::GetLastError(const char** file, unsigned int* line, const char** function)
{
	if (file)
		*file = s_lastErrorFile;

	if (line)
		*line = s_lastErrorLine;

	if (function)
		*function = s_lastErrorFunction;

	return s_lastError;
}

unsigned int NzError::GetLastSystemErrorCode()
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

NzString NzError::GetLastSystemError(unsigned int code)
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

	NzString error(NzString::Unicode(buffer));
	LocalFree(buffer);

	error.Trim(); // Pour une raison inconnue, Windows met deux-trois retours à la ligne après le message

	return error;
	#elif defined(NAZARA_PLATFORM_POSIX)
	return std::strerror(code);
	#else
		#error GetLastSystemError is not implemented on this platform

	return NzString("GetLastSystemError is not implemented on this platform");
	#endif
}

void NzError::SetFlags(nzUInt32 flags)
{
	s_flags = flags;
}

nzUInt32 NzError::s_flags = nzErrorFlag_None;
NzString NzError::s_lastError;
const char* NzError::s_lastErrorFunction = "";
const char* NzError::s_lastErrorFile = "";
unsigned int NzError::s_lastErrorLine = 0;
