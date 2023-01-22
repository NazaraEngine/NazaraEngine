// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPFILESYSTEMCOMPONENT_HPP
#define NAZARA_CORE_APPFILESYSTEMCOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/VirtualDirectory.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API AppFilesystemComponent : public ApplicationComponent
	{
		public:
			using ApplicationComponent::ApplicationComponent;
			AppFilesystemComponent(const AppFilesystemComponent&) = delete;
			AppFilesystemComponent(AppFilesystemComponent&&) = delete;
			~AppFilesystemComponent() = default;

			template<typename T, typename... Args> std::shared_ptr<T> GetOrLoad(std::string_view assetPath, Args&&... args);

			inline const VirtualDirectoryPtr& RegisterPath(std::filesystem::path filepath);
			inline const VirtualDirectoryPtr& RegisterVirtualDirectory(VirtualDirectoryPtr directory);

			inline void UnregisterVirtualDirectory(const VirtualDirectoryPtr& directory);

			AppFilesystemComponent& operator=(const AppFilesystemComponent&) = delete;
			AppFilesystemComponent& operator=(AppFilesystemComponent&&) = delete;

		private:
			std::vector<VirtualDirectoryPtr> m_virtualDirectories;
	};
}

#include <Nazara/Core/AppFilesystemComponent.inl>

#endif // NAZARA_CORE_APPFILESYSTEMCOMPONENT_HPP
