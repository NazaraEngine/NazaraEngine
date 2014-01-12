// Copyright (C) 2014 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DIRECTORYIMPL_HPP
#define NAZARA_DIRECTORYIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

class NzDirectory;
class NzString;

class NzDirectoryImpl : NzNonCopyable
{
	public:
		NzDirectoryImpl(const NzDirectory* parent);
		~NzDirectoryImpl() = default;

		void Close();

		NzString GetResultName() const;
		nzUInt64 GetResultSize() const;

		bool IsResultDirectory() const;

		bool NextResult();

		bool Open(const NzString& dirPath);

		static bool Create(const NzString& dirPath);
		static bool Exists(const NzString& dirPath);
		static NzString GetCurrent();
		static bool Remove(const NzString& dirPath);

	private:
		DIR* m_handle;
		dirent64* m_result;
};

#endif // NAZARA_DIRECTORYIMPL_HPP
