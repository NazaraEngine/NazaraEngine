// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ANDROID_ASSETDIRRESOLVER_HPP
#define NAZARA_CORE_ANDROID_ASSETDIRRESOLVER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/VirtualDirectory.hpp>
#include <android/asset_manager.h>

namespace Nz
{
	class NAZARA_CORE_API AndroidAssetDirResolver final : public VirtualDirectoryResolver
	{
		public:
			inline AndroidAssetDirResolver(AAssetManager* manager, std::string basePath, AAssetDir* dir);
			AndroidAssetDirResolver(const AndroidAssetDirResolver&) = delete;
			AndroidAssetDirResolver(AndroidAssetDirResolver&&) = delete;
			~AndroidAssetDirResolver();

			void ForEach(std::weak_ptr<VirtualDirectory> parent, FunctionRef<bool(std::string_view name, VirtualDirectory::Entry&& entry)> callback) const override;

			std::optional<VirtualDirectory::Entry> Resolve(std::weak_ptr<VirtualDirectory> parent, const std::string_view* parts, std::size_t partCount) const override;

			AndroidAssetDirResolver& operator=(const AndroidAssetDirResolver&) = delete;
			AndroidAssetDirResolver& operator=(AndroidAssetDirResolver&&) = delete;

		private:
			std::string m_basePath;
			AAssetDir* m_assetDir;
			AAssetManager* m_manager;
	};
}

#include <Nazara/Core/Android/AndroidAssetDirResolver.inl>

#endif // NAZARA_CORE_ANDROID_ASSETDIRRESOLVER_HPP
