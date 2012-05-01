// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOG_HPP
#define NAZARA_LOG_HPP

#define NAZARA_LOG

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Core/ThreadSafety.hpp>
#include <Nazara/Utility/NonCopyable.hpp>

#define NazaraLog NzLog::Instance()

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

#undef NAZARA_LOGGER

#endif // NAZARA_LOGGER_HPP
