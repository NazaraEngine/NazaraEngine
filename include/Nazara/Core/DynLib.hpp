// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIB_HPP
#define NAZARA_DYNLIB_HPP

#define NAZARA_DYNLIB

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Core/ThreadSafety.hpp>
#include <Nazara/Utility/NonCopyable.hpp>

class NzDynLibImpl;
typedef int (*NzDynLibFunc)(); // Type "générique" de pointeur sur fonction

class NzDynLib : NzNonCopyable
{
	friend class NzDynLibImpl;

	public:
		NzDynLib(const NzString& libraryPath);
		~NzDynLib();

		NzString GetLastError() const;
		NzDynLibFunc GetSymbol(const NzString& symbol) const;
		bool Load();
		void Unload();

	private:
		void SetLastError(const NzString& error);

		NazaraMutexAttrib(m_mutex, mutable)

		NzString m_lastError;
		NzString m_path;
		NzDynLibImpl* m_impl;
};

#undef NAZARA_DYNLIB

#endif // NAZARA_DYNLIB_HPP
