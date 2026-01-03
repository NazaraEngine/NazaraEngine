// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_ABSTRACTLOGGER_HPP
#define NAZARA_CORE_ABSTRACTLOGGER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
#include <string>

namespace Nz
{
	class NAZARA_CORE_API AbstractLogger
	{
		public:
			AbstractLogger() = default;
			AbstractLogger(const AbstractLogger&) = default;
			AbstractLogger(AbstractLogger&&) noexcept = default;
			virtual ~AbstractLogger();

			virtual void EnableStdReplication(bool enable) = 0;

			virtual bool IsStdReplicationEnabled() const = 0;

			virtual void Write(std::string_view string) = 0;
			virtual void WriteError(ErrorType type, std::string_view error, unsigned int line = 0, const char* file = nullptr, const char* function = nullptr);

			AbstractLogger& operator=(const AbstractLogger&) = default;
			AbstractLogger& operator=(AbstractLogger&&) noexcept = default;
	};
}

#endif // NAZARA_CORE_ABSTRACTLOGGER_HPP
