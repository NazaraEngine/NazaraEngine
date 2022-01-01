// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/DynLibImpl.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <memory>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	DynLibImpl::DynLibImpl(DynLib*) :
	m_handle(nullptr)
	{
	}

	DynLibImpl::~DynLibImpl()
	{
		if (m_handle)
			FreeLibrary(m_handle);
	}

	DynLibFunc DynLibImpl::GetSymbol(const char* symbol, std::string* errorMessage) const
	{
		DynLibFunc sym = reinterpret_cast<DynLibFunc>(GetProcAddress(m_handle, symbol));
		if (!sym)
			*errorMessage = Error::GetLastSystemError();

		return sym;
	}

	bool DynLibImpl::Load(const std::filesystem::path& libraryPath, std::string* errorMessage)
	{
		std::filesystem::path path = libraryPath;
		if (path.extension() != ".dll")
			path += ".dll";

		m_handle = LoadLibraryExW(ToWideString(path.generic_u8string()).data(), nullptr, (path.is_absolute()) ? LOAD_WITH_ALTERED_SEARCH_PATH : 0);
		if (m_handle)
			return true;
		else
		{
			*errorMessage = Error::GetLastSystemError();
			return false;
		}
	}
}
