// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/DirectoryModuleResolver.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline DirectoryModuleResolver::DirectoryModuleResolver() :
	m_searchDirectory(std::make_shared<VirtualDirectory>())
	{
	}

	inline void DirectoryModuleResolver::RegisterModuleDirectory(std::string_view path, std::filesystem::path realPath)
	{
		m_searchDirectory->StoreDirectory(path, realPath);
	}

	inline void DirectoryModuleResolver::RegisterModuleFile(std::string_view path, std::filesystem::path realPath)
	{
		m_searchDirectory->StoreFile(path, realPath);
	}

	inline void DirectoryModuleResolver::RegisterModuleFile(std::string_view path, std::vector<UInt8> fileContent)
	{
		m_searchDirectory->StoreFile(path, std::move(fileContent));
	}

	inline void DirectoryModuleResolver::RegisterModuleFile(std::string_view path, const void* staticData, std::size_t size)
	{
		m_searchDirectory->StoreFile(path, staticData, size);
	}
}

#include <Nazara/Shader/DebugOff.hpp>
