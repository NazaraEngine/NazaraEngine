// Copyright (C) 2015 Jérôme Leclercq
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

	void Log::Enable(bool enable)
	{
		s_enabled = enable;
	}

	AbstractLogger* Log::GetLogger()
	{
		return s_logger;
	}

	bool Log::IsEnabled()
	{
		return s_enabled;
	}

	void Log::SetLogger(AbstractLogger* logger)
	{
		if (s_logger != &s_stdLogger)
			delete s_logger;

		s_logger = logger;
		if (!s_logger)
			s_logger = &s_stdLogger;
	}

	void Log::Write(const String& string)
	{
		if (s_enabled)
			s_logger->Write(string);
		
		OnLogWrite(string);
	}

	void Log::WriteError(ErrorType type, const String& error, unsigned int line, const char* file, const char* function)
	{
		if (s_enabled)
			s_logger->WriteError(type, error, line, file, function);
		
		OnLogWriteError(type, error, line, file, function);
	}

	bool Log::Initialize()
	{
		SetLogger(new FileLogger());
		return true;
	}

	void Log::Uninitialize()
	{
		SetLogger(nullptr);
	}

	NazaraStaticSignalImpl(Log, OnLogWrite);
	NazaraStaticSignalImpl(Log, OnLogWriteError);

	AbstractLogger* Log::s_logger = &s_stdLogger;
	bool Log::s_enabled = true;
}
