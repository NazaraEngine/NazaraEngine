// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <cstdlib>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <windows.h>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <cstring>
#endif

#include <Nazara/Core/Debug.hpp>

void NzError(nzErrorType type, const NzString& error, unsigned int line, const char* file, const char* function)
{
	NazaraLog->WriteError(type, error, line, file, function);

	#if NAZARA_CORE_EXIT_ON_ASSERT_FAILURE
	if (type == nzErrorType_AssertFailed)
		std::exit(EXIT_FAILURE);
	#endif
}

unsigned int NzGetLastSystemErrorCode()
{
	#if defined(NAZARA_PLATFORM_WINDOWS)
	return GetLastError();
	#elif defined(NAZARA_PLATFORM_POSIX)
	return errno;
	#else
		#error GetLastSystemErrorCode is not implemented on this platform
	#endif
	return 0;
}

NzString NzGetLastSystemError(unsigned int code)
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

	return "GetLastSystemError is not implemented on this platform";
	#endif
}
