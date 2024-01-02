// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_STDLOGGER_HPP
#define NAZARA_CORE_STDLOGGER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractLogger.hpp>

namespace Nz
{
	class NAZARA_CORE_API StdLogger : public AbstractLogger
	{
		public:
			StdLogger() = default;
			StdLogger(const StdLogger&) = default;
			StdLogger(StdLogger&&) noexcept = default;
			~StdLogger();

			void EnableStdReplication(bool enable) override;

			bool IsStdReplicationEnabled() const override;

			void Write(std::string_view string) override;
			void WriteError(ErrorType type, std::string_view error, unsigned int line = 0, const char* file = nullptr, const char* function = nullptr) override;

			StdLogger& operator=(const StdLogger&) = default;
			StdLogger& operator=(StdLogger&&) noexcept = default;
	};
}

#endif // NAZARA_CORE_STDLOGGER_HPP
