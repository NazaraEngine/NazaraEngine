// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/FileLogger.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <array>
#include <ctime>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	FileLogger::FileLogger(const String& logPath) :
	m_outputFile(logPath),
	m_forceStdOutput(false),
	m_stdReplicationEnabled(true),
	m_timeLoggingEnabled(true)
	{
	}

	FileLogger::~FileLogger() = default;

	void FileLogger::EnableTimeLogging(bool enable)
	{
		m_timeLoggingEnabled = enable;
	}

	void FileLogger::EnableStdReplication(bool enable)
	{
		m_stdReplicationEnabled = enable;
	}

	bool FileLogger::IsStdReplicationEnabled()
	{
		return m_stdReplicationEnabled;
	}

	bool FileLogger::IsTimeLoggingEnabled()
	{
		return m_timeLoggingEnabled;
	}

	void FileLogger::Write(const String& string)
	{
		if (m_forceStdOutput || m_stdReplicationEnabled)
		{
			m_stdLogger.Write(string);

			if (m_forceStdOutput)
				return;
		}

		// To prevent infinite loops
		m_forceStdOutput = true;
		CallOnExit resetOnExit([this] () 
		{ 
			m_forceStdOutput = false;
		});

		if (!m_outputFile.IsOpen())
		{
			if (!m_outputFile.Open(OpenMode_Text | OpenMode_Truncate | OpenMode_WriteOnly))
			{
				NazaraError("Failed to open output file");
				return;
			}
		}

		// Apply some processing before writing
		StringStream stream;
		if (m_timeLoggingEnabled)
		{
			std::array<char, 24> buffer;

			time_t currentTime = std::time(nullptr);
			std::strftime(buffer.data(), 24, "%d/%m/%Y - %H:%M:%S: ", std::localtime(&currentTime));

			stream << buffer.data();
		}

		stream << string << '\n';

		m_outputFile.Write(stream);
	}

	void FileLogger::WriteError(ErrorType type, const String& error, unsigned int line, const char* file, const char* function)
	{
		if (m_forceStdOutput || m_stdReplicationEnabled)
		{
			m_stdLogger.WriteError(type, error, line, file, function);

			if (m_forceStdOutput)
				return;
		}

		AbstractLogger::WriteError(type, error, line, file, function);
		m_outputFile.Flush();
	}
}
