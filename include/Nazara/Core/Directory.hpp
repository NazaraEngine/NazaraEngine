// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DIRECTORY_HPP
#define NAZARA_DIRECTORY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#define NAZARA_DIRECTORY_SEPARATOR '\\'
#elif defined(NAZARA_PLATFORM_LINUX)
	#define NAZARA_DIRECTORY_SEPARATOR '/'
#else
	#error OS not handled
	#define NAZARA_DIRECTORY_SEPARATOR '/'
#endif

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_DIRECTORY
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

class NzDirectoryImpl;

class NAZARA_API NzDirectory
{
	public:
		NzDirectory();
		NzDirectory(const NzString& dirPath);
		~NzDirectory();

		void Close();

		bool Exists() const;

		NzString GetPattern() const;
		NzString GetResultName() const;
		NzString GetResultPath() const;
		nzUInt64 GetResultSize() const;

		bool IsOpen() const;
		bool IsResultDirectory() const;

		bool NextResult(bool skipDots = true);

		bool Open();

		void SetDirectory(const NzString& dirPath);
		void SetPattern(const NzString& pattern);

		static bool Copy(const NzString& sourcePath, const NzString& destPath);
		static bool Create(const NzString& dirPath, bool recursive = false);
		static bool Exists(const NzString& dirPath);
		static NzString GetCurrent();
		static const char* GetCurrentFileRelativeToEngine(const char* currentFile);
		static bool Remove(const NzString& dirPath, bool emptyDirectory = false);
		static bool SetCurrent(const NzString& dirPath);

	private:
		NazaraMutexAttrib(m_mutex, mutable)

		NzString m_dirPath;
		NzString m_pattern;
		NzDirectoryImpl* m_impl = nullptr;
};

#endif // NAZARA_DIRECTORY_HPP
