// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
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
	/*!
	* \ingroup core
	* \class Nz::Error
	* \brief Core class that represents an error
	*/

	/*!
	* \brief Gets the flags of the error
	* \return Flag
	*/

	ErrorModeFlags Error::GetFlags()
	{
		return s_flags;
	}

	/*!
	* \brief Gets the last error
	* \return Last error
	*
	* \param file Optional argument to set last error file
	* \param line Optional argument to set last error line
	* \param function Optional argument to set last error function
	*/

	std::string Error::GetLastError(const char** file, unsigned int* line, const char** function)
	{
		if (file)
			*file = s_lastErrorFile;

		if (line)
			*line = s_lastErrorLine;

		if (function)
			*function = s_lastErrorFunction;

		return s_lastError;
	}

	/*!
	* \brief Gets the last system error code
	* \return "errno"
	*/

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

	/*!
	* \brief Gets the string representation of the last system error code
	* \return Message of the error
	*
	* \param code Code of the error
	*/

	std::string Error::GetLastSystemError(unsigned int code)
	{
		#if defined(NAZARA_PLATFORM_WINDOWS)
		wchar_t* buffer = nullptr;

		DWORD length = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		               nullptr,
		               code,
		               0,
		               reinterpret_cast<LPWSTR>(&buffer),
		               0,
		               nullptr);

		if (length == 0)
			return "<internal error: FormatMessageW failed with " + std::to_string(::GetLastError()) + ">";

		CallOnExit freeOnExit([buffer] { LocalFree(buffer); });
		return FromWideString(buffer);
		#elif defined(NAZARA_PLATFORM_POSIX)
		return std::strerror(code);
		#else
			#error GetLastSystemError is not implemented on this platform

		return "GetLastSystemError is not implemented on this platform";
		#endif
	}

	/*!
	* \brief Sets the flags
	*
	* \param flags Flags for the error
	*/

	void Error::SetFlags(ErrorModeFlags flags)
	{
		s_flags = flags;
	}

	/*!
	* \brief Checks if the error should trigger
	*
	* \param type ErrorType of the error
	* \param error Message of the error
	*
	* \remark Produces a std::abort on AssertFailed with NAZARA_CORE_EXIT_ON_ASSERT_FAILURE defined
	* \remark Produces a std::runtime_error on AssertFailed or throwing exception
	*/

	void Error::Trigger(ErrorType type, std::string error)
	{
		if (type == ErrorType::AssertFailed || (s_flags & ErrorMode::Silent) == 0 || (s_flags & ErrorMode::SilentDisabled) != 0)
			Log::WriteError(type, error);

		s_lastError = std::move(error);
		s_lastErrorFile = "";
		s_lastErrorFunction = "";
		s_lastErrorLine = 0;

		#if NAZARA_CORE_EXIT_ON_ASSERT_FAILURE
		if (type == ErrorType::AssertFailed)
			std::abort();
		#endif

		if (type == ErrorType::AssertFailed || (type != ErrorType::Warning &&
			(s_flags & ErrorMode::ThrowException) != 0 && (s_flags & ErrorMode::ThrowExceptionDisabled) == 0))
			throw std::runtime_error(s_lastError);
	}

	/*!
	* \brief Checks if the error should trigger
	*
	* \param type ErrorType of the error
	* \param error Message of the error
	* \param line Line of the error
	* \param file File of the error
	* \param function Function of the error
	*
	* \remark Produces a std::abort on AssertFailed with NAZARA_CORE_EXIT_ON_ASSERT_FAILURE defined
	* \remark Produces a std::runtime_error on AssertFailed or throwing exception
	*/

	void Error::Trigger(ErrorType type, std::string error, unsigned int line, const char* file, const char* function)
	{
		file = GetCurrentFileRelativeToEngine(file);

		if (type == ErrorType::AssertFailed || (s_flags & ErrorMode::Silent) == 0 || (s_flags & ErrorMode::SilentDisabled) != 0)
			Log::WriteError(type, error, line, file, function);

		s_lastError = std::move(error);
		s_lastErrorFile = file;
		s_lastErrorFunction = function;
		s_lastErrorLine = line;

		#if NAZARA_CORE_EXIT_ON_ASSERT_FAILURE
		if (type == ErrorType::AssertFailed)
			std::abort();
		#endif

		if (type == ErrorType::AssertFailed || (type != ErrorType::Warning &&
			(s_flags & ErrorMode::ThrowException) != 0 && (s_flags & ErrorMode::ThrowExceptionDisabled) == 0))
			throw std::runtime_error(s_lastError);
	}

	const char* Error::GetCurrentFileRelativeToEngine(const char* file)
	{
		if (const char* ptr = std::strstr(file, "NazaraEngine/"))
			return ptr;

		if (const char* ptr = std::strstr(file, "NazaraEngine\\"))
			return ptr;

		return file;
	}

	ErrorModeFlags Error::s_flags = ErrorMode::None;
	std::string Error::s_lastError;
	const char* Error::s_lastErrorFunction = "";
	const char* Error::s_lastErrorFile = "";
	unsigned int Error::s_lastErrorLine = 0;
}
