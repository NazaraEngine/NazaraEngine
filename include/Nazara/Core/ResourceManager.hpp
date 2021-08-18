// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCEMANAGER_HPP
#define NAZARA_RESOURCEMANAGER_HPP

#include <Nazara/Core/ResourceLoader.hpp>
#include <filesystem>
#include <memory>
#include <unordered_map>

namespace Nz
{
	template<typename Type, typename Parameters>
	class ResourceManager
	{
		public:
			using Loader = ResourceLoader<Type, Parameters>;

			ResourceManager(Loader& loader);
			explicit ResourceManager(const ResourceManager&) = default;
			ResourceManager(ResourceManager&&) noexcept = default;
			~ResourceManager() = default;

			void Clear();

			std::shared_ptr<Type> Get(const std::filesystem::path& filePath);
			const Parameters& GetDefaultParameters();

			void Register(const std::filesystem::path& filePath, std::shared_ptr<Type> resource);
			void SetDefaultParameters(Parameters params);
			void Unregister(const std::filesystem::path& filePath);

			ResourceManager& operator=(const ResourceManager&) = delete;
			ResourceManager& operator=(ResourceManager&&) = delete;

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

#include <Nazara/Core/ResourceManager.inl>

#endif // NAZARA_RESOURCEMANAGER_HPP
