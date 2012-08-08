// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIB_HPP
#define NAZARA_DYNLIB_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/String.hpp>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_DYNLIB
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

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

#endif // NAZARA_DYNLIB_HPP
