// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOG_HPP
#define NAZARA_LOG_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/String.hpp>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_LOG
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#ifdef NAZARA_DEBUG
#define NazaraDebug(txt) NazaraNotice(txt)
#else
#define NazaraDebug(txt)
#endif

#define NazaraLog NzLog::Instance()
#define NazaraNotice(txt) NazaraLog->Write(txt)

class NzFile;

class NAZARA_API NzLog : NzNonCopyable
{
	public:
		void Enable(bool enable);
		void EnableAppend(bool enable);
		void EnableDateTime(bool enable);

		NzString GetFile() const;

		bool IsEnabled() const;

		void SetFile(const NzString& filePath);

		void Write(const NzString& string);
		void WriteError(nzErrorType type, const NzString& error, unsigned int line, const NzString& file, const NzString& func);

		static NzLog* Instance();

	private:
		NzLog();
		~NzLog();

		NazaraMutexAttrib(m_mutex, mutable)

		NzString m_filePath;
		NzFile* m_file;
		bool m_append;
		bool m_enabled;
		bool m_writeTime;
};

#endif // NAZARA_LOGGER_HPP
