// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPFILESYSTEMCOMPONENT_HPP
#define NAZARA_CORE_APPFILESYSTEMCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/AssetParameters.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/VirtualDirectory.hpp>
#include <memory>
#include <unordered_map>

namespace Nz
{
	class NAZARA_CORE_API AppFilesystemComponent : public ApplicationComponent
	{
		public:
			inline AppFilesystemComponent(ApplicationBase& app);
			AppFilesystemComponent(const AppFilesystemComponent&) = delete;
			AppFilesystemComponent(AppFilesystemComponent&&) = delete;
			~AppFilesystemComponent() = default;

			template<typename T> const typename T::Params* GetDefaultAssetParameters() const;
			inline VirtualDirectoryPtr GetDirectory(std::string_view assetPath);

			template<typename T, typename... ExtraArgs> std::shared_ptr<T> Load(std::string_view assetPath, ExtraArgs&&... args);
			template<typename T, typename... ExtraArgs> std::shared_ptr<T> Load(std::string_view assetPath, typename T::Params params, ExtraArgs&&... args);

			const VirtualDirectoryPtr& Mount(std::string_view name, std::filesystem::path filepath);
			const VirtualDirectoryPtr& Mount(std::string_view name, VirtualDirectoryPtr directory);

			void MountDefaultDirectories();

			template<typename T, typename... ExtraArgs> std::shared_ptr<T> Open(std::string_view assetPath, ExtraArgs&&... args);
			template<typename T, typename... ExtraArgs> std::shared_ptr<T> Open(std::string_view assetPath, typename T::Params params, ExtraArgs&&... args);

			template<typename T> void SetDefaultAssetParameters(typename T::Params params);

			AppFilesystemComponent& operator=(const AppFilesystemComponent&) = delete;
			AppFilesystemComponent& operator=(AppFilesystemComponent&&) = delete;

		private:
			template<typename T, typename... ExtraArgs> std::shared_ptr<T> LoadImpl(std::string_view assetPath, const typename T::Params& params, ExtraArgs&&... args);
			template<typename T, typename... ExtraArgs> std::shared_ptr<T> OpenImpl(std::string_view assetPath, const typename T::Params& params, ExtraArgs&&... args);

			std::unordered_map<UInt64 /*typehash*/, std::unique_ptr<AssetParameters>> m_defaultParameters;
			VirtualDirectoryPtr m_rootDirectory;
	};
}

#include <Nazara/Core/AppFilesystemComponent.inl>

#endif // NAZARA_CORE_APPFILESYSTEMCOMPONENT_HPP
