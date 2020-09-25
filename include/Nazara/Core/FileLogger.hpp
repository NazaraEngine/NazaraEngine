// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILELOGGER_HPP
#define NAZARA_FILELOGGER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/StdLogger.hpp>
#include <filesystem>
#include <fstream>

namespace Nz
{
	class NAZARA_CORE_API FileLogger : public AbstractLogger
	{
		public:
			FileLogger(std::filesystem::path logPath = "NazaraLog.log");
			FileLogger(const FileLogger&) = default;
			FileLogger(FileLogger&&) = default;
			~FileLogger();

			void EnableTimeLogging(bool enable);
			void EnableStdReplication(bool enable) override;

			bool IsStdReplicationEnabled() const override;
			bool IsTimeLoggingEnabled() const;

			void Write(const std::string_view& string) override;
			void WriteError(ErrorType type, const std::string_view& error, unsigned int line = 0, const char* file = nullptr, const char* function = nullptr) override;

			FileLogger& operator=(const FileLogger&) = default;
			FileLogger& operator=(FileLogger&&) = default;

		private:
			std::fstream m_outputFile;
			std::filesystem::path m_outputPath;
			StdLogger m_stdLogger;
			bool m_forceStdOutput;
			bool m_stdReplicationEnabled;
			bool m_timeLoggingEnabled;
	};
}

#endif // NAZARA_FILELOGGER_HPP
