// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/DynLibImpl.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/String.hpp>
#include <memory>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	DynLibImpl::DynLibImpl(DynLib* parent)
	{
		NazaraUnused(parent);
	}

	DynLibFunc DynLibImpl::GetSymbol(const String& symbol, String* errorMessage) const
	{
		DynLibFunc sym = reinterpret_cast<DynLibFunc>(GetProcAddress(m_handle, symbol.GetConstBuffer()));
		if (!sym)
			*errorMessage = Error::GetLastSystemError();

		return sym;
	}

	bool DynLibImpl::Load(const String& libraryPath, String* errorMessage)
	{
		String path = libraryPath;
		if (!path.EndsWith(".dll"))
			path += ".dll";

		m_handle = LoadLibraryExW(path.GetWideString().data(), nullptr, (File::IsAbsolute(path)) ? LOAD_WITH_ALTERED_SEARCH_PATH : 0);
		if (m_handle)
			return true;
		else
		{
			*errorMessage = Error::GetLastSystemError();
			return false;
		}
	}

	void DynLibImpl::Unload()
	{
		FreeLibrary(m_handle);
	}

}
