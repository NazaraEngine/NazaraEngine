// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <memory>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/DynLibImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/DynLibImpl.hpp>
#else
	#error No implementation for this platform
#endif

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_DYNLIB
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	DynLib::DynLib() :
	m_impl(nullptr)
	{
	}

	DynLib::DynLib(DynLib&& lib) :
	m_lastError(std::move(lib.m_lastError)),
	m_impl(lib.m_impl)
	{
		lib.m_impl = nullptr;
	}

	DynLib::~DynLib()
	{
		Unload();
	}

	String DynLib::GetLastError() const
	{
		NazaraLock(m_mutex)

		return m_lastError;
	}

	DynLibFunc DynLib::GetSymbol(const String& symbol) const
	{
		NazaraLock(m_mutex)

		#if NAZARA_CORE_SAFE
		if (!m_impl)
		{
			NazaraError("Library not opened");
			return nullptr;
		}
		#endif

		return m_impl->GetSymbol(symbol, &m_lastError);
	}

	bool DynLib::IsLoaded() const
	{
		return m_impl != nullptr;
	}

	bool DynLib::Load(const String& libraryPath)
	{
		NazaraLock(m_mutex)

		Unload();

		std::unique_ptr<DynLibImpl> impl(new DynLibImpl(this));
		if (!impl->Load(libraryPath, &m_lastError))
		{
			NazaraError("Failed to load library: " + m_lastError);
			return false;
		}

		m_impl = impl.release();

		return true;
	}

	void DynLib::Unload()
	{
		NazaraLock(m_mutex)

		if (m_impl)
		{
			m_impl->Unload();
			delete m_impl;
			m_impl = nullptr;
		}
	}

	DynLib& DynLib::operator=(DynLib&& lib)
	{
		Unload();

		m_impl = lib.m_impl;
		m_lastError = std::move(lib.m_lastError);

		lib.m_impl = nullptr;

		return *this;
	}
}
