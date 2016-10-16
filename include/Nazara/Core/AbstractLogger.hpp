// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTLOGGER_HPP
#define NAZARA_ABSTRACTLOGGER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/String.hpp>

namespace Nz
{
	class NAZARA_CORE_API AbstractLogger
	{
		public:
			AbstractLogger() = default;
			virtual ~AbstractLogger();

			virtual void EnableStdReplication(bool enable) = 0;

			virtual bool IsStdReplicationEnabled() const = 0;

			virtual void Write(const String& string) = 0;
			virtual void WriteError(ErrorType type, const String& error, unsigned int line = 0, const char* file = nullptr, const char* function = nullptr);
	};
}

#endif // NAZARA_ABSTRACTLOGGER_HPP
