// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/DynLibImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <memory>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	DynLibImpl::~DynLibImpl()
	{
		if (m_handle)
			FreeLibrary(m_handle);
	}

	DynLibFunc DynLibImpl::GetSymbol(const char* symbol, std::string* errorMessage) const
	{
		DynLibFunc sym = reinterpret_cast<DynLibFunc>(::GetProcAddress(m_handle, symbol));
		if (!sym)
			*errorMessage = Error::GetLastSystemError();

		return sym;
	}

	bool DynLibImpl::Load(const std::filesystem::path& libraryPath, std::string* errorMessage)
	{
		if constexpr (std::is_same_v<std::filesystem::path::value_type, wchar_t>)
			m_handle = LoadLibraryExW(libraryPath.c_str(), nullptr, (libraryPath.is_absolute()) ? LOAD_WITH_ALTERED_SEARCH_PATH : 0);
		else
			m_handle = LoadLibraryExW(ToWideString(PathToString(libraryPath)).data(), nullptr, (libraryPath.is_absolute()) ? LOAD_WITH_ALTERED_SEARCH_PATH : 0);

		if (m_handle)
			return true;
		else
		{
			*errorMessage = Error::GetLastSystemError();
			return false;
		}
	}
}

#include <Nazara/Core/AntiWindows.hpp>
