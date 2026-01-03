// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_FILESYSTEMAPPCOMPONENT_HPP
#define NAZARA_CORE_FILESYSTEMAPPCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/VirtualDirectory.hpp>
#include <memory>
#include <unordered_map>

namespace Nz
{
	class NAZARA_CORE_API FilesystemAppComponent final : public ApplicationComponent
	{
		public:
			inline FilesystemAppComponent(ApplicationBase& app);
			FilesystemAppComponent(const FilesystemAppComponent&) = delete;
			FilesystemAppComponent(FilesystemAppComponent&&) = delete;
			~FilesystemAppComponent() = default;

			template<typename T> const typename T::Params* GetDefaultResourceParameters() const;
			inline VirtualDirectoryPtr GetDirectory(std::string_view dirPath);
			inline std::shared_ptr<Stream> GetFile(std::string_view filepath);
			template<typename F> bool GetFileContent(std::string_view filepath, F&& callback);

			template<typename F> bool IterateOnDirectory(std::string_view dirPath, F&& callback);

			template<typename T, typename... ExtraArgs> std::shared_ptr<T> Load(std::string_view assetPath, ExtraArgs&&... args);
			template<typename T, typename... ExtraArgs> std::shared_ptr<T> Load(std::string_view assetPath, typename T::Params params, ExtraArgs&&... args);

			const VirtualDirectoryPtr& Mount(std::string_view dirPath, std::filesystem::path filepath);
			const VirtualDirectoryPtr& Mount(std::string_view dirPath, VirtualDirectoryPtr directory);

			void MountDefaultDirectories();

			template<typename T, typename... ExtraArgs> std::shared_ptr<T> Open(std::string_view assetPath, ExtraArgs&&... args);
			template<typename T, typename... ExtraArgs> std::shared_ptr<T> Open(std::string_view assetPath, typename T::Params params, ExtraArgs&&... args);

			template<typename T> void SetDefaultResourceParameters(typename T::Params params);

			FilesystemAppComponent& operator=(const FilesystemAppComponent&) = delete;
			FilesystemAppComponent& operator=(FilesystemAppComponent&&) = delete;

		private:
			template<typename T, typename... ExtraArgs> std::shared_ptr<T> LoadImpl(std::string_view assetPath, const typename T::Params& params, ExtraArgs&&... args);
			template<typename T, typename... ExtraArgs> std::shared_ptr<T> OpenImpl(std::string_view assetPath, const typename T::Params& params, ExtraArgs&&... args);

			std::unordered_map<UInt64 /*typehash*/, std::unique_ptr<ResourceParameters>> m_defaultParameters;
			VirtualDirectoryPtr m_rootDirectory;
	};
}

#include <Nazara/Core/FilesystemAppComponent.inl>

#endif // NAZARA_CORE_FILESYSTEMAPPCOMPONENT_HPP
