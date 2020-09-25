// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCEMANAGER_HPP
#define NAZARA_RESOURCEMANAGER_HPP

#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <filesystem>
#include <unordered_map>

namespace Nz
{
	template<typename Type, typename Parameters>
	class ResourceManager
	{
		friend Type;

		public:
			ResourceManager() = delete;
			~ResourceManager() = delete;

			static void Clear();

			static ObjectRef<Type> Get(const std::filesystem::path& filePath);
			static const Parameters& GetDefaultParameters();

			static void Purge();
			static void Register(const std::filesystem::path& filePath, ObjectRef<Type> resource);
			static void SetDefaultParameters(const Parameters& params);
			static void Unregister(const std::filesystem::path& filePath);

		private:
			static bool Initialize();
			static void Uninitialize();

			// https://stackoverflow.com/questions/51065244/is-there-no-standard-hash-for-stdfilesystempath
			struct PathHash
			{
				std::size_t operator()(const std::filesystem::path& p) const
				{
					return hash_value(p);
				}
			};

			using ManagerMap = std::unordered_map<std::filesystem::path, ObjectRef<Type>, PathHash>;
			using ManagerParams = Parameters;
	};
}

#include <Nazara/Core/ResourceManager.inl>

#endif // NAZARA_RESOURCEMANAGER_HPP
