// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <cstdlib>
#include <stdexcept>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Windows.h>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <cstring>
	#include <errno.h>
#endif

#include <Nazara/Core/Debug.hpp>

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

		thread_local ErrorModeFlags s_flags;
		thread_local std::string s_lastError = "no error";
		thread_local std::string_view s_lastErrorFunction;
		thread_local std::string_view s_lastErrorFile;
		thread_local unsigned int s_lastErrorLine = 0;
	}

	/*!
	* \ingroup core
	* \class Nz::Error
	* \brief Core class that represents an error
	*/
	ErrorModeFlags Error::ApplyFlags(ErrorModeFlags orFlags, ErrorModeFlags andFlags)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		ErrorModeFlags& flags = s_flags;

		ErrorModeFlags previousFlags = flags;

		flags |= orFlags;
		flags &= andFlags;

		return previousFlags;
	}

	/*!
	* \brief Gets the flags of the error
	* \return Flag
	*/
	ErrorModeFlags Error::GetFlags()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

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
	std::string Error::GetLastError(std::string_view* file, unsigned int* line, std::string_view* function)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

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

	ErrorModeFlags Error::SetFlags(ErrorModeFlags flags)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		ErrorModeFlags previousFlags = s_flags;
		s_flags = flags;

		return previousFlags;
	}

	void Error::TriggerInternal(ErrorType type, std::string error, unsigned int line, std::string_view file, std::string_view function)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		ErrorModeFlags& flags = s_flags;

		if (type == ErrorType::AssertFailed || (flags & ErrorMode::Silent) == 0)
		{
			if (line == 0 && file.empty())
				Log::Write("{}{}", s_errorTypes[type], error);
			else
				Log::Write("{}{} ({}:{}: {})", s_errorTypes[type], error, file, line, function);
		}

		if (type != ErrorType::Warning)
		{
			s_lastError = std::move(error);
			s_lastErrorFile = file;
			s_lastErrorFunction = function;
			s_lastErrorLine = line;
		}

		#if NAZARA_CORE_EXIT_ON_ASSERT_FAILURE
		if (type == ErrorType::AssertFailed)
			std::abort();
		#endif

		if (type == ErrorType::AssertFailed || (type != ErrorType::Warning && flags.Test(ErrorMode::ThrowException)))
			throw std::runtime_error(s_lastError);
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
