// Copyright (C) 2024 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Posix/DynLibImpl.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <dlfcn.h>
#include <cstring>

namespace Nz::PlatformImpl
{
	DynLibImpl::~DynLibImpl()
	{
		if (m_handle)
			dlclose(m_handle);
	}

	DynLibFunc DynLibImpl::GetSymbol(const char* symbol, std::string* errorMessage) const
	{
		dlerror(); // Clear error flag

		void* ptr = dlsym(m_handle, symbol);
		if (!ptr)
			*errorMessage = dlerror();

		return BitCast<DynLibFunc>(ptr);
	}

	bool DynLibImpl::Load(const std::filesystem::path& libraryPath, std::string* errorMessage)
	{
		dlerror(); // Clear error flag
		m_handle = dlopen(PathToString(libraryPath).data(), RTLD_LAZY | RTLD_GLOBAL);

		if (!m_handle)
		{
			*errorMessage = dlerror();
			return false;
		}

		return true;
	}
}
