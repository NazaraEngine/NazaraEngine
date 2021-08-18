// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/FileLogger.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <array>
#include <ctime>
#include <filesystem>
#include <sstream>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::FileLogger
	* \brief Core class that represents a file logger
	*/

	/*!
	* \brief Constructs a FileLogger object with a file name
	*
	* \param logPath Path to log
	*/

	FileLogger::FileLogger(std::filesystem::path logPath) :
	m_outputPath(std::move(logPath)),
	m_forceStdOutput(false),
	m_stdReplicationEnabled(true),
	m_timeLoggingEnabled(true)
	{
	}

	/*!
	* \brief Destructs the object
	*/

	FileLogger::~FileLogger() = default;

	/*!
	* \brief Enables the log of the time
	*
	* \param enable If true, enables the time log
	*/

	void FileLogger::EnableTimeLogging(bool enable)
	{
		m_timeLoggingEnabled = enable;
	}

	/*!
	* \brief Enables the replication to the stdout
	*
	* \param enable If true, enables the replication
	*/

	void FileLogger::EnableStdReplication(bool enable)
	{
		m_stdReplicationEnabled = enable;
	}

	/*!
	* \brief Checks whether or not the replication to the stdout is enabled
	* \return true If replication is enabled
	*/

	bool FileLogger::IsStdReplicationEnabled() const
	{
		return m_stdReplicationEnabled;
	}

	/*!
	* \brief Checks whether or not the logging of the time is enabled
	* \return true If logging of the time is enabled
	*/

	bool FileLogger::IsTimeLoggingEnabled() const
	{
		return m_timeLoggingEnabled;
	}

	/*!
	* \brief Writes a string in the log
	*
	* \param string String to log
	*
	* \remark Produces a NazaraError if file could not be opened
	*
	* \see WriteError
	*/

	void FileLogger::Write(const std::string_view& string)
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

		if (!m_outputFile.is_open())
		{
			m_outputFile.open(m_outputPath, std::ios_base::trunc | std::ios_base::out);
			if (!m_outputFile.is_open())
			{
				NazaraError("Failed to open output file");
				return;
			}
		}

		// Apply some processing before writing
		std::ostringstream stream;
		if (m_timeLoggingEnabled)
		{
			std::array<char, 24> buffer;

			time_t currentTime = std::time(nullptr);
			std::strftime(buffer.data(), 24, "%d/%m/%Y - %H:%M:%S: ", std::localtime(&currentTime));

			stream << buffer.data();
		}

		stream << string << '\n';

		m_outputFile << stream.str();
	}

	/*!
	* \brief Writes an error in the log
	*
	* \param type The error type
	* \param error The error text
	* \param line The line the error occurred
	* \param file The file the error occurred
	* \param function The function the error occurred
	*
	* \see Write
	*/

	void FileLogger::WriteError(ErrorType type, const std::string_view& error, unsigned int line, const char* file, const char* function)
	{
		AbstractLogger::WriteError(type, error, line, file, function);
		m_outputFile.flush();
	}
}
