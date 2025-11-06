// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Android/AndroidAssetDirResolver.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Android/AndroidActivity.hpp>
#include <Nazara/Core/Android/AndroidAssetStream.hpp>

namespace Nz
{
	AndroidAssetDirResolver::AndroidAssetDirResolver(std::string basePath) :
	AndroidAssetDirResolver(AndroidActivity::Instance()->GetNativeActivity()->assetManager, std::move(basePath))
	{
	}

	AndroidAssetDirResolver::~AndroidAssetDirResolver()
	{
		if (m_assetDir)
			AAssetDir_close(m_assetDir);
	}

	void AndroidAssetDirResolver::ForEach(std::weak_ptr<VirtualDirectory> parent, FunctionRef<bool(std::string_view name, VirtualDirectory::Entry&& entry)> callback) const
	{
		if (!m_assetDir)
		{
			m_assetDir = AAssetManager_openDir(m_manager, m_basePath.c_str());
			if (!m_assetDir)
				return;
		}
		else
			AAssetDir_rewind(m_assetDir);

		while (const char* filename = AAssetDir_getNextFileName(m_assetDir))
		{
			std::string fullPath = m_basePath + "/" + filename;
			if (AAsset* asset = AAssetManager_open(m_manager, fullPath.c_str(), AASSET_MODE_RANDOM))
			{
				if (!callback(filename, VirtualDirectory::FileEntry{ std::make_shared<AndroidAssetStream>(std::move(fullPath), asset) }))
					return;
			}
		}
	}

	std::optional<VirtualDirectory::Entry> AndroidAssetDirResolver::Resolve(std::weak_ptr<VirtualDirectory> parent, const std::string_view* parts, std::size_t partCount) const
	{
		std::string fullPath = m_basePath;
		for (std::size_t i = 0; i < partCount; ++i)
		{
			if (!fullPath.empty())
				fullPath += "/";

			fullPath.append(parts[i]);
		}

		if (AAsset* asset = AAssetManager_open(m_manager, fullPath.c_str(), AASSET_MODE_RANDOM))
			return VirtualDirectory::FileEntry{ std::make_shared<AndroidAssetStream>(std::move(fullPath), asset) };
		else if (AAssetDir* assetDir = AAssetManager_openDir(m_manager, fullPath.c_str()))
		{
			// AAssetManager_openDir always returns a non-null pointer, even if the dir doesn't exist
			// So we check if they contains at least one file, consequence of this is that empty directories
			// won't be returned but that shouldn't be a big deal
			if (AAssetDir_getNextFileName(assetDir) != nullptr)
			{
				VirtualDirectoryPtr virtualDir = std::make_shared<VirtualDirectory>(std::make_shared<AndroidAssetDirResolver>(m_manager, std::move(fullPath), assetDir), parent);
				return VirtualDirectory::DirectoryEntry{ { std::move(virtualDir) } };
			}
			else
				AAssetDir_close(assetDir);
		}

		return std::nullopt; //< not found
	}
}
