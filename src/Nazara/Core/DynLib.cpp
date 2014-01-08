// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
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

#include <Nazara/Core/Debug.hpp>

NzDynLib::NzDynLib() :
m_impl(nullptr)
{
}

NzDynLib::NzDynLib(NzDynLib&& lib) :
m_lastError(std::move(lib.m_lastError)),
m_impl(lib.m_impl)
{
	lib.m_impl = nullptr;
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

	return m_impl->GetSymbol(symbol, &m_lastError);
}

bool NzDynLib::IsLoaded() const
{
	return m_impl != nullptr;
}

bool NzDynLib::Load(const NzString& libraryPath, bool appendExtension)
{
	NazaraLock(m_mutex)

	Unload();

	m_impl = new NzDynLibImpl(this);
	if (!m_impl->Load(libraryPath, appendExtension, &m_lastError))
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

NzDynLib& NzDynLib::operator=(NzDynLib&& lib)
{
	Unload();

	m_impl = lib.m_impl;
	m_lastError = std::move(lib.m_lastError);

	lib.m_impl = nullptr;

	return *this;
}
