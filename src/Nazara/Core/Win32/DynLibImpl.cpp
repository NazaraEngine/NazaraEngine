// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/DynLibImpl.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/String.hpp>
#include <memory>
#include <Nazara/Core/Debug.hpp>

NzDynLibImpl::NzDynLibImpl(NzDynLib* parent)
{
	NazaraUnused(parent);
}

NzDynLibFunc NzDynLibImpl::GetSymbol(const NzString& symbol, NzString* errorMessage) const
{
	NzDynLibFunc sym = reinterpret_cast<NzDynLibFunc>(GetProcAddress(m_handle, symbol.GetConstBuffer()));
	if (!sym)
		*errorMessage = NzError::GetLastSystemError();

	return sym;
}

bool NzDynLibImpl::Load(const NzString& libraryPath, NzString* errorMessage)
{
	NzString path = libraryPath;
	if (!path.EndsWith(".dll"))
		path += ".dll";

	std::unique_ptr<wchar_t[]> wPath(path.GetWideBuffer());
	m_handle = LoadLibraryExW(wPath.get(), nullptr, (NzFile::IsAbsolute(path)) ? LOAD_WITH_ALTERED_SEARCH_PATH : 0);

	if (m_handle)
		return true;
	else
	{
		*errorMessage = NzError::GetLastSystemError();
		return false;
	}
}

void NzDynLibImpl::Unload()
{
	FreeLibrary(m_handle);
}
