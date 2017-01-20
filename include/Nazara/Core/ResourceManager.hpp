// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCEMANAGER_HPP
#define NAZARA_RESOURCEMANAGER_HPP

#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/String.hpp>
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

			static ObjectRef<Type> Get(const String& filePath);
			static const Parameters& GetDefaultParameters();

			static void Purge();
			static void Register(const String& filePath, ObjectRef<Type> resource);
			static void SetDefaultParameters(const Parameters& params);
			static void Unregister(const String& filePath);

		private:
			static bool Initialize();
			static void Uninitialize();

			using ManagerMap = std::unordered_map<String, ObjectRef<Type>>;
			using ManagerParams = Parameters;
	};
}

#include <Nazara/Core/ResourceManager.inl>

#endif // NAZARA_RESOURCEMANAGER_HPP
