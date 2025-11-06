// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_ANDROID_ANDROIDLOGGER_HPP
#define NAZARA_CORE_ANDROID_ANDROIDLOGGER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractLogger.hpp>

namespace Nz
{
	class NAZARA_CORE_API AndroidLogger : public AbstractLogger
	{
		public:
			AndroidLogger() = default;
			AndroidLogger(const AndroidLogger&) = default;
			AndroidLogger(AndroidLogger&&) noexcept = default;
			~AndroidLogger();

			void EnableStdReplication(bool enable) override;

			bool IsStdReplicationEnabled() const override;

			void Write(std::string_view string) override;
			void WriteError(ErrorType type, std::string_view error, unsigned int line = 0, const char* file = nullptr, const char* function = nullptr) override;

			AndroidLogger& operator=(const AndroidLogger&) = default;
			AndroidLogger& operator=(AndroidLogger&&) noexcept = default;
	};
}

#endif // NAZARA_CORE_ANDROID_ANDROIDLOGGER_HPP
