// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Win32/DynLibImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/Win32/Win32Utils.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <memory>

namespace Nz::PlatformImpl
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
		std::wstring path = libraryPath.wstring();

		std::size_t lastDirSep = std::wstring::npos;
		if (std::size_t sepOffset = path.find_first_of(L"\\/"); sepOffset != std::wstring::npos)
		{
			do 
			{
				path[sepOffset] = L'\\';
				lastDirSep = sepOffset;
			}
			while ((sepOffset = path.find_first_of(L"\\/", sepOffset + 1)) != std::wstring::npos);

			path[lastDirSep] = L'\0';
			SetDllDirectoryW(path.data());
			path[lastDirSep] = L'\\';
		}

		m_handle = LoadLibraryExW(path.data(), nullptr, (libraryPath.is_absolute()) ? LOAD_WITH_ALTERED_SEARCH_PATH : 0);

		if (lastDirSep != std::wstring::npos)
			SetDllDirectoryW(nullptr);

		if (!m_handle)
		{
			*errorMessage = Error::GetLastSystemError();
			return false;
		}

		return true;
	}
}

#include <Nazara/Core/AntiWindows.hpp>
