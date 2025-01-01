// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_ERROR_HPP
#define NAZARA_CORE_ERROR_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Format.hpp>
#include <NazaraUtils/Assert.hpp>
#include <string>

#define NazaraCheck(a, fmt, ...)      if NAZARA_UNLIKELY(!(a)) Nz::Error::Trigger(Nz::ErrorType::CheckFailed, __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, Nz::Format(NAZARA_FORMAT(fmt) __VA_OPT__(,) __VA_ARGS__))
#define NazaraError(fmt, ...)         Nz::Error::Trigger(Nz::ErrorType::Normal,   __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, Nz::Format(NAZARA_FORMAT(fmt) __VA_OPT__(,) __VA_ARGS__))
#define NazaraInternalError(fmt, ...) Nz::Error::Trigger(Nz::ErrorType::Internal, __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, Nz::Format(NAZARA_FORMAT(fmt) __VA_OPT__(,) __VA_ARGS__))
#define NazaraWarning(fmt, ...)       Nz::Error::Trigger(Nz::ErrorType::Warning,  __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, Nz::Format(NAZARA_FORMAT(fmt) __VA_OPT__(,) __VA_ARGS__))

namespace Nz
{
	class NAZARA_CORE_API Error
	{
		public:
			Error() = delete;
			~Error() = delete;

			static ErrorModeFlags ApplyFlags(ErrorModeFlags orFlags, ErrorModeFlags andFlags);

			static constexpr std::string_view TranslateFilepath(std::string_view file);
			static ErrorModeFlags GetFlags();
			static std::string GetLastError(std::string_view* file = nullptr, unsigned int* line = nullptr, std::string_view* function = nullptr);
			static unsigned int GetLastSystemErrorCode();
			static std::string GetLastSystemError(unsigned int code = GetLastSystemErrorCode());

			static ErrorModeFlags SetFlags(ErrorModeFlags flags);

			static inline void Trigger(ErrorType type, std::string error);
			static inline void Trigger(ErrorType type, unsigned int line, std::string_view file, std::string_view function, std::string error);

		private:
			static void TriggerInternal(ErrorType type, std::string error, unsigned int line, std::string_view file, std::string_view function);
	};
}

#include <Nazara/Core/Error.inl>

#endif // NAZARA_CORE_ERROR_HPP
