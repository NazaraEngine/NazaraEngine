// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/DynLibImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/DynLibImpl.hpp>
#else
	#error No implementation for this platform
#endif

#define NAZARA_CLASS_DYNLIB
#include <Nazara/Core/ThreadSafety.hpp>
#include <Nazara/Core/Debug.hpp>

NzDynLib::NzDynLib(const NzString& libraryPath) :
m_path(libraryPath),
m_impl(nullptr)
{
}

NzDynLib::~NzDynLib()
{
	Unload();
}

NzString NzDynLib::GetLastError() const
{
	NazaraLock(m_mutex)

	return m_lastError;
}

NzDynLibFunc NzDynLib::GetSymbol(const NzString& symbol) const
{
	NazaraLock(m_mutex)

	#if NAZARA_CORE_SAFE
	if (!m_impl)
	{
		NazaraError("Library not opened");
		return nullptr;
	}
	#endif

	return m_impl->GetSymbol(symbol);
}

bool NzDynLib::Load()
{
	NazaraLock(m_mutex)

	Unload();

	m_impl = new NzDynLibImpl(this);
	if (!m_impl->Load(m_path))
	{
		delete m_impl;
		m_impl = nullptr;

		return false;
	}

	return true;
}

void NzDynLib::Unload()
{
	NazaraLock(m_mutex)

	if (m_impl)
	{
		m_impl->Unload();
		delete m_impl;
		m_impl = nullptr;
	}
}

void NzDynLib::SetLastError(const NzString& error)
{
	NazaraLock(m_mutex)

	m_lastError = error;
}
