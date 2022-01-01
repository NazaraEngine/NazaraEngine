// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_STDLOGGER_HPP
#define NAZARA_CORE_STDLOGGER_HPP

#include <Nazara/Prerequisites.hpp>
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

			void Write(const std::string_view& error) override;
			void WriteError(ErrorType type, const std::string_view& error, unsigned int line = 0, const char* file = nullptr, const char* function = nullptr) override;

			StdLogger& operator=(const StdLogger&) = default;
			StdLogger& operator=(StdLogger&&) noexcept = default;
	};
}

#endif // NAZARA_CORE_STDLOGGER_HPP
