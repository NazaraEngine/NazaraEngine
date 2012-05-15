// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <ctime>

#if NAZARA_CORE_REDIRECT_TO_CERR_ON_LOG_FAILURE
#include <cstdio>
#endif

#define NAZARA_CLASS_LOG
#include <Nazara/Core/ThreadSafety.hpp>
#include <Nazara/Core/Debug.hpp>

namespace
{
	NzString errorType[] = {
		"Assert failed: ",	// nzErrorType_AssertFailed
		"Internal error: ",	// nzErrorType_Internal
		"Error: ",			// nzErrorType_Normal
		"Warning: "			// nzErrorType_Warning
	};
}

NzLog::NzLog() :
m_filePath("NazaraLog.log"),
m_file(nullptr),
m_append(false),
m_enabled(true),
m_writeTime(true)
{
}

NzLog::~NzLog()
{
	delete m_file;
}

void NzLog::Enable(bool enable)
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

void NzLog::EnableAppend(bool enable)
{
	NazaraLock(m_mutex)

	m_append = enable;
	if (!m_append && m_file)
	{
		m_file->Delete();
		m_file = nullptr;
	}
}

void NzLog::EnableDateTime(bool enable)
{
	NazaraLock(m_mutex)

	m_writeTime = enable;
}

NzString NzLog::GetFile() const
{
	NazaraLock(m_mutex)

	if (m_file)
		return m_file->GetFilePath();
	else
		return NzString();
}

bool NzLog::IsEnabled() const
{
	NazaraLock(m_mutex)

	return m_enabled;
}

void NzLog::SetFile(const NzString& filePath)
{
	NazaraLock(m_mutex)

	m_filePath = filePath;
	if (m_file)
		m_file->SetFile(filePath);
}

void NzLog::Write(const NzString& string)
{
	NazaraLock(m_mutex)

	if (m_enabled)
	{
		if (!m_file)
			m_file = new NzFile(m_filePath, NzFile::Text | NzFile::WriteOnly | ((m_append) ? NzFile::Append : NzFile::Truncate));

		NzString line;

		if (m_writeTime)
		{
			line.Reserve(23 + string.GetSize() + 1);
			line.Resize(23);

			time_t currentTime = std::time(nullptr);
			std::strftime(&line[0], 24, "%d/%m/%Y - %H:%M:%S: ", std::localtime(&currentTime));
		}
		else
			line.Reserve(string.GetSize() + 1);

		line += string;
		line += '\n';

		#if NAZARA_CORE_REDIRECT_TO_CERR_ON_LOG_FAILURE
		if (!m_file->IsOpen() || !m_file->Write(line))
			std::fputs(line.GetBuffer(), stderr);
		#else
		if (m_file->IsOpen())
			m_file->Write(line);
		#endif
	}
}

void NzLog::WriteError(nzErrorType type, const NzString& error, unsigned int line, const NzString& file, const NzString& func)
{
	NzString stream;
	stream.Reserve(errorType[type].GetSize() + error.GetSize() + 2 + file.GetSize() + 1 + NzGetNumberLength(line) +2 + func.GetSize() + 1);
	stream += errorType[type];
	stream += error;
	stream += " (";
	stream += file;
	stream += ':';
	stream += NzString::Number(line);
	stream += ": ";
	stream += func;
	stream += ')';
	/*NzStringStream stream;
	stream << errorType[type] << error << " (" << file << ':' << line << ": " << func << ')';*/
	Write(stream);
}

NzLog* NzLog::Instance()
{
	static NzLog log;
	return &log;
}
