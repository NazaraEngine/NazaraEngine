// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DIRECTORYIMPL_HPP
#define NAZARA_DIRECTORYIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <windows.h>

class NzDirectory;
class NzString;

class NzDirectoryImpl
{
	public:
		NzDirectoryImpl(const NzDirectory* parent);
		NzDirectoryImpl(const NzDirectoryImpl&) = delete;
		NzDirectoryImpl(NzDirectoryImpl&&) = delete; ///TODO
		~NzDirectoryImpl() = default;

		void Close();

		NzString GetResultName() const;
		nzUInt64 GetResultSize() const;

		bool IsResultDirectory() const;

		bool NextResult();

		bool Open(const NzString& dirPath);

		NzDirectoryImpl& operator=(const NzDirectoryImpl&) = delete;
		NzDirectoryImpl& operator=(NzDirectoryImpl&&) = delete; ///TODO

		static bool Create(const NzString& dirPath);
		static bool Exists(const NzString& dirPath);
		static NzString GetCurrent();
		static bool Remove(const NzString& dirPath);

	private:
		HANDLE m_handle;
		WIN32_FIND_DATAW m_result;
		bool m_firstCall;
};

#endif // NAZARA_DIRECTORYIMPL_HPP
