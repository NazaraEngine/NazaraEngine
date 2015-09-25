// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <ctime>
#include <cstring>

#if NAZARA_CORE_DUPLICATE_LOG_TO_COUT
	#include <cstdio>
#endif

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_LOG
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
		const char* errorType[] = {
			"Assert failed: ",	// ErrorType_AssertFailed
			"Internal error: ",	// ErrorType_Internal
			"Error: ",			// ErrorType_Normal
			"Warning: "			// ErrorType_Warning
		};

		static_assert(sizeof(errorType)/sizeof(const char*) == ErrorType_Max+1, "Error type array is incomplete");
	}

	Log::Log() :
	m_filePath("NazaraLog.log"),
	m_file(nullptr),
	m_append(false),
	m_enabled(true),
	m_writeTime(true)
	{
	}

	Log::~Log()
	{
		delete m_file;
	}

	void Log::Enable(bool enable)
	{
		NazaraLock(m_mutex)

		if (m_enabled == enable)
			return;

		m_enabled = enable;
		if (!m_enabled && m_file)
		{
			delete m_file;
			m_file = nullptr;
		}
	}

	void Log::EnableAppend(bool enable)
	{
		NazaraLock(m_mutex)

		m_append = enable;
		if (!m_append && m_file)
		{
			m_file->Delete();
			m_file = nullptr;
		}
	}

	void Log::EnableDateTime(bool enable)
	{
		NazaraLock(m_mutex)

		m_writeTime = enable;
	}

	String Log::GetFile() const
	{
		NazaraLock(m_mutex)

		if (m_file)
			return m_file->GetPath();
		else
			return String();
	}

	bool Log::IsEnabled() const
	{
		NazaraLock(m_mutex)

		return m_enabled;
	}

	void Log::SetFile(const String& filePath)
	{
		NazaraLock(m_mutex)

		m_filePath = filePath;
		if (m_file)
			m_file->SetFile(filePath);
	}

	void Log::Write(const String& string)
	{
		NazaraLock(m_mutex)

		if (m_enabled)
		{
			if (!m_file)
				m_file = new File(m_filePath, OpenMode_Text | OpenMode_WriteOnly | ((m_append) ? OpenMode_Append : OpenMode_Truncate));

			String line;

			if (m_writeTime)
			{
				line.Reserve(23 + string.GetSize() + 1);
				line.Set(23, '\0'); // Buffer non-initialisé

				time_t currentTime = std::time(nullptr);
				std::strftime(&line[0], 24, "%d/%m/%Y - %H:%M:%S: ", std::localtime(&currentTime));
			}
			else
				line.Reserve(string.GetSize() + 1);

			line += string;
			line += '\n';

			if (m_file->IsOpen())
				m_file->Write(line);

			#if NAZARA_CORE_DUPLICATE_LOG_TO_COUT
			std::fputs(line.GetConstBuffer(), stdout);
			#endif
		}
	}

	void Log::WriteError(ErrorType type, const String& error)
	{
		StringStream stream;
		stream << errorType[type] << error;
		Write(stream);
	}

	void Log::WriteError(ErrorType type, const String& error, unsigned int line, const String& file, const String& func)
	{
		StringStream stream;
		stream << errorType[type] << error << " (" << file << ':' << line << ": " << func << ')';
		Write(stream);
	}

	Log* Log::Instance()
	{
		static Log log;
		return &log;
}
}
