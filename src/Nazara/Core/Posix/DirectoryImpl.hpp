// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DIRECTORYIMPL_HPP
#define NAZARA_DIRECTORYIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <dirent.h>

namespace Nz
{
	class Directory;
	class String;

	class DirectoryImpl
	{
		public:
			DirectoryImpl(const Directory* parent);
			DirectoryImpl(const DirectoryImpl&) = delete;
			DirectoryImpl(DirectoryImpl&&) = delete; ///TODO
			~DirectoryImpl() = default;

			void Close();

			String GetResultName() const;
			UInt64 GetResultSize() const;

			bool IsResultDirectory() const;

			bool NextResult();

			bool Open(const String& dirPath);

			DirectoryImpl& operator=(const DirectoryImpl&) = delete;
			DirectoryImpl& operator=(DirectoryImpl&&) = delete; ///TODO

			static bool Create(const String& dirPath);
			static bool Exists(const String& dirPath);
			static String GetCurrent();
			static bool Remove(const String& dirPath);

		private:
			DIR* m_handle;
			dirent64* m_result;
	};
}

#endif // NAZARA_DIRECTORYIMPL_HPP
