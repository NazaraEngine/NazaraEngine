// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ERROR_HPP
#define NAZARA_ERROR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/String.hpp>

#if NAZARA_CORE_ENABLE_ASSERTS || defined(NAZARA_DEBUG)
	#define NazaraAssert(a, err) if (!(a)) Nz::Error::Trigger(Nz::ErrorType_AssertFailed, err, __LINE__, __FILE__, NAZARA_FUNCTION)
#else
	#define NazaraAssert(a, err) for (;;) break
#endif

#define NazaraError(err) Nz::Error::Trigger(Nz::ErrorType_Normal, err, __LINE__, __FILE__, NAZARA_FUNCTION)
#define NazaraInternalError(err) Nz::Error::Trigger(Nz::ErrorType_Internal, err, __LINE__, __FILE__, NAZARA_FUNCTION)
#define NazaraWarning(err) Nz::Error::Trigger(Nz::ErrorType_Warning, err, __LINE__, __FILE__, NAZARA_FUNCTION)

namespace Nz
{
	class NAZARA_CORE_API Error
	{
		public:
			Error() = delete;
			~Error() = delete;

			static UInt32 GetFlags();
			static String GetLastError(const char** file = nullptr, unsigned int* line = nullptr, const char** function = nullptr);
			static unsigned int GetLastSystemErrorCode();
			static String GetLastSystemError(unsigned int code = GetLastSystemErrorCode());

			static void SetFlags(UInt32 flags);

			static void Trigger(ErrorType type, const String& error);
			static void Trigger(ErrorType type, const String& error, unsigned int line, const char* file, const char* function);

		private:
			static UInt32 s_flags;
			static String s_lastError;
			static const char* s_lastErrorFunction;
			static const char* s_lastErrorFile;
			static unsigned int s_lastErrorLine;
	};
}

#endif // NAZARA_ERROR_HPP
