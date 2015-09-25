// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOG_HPP
#define NAZARA_LOG_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Error.hpp>
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

#define NazaraLog Nz::Log::Instance()
#define NazaraNotice(txt) NazaraLog->Write(txt)

namespace Nz
{
	class File;

	class NAZARA_CORE_API Log
	{
		public:
			void Enable(bool enable);
			void EnableAppend(bool enable);
			void EnableDateTime(bool enable);

			String GetFile() const;

			bool IsEnabled() const;

			void SetFile(const String& filePath);

			void Write(const String& string);
			void WriteError(ErrorType type, const String& error);
			void WriteError(ErrorType type, const String& error, unsigned int line, const String& file, const String& func);

			static Log* Instance();

		private:
			Log();
			Log(const Log&) = delete;
			Log(Log&&) = delete;
			~Log();

			Log& operator=(const Log&) = delete;
			Log& operator=(Log&&) = delete;

			NazaraMutexAttrib(m_mutex, mutable)

			String m_filePath;
			File* m_file;
			bool m_append;
			bool m_enabled;
			bool m_writeTime;
	};
}

#endif // NAZARA_LOGGER_HPP
