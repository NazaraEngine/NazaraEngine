// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ASSETMANAGER_HPP
#define NAZARA_CORE_ASSETMANAGER_HPP

#include <Nazara/Core/AssetLoader.hpp>
#include <filesystem>
#include <memory>
#include <unordered_map>

namespace Nz
{
	template<typename Type, typename Parameters>
	class AssetManager
	{
		public:
			using Loader = AssetLoader<Type, Parameters>;

			AssetManager(Loader& loader);
			explicit AssetManager(const AssetManager&) = default;
			AssetManager(AssetManager&&) noexcept = default;
			~AssetManager() = default;

			void Clear();

			std::shared_ptr<Type> Get(const std::filesystem::path& filePath);
			const Parameters& GetDefaultParameters();

			void Register(const std::filesystem::path& filePath, std::shared_ptr<Type> resource);
			void SetDefaultParameters(Parameters params);
			void Unregister(const std::filesystem::path& filePath);

			AssetManager& operator=(const AssetManager&) = delete;
			AssetManager& operator=(AssetManager&&) = delete;

		private:
			// https://stackoverflow.com/questions/51065244/is-there-no-standard-hash-for-stdfilesystempath
			struct PathHash
			{
				std::size_t operator()(const std::filesystem::path& p) const
				{
					return hash_value(p);
				}
			};

			std::unordered_map<std::filesystem::path, std::shared_ptr<Type>, PathHash> m_resources;
			Loader& m_loader;
			Parameters m_defaultParameters;
	};
}

#include <Nazara/Core/AssetManager.inl>

#endif // NAZARA_CORE_ASSETMANAGER_HPP
