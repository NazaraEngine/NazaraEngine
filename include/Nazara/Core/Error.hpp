// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ERROR_HPP
#define NAZARA_CORE_ERROR_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Enums.hpp>
#include <string>

#if NAZARA_CORE_ENABLE_ASSERTS || defined(NAZARA_DEBUG)
	#define NazaraAssert(a, err) if (!(a)) Nz::Error::Trigger(Nz::ErrorType::AssertFailed, err, __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION)
#else
	#define NazaraAssert(a, err) for (;;) break
#endif

#define NazaraError(err) Nz::Error::Trigger(Nz::ErrorType::Normal, err, __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION)
#define NazaraInternalError(err) Nz::Error::Trigger(Nz::ErrorType::Internal, err, __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION)
#define NazaraWarning(err) Nz::Error::Trigger(Nz::ErrorType::Warning, err, __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION)

namespace Nz
{
	class NAZARA_CORE_API Error
	{
		public:
			Error() = delete;
			~Error() = delete;

			static ErrorModeFlags GetFlags();
			static std::string GetLastError(const char** file = nullptr, unsigned int* line = nullptr, const char** function = nullptr);
			static unsigned int GetLastSystemErrorCode();
			static std::string GetLastSystemError(unsigned int code = GetLastSystemErrorCode());

			static void SetFlags(ErrorModeFlags flags);

			static void Trigger(ErrorType type, std::string error);
			static void Trigger(ErrorType type, std::string error, unsigned int line, const char* file, const char* function);

		private:
			static const char* GetCurrentFileRelativeToEngine(const char* file);

			static ErrorModeFlags s_flags;
			static std::string s_lastError;
			static const char* s_lastErrorFunction;
			static const char* s_lastErrorFile;
			static unsigned int s_lastErrorLine;
	};
}

#endif // NAZARA_CORE_ERROR_HPP
