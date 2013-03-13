// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/DynLibImpl.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Core/Debug.hpp>

NzDynLibImpl::NzDynLibImpl(NzDynLib* parent) :
m_parent(parent)
{
}

NzDynLibFunc NzDynLibImpl::GetSymbol(const NzString& symbol) const
{
	NzDynLibFunc sym = reinterpret_cast<NzDynLibFunc>(GetProcAddress(m_handle, symbol.GetConstBuffer()));
	if (!sym)
		m_parent->SetLastError(NzGetLastSystemError());

	return sym;
}

bool NzDynLibImpl::Load(const NzString& libraryPath)
{
	NzString path = libraryPath;
	if (!path.EndsWith(".dll"))
		path += ".dll";

	wchar_t* pathW = path.GetWideBuffer();
	m_handle = LoadLibraryExW(pathW, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
	delete[] pathW;

	if (m_handle)
		return true;
	else
	{
		m_parent->SetLastError(NzGetLastSystemError());
		return false;
	}
}

void NzDynLibImpl::Unload()
{
	FreeLibrary(m_handle);
}
