// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIB_HPP
#define NAZARA_DYNLIB_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/String.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#define NAZARA_DYNLIB_EXTENSION ".dll"
#elif defined(NAZARA_PLATFORM_LINUX)
	#define NAZARA_DYNLIB_EXTENSION ".so"
#elif defined(NAZARA_PLATFORM_MACOSX)
	#define NAZARA_DYNLIB_EXTENSION ".dynlib"
#else
	#error OS not handled
#endif

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_DYNLIB
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

using NzDynLibFunc =  int (*)(); // Type "générique" de pointeur sur fonction

class NzDynLibImpl;

class NAZARA_API NzDynLib : NzNonCopyable
{
	public:
		NzDynLib();
		NzDynLib(NzDynLib&& lib);
		~NzDynLib();

		NzString GetLastError() const;
		NzDynLibFunc GetSymbol(const NzString& symbol) const;

		bool IsLoaded() const;

		bool Load(const NzString& libraryPath);
		void Unload();

		NzDynLib& operator=(NzDynLib&& lib);

	private:
		NazaraMutexAttrib(m_mutex, mutable)

		mutable NzString m_lastError;
		NzDynLibImpl* m_impl;
};

#endif // NAZARA_DYNLIB_HPP
