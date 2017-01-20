// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DYNLIB_HPP
#define NAZARA_DYNLIB_HPP

#include <Nazara/Prerequesites.hpp>
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

namespace Nz
{
	using DynLibFunc = int (*)(); // "Generic" type of poiter to function

	class DynLibImpl;

	class NAZARA_CORE_API DynLib
	{
		public:
			DynLib();
			DynLib(const DynLib&) = delete;
			DynLib(DynLib&& lib);
			~DynLib();

			String GetLastError() const;
			DynLibFunc GetSymbol(const String& symbol) const;

			bool IsLoaded() const;

			bool Load(const String& libraryPath);
			void Unload();

			DynLib& operator=(const DynLib&) = delete;
			DynLib& operator=(DynLib&& lib);

		private:
			NazaraMutexAttrib(m_mutex, mutable)

			mutable String m_lastError;
			DynLibImpl* m_impl;
	};
}

#endif // NAZARA_DYNLIB_HPP
