// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/FileLogger.hpp>
#include <Nazara/Core/StdLogger.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
		StdLogger s_stdLogger;
	}

	/*!
	* \ingroup core
	* \class Nz::Log
	* \brief Core class that represents a logger
	*/

	/*!
	* \brief Enables the log
	*
	* \param enable If true, enables the log
	*/

	void Log::Enable(bool enable)
	{
		s_enabled = enable;
	}

	/*!
	* \brief Gets the logger
	* \return An abstract pointer to the logger
	*/

	AbstractLogger* Log::GetLogger()
	{
		return s_logger;
	}

	/*!
	* \brief Checks whether or not the logging is enabled
	* \return true If logging is enabled
	*/

	bool Log::IsEnabled()
	{
		return s_enabled;
	}

	/*!
	* \brief Sets the logger
	*
	* \param logger AbstractLogger to log
	*/

	void Log::SetLogger(AbstractLogger* logger)
	{
		if (s_logger != &s_stdLogger)
			delete s_logger;

		s_logger = logger;
		if (!s_logger)
			s_logger = &s_stdLogger;
	}

	/*!
	* \brief Writes a string in the log
	*
	* \param string String to log
	*
	* \see WriteError
	*/

	void Log::Write(const String& string)
	{
		if (s_enabled)
			s_logger->Write(string);

		OnLogWrite(string);
	}

	/*!
	* \brief Writes the error in the log
	*
	* \param type Type of the error
	* \param error Message of the error
	* \param line Line of the error
	* \param file File of the error
	* \param function Function of the error
	*
	* \see Write
	*/

	void Log::WriteError(ErrorType type, const String& error, unsigned int line, const char* file, const char* function)
	{
		if (s_enabled)
			s_logger->WriteError(type, error, line, file, function);

		OnLogWriteError(type, error, line, file, function);
	}

	/*!
	* \brief Initializes the Log class
	* \return true if successful
	*/

	bool Log::Initialize()
	{
		if (s_logger == &s_stdLogger)
			SetLogger(new FileLogger());

		return true;
	}

	/*!
	* \brief Unitializes the Log class
	*/

	void Log::Uninitialize()
	{
		SetLogger(nullptr);
	}

	NazaraStaticSignalImpl(Log, OnLogWrite);
	NazaraStaticSignalImpl(Log, OnLogWriteError);

	AbstractLogger* Log::s_logger = &s_stdLogger;
	bool Log::s_enabled = true;
}
