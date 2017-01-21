// Copyright (C) 2017 Jérôme Leclercq
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

namespace Nz
{
	class DirectoryImpl;

	class NAZARA_CORE_API Directory
	{
		public:
			Directory();
			Directory(const String& dirPath);
			Directory(const Directory&) = delete;
			Directory(Directory&&) = delete; ///TODO
			~Directory();

			void Close();

			bool Exists() const;

			String GetPath() const;
			String GetPattern() const;
			String GetResultName() const;
			String GetResultPath() const;
			UInt64 GetResultSize() const;

			bool IsOpen() const;
			bool IsResultDirectory() const;

			bool NextResult(bool skipDots = true);

			bool Open();

			void SetPath(const String& dirPath);
			void SetPattern(const String& pattern);

			static bool Copy(const String& sourcePath, const String& destPath);
			static bool Create(const String& dirPath, bool recursive = false);
			static bool Exists(const String& dirPath);
			static String GetCurrent();
			static const char* GetCurrentFileRelativeToEngine(const char* currentFile);
			static bool Remove(const String& dirPath, bool emptyDirectory = false);
			static bool SetCurrent(const String& dirPath);

			Directory& operator=(const Directory&) = delete;
			Directory& operator=(Directory&&) = delete; ///TODO

		private:
			NazaraMutexAttrib(m_mutex, mutable)

			String m_dirPath;
			String m_pattern;
			DirectoryImpl* m_impl;
	};
}

#endif // NAZARA_DIRECTORY_HPP
