// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_LOG_HPP
#define NAZARA_CORE_LOG_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/Signal.hpp>
#include <string>

#ifdef NAZARA_DEBUG
	#define NazaraDebug(...) NazaraNotice(__VA_ARGS__)
#else
	#define NazaraDebug(...)
#endif

#define NazaraNotice(...) Nz::Log::Write(__VA_ARGS__)

namespace Nz
{
	class AbstractLogger;

	class NAZARA_CORE_API Log
	{
		friend class Core;

		public:
			static void Enable(bool enable);

			static AbstractLogger* GetLogger();

			static bool IsEnabled();

			static void SetLogger(AbstractLogger* logger);

			static void Write(std::string_view str);
			template<typename... Args> static void Write(FormatString<Args...> fmt, Args&&... args);
			static void WriteError(ErrorType type, std::string_view error, unsigned int line = 0, const char* file = nullptr, const char* function = nullptr);

			NazaraStaticSignal(OnLogWrite, std::string_view /*string*/);
			NazaraStaticSignal(OnLogWriteError, ErrorType /*type*/, std::string_view /*error*/, unsigned int /*line*/, const char* /*file*/, const char* /*function*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			static AbstractLogger* s_logger;
			static bool s_enabled;
	};
}

#include <Nazara/Core/Log.inl>

#endif // NAZARA_CORE_LOG_HPP
