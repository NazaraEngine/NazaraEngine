// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILELOGGER_HPP
#define NAZARA_FILELOGGER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StdLogger.hpp>

namespace Nz
{
	class NAZARA_CORE_API FileLogger : public AbstractLogger
	{
		public:
			FileLogger(const String& logPath = "NazaraLog.log");
			FileLogger(const FileLogger&) = default;
			FileLogger(FileLogger&&) = default;
			~FileLogger();

			void EnableTimeLogging(bool enable);
			void EnableStdReplication(bool enable) override;

			bool IsStdReplicationEnabled() const override;
			bool IsTimeLoggingEnabled() const;

			void Write(const String& string) override;
			void WriteError(ErrorType type, const String& error, unsigned int line = 0, const char* file = nullptr, const char* function = nullptr) override;

			FileLogger& operator=(const FileLogger&) = default;
			FileLogger& operator=(FileLogger&&) = default;

		private:
			File m_outputFile;
			StdLogger m_stdLogger;
			bool m_forceStdOutput;
			bool m_stdReplicationEnabled;
			bool m_timeLoggingEnabled;
	};
}

#endif // NAZARA_FILELOGGER_HPP
