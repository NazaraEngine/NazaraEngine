// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCEMANAGER_HPP
#define NAZARA_RESOURCEMANAGER_HPP

#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/String.hpp>
#include <unordered_map>

template<typename Type, typename Parameters>
class NzResourceManager
{
	friend Type;

	public:
		NzResourceManager() = delete;
		~NzResourceManager() = delete;

		static void Clear();

		static NzObjectRef<Type> Get(const NzString& filePath);
		static const Parameters& GetDefaultParameters();

		static void Purge();
		static void Register(const NzString& filePath, NzObjectRef<Type> resource);
		static void SetDefaultParameters(const Parameters& params);
		static void Unregister(const NzString& filePath);

	private:
		static bool Initialize();
		static void Uninitialize();

		using ManagerMap = std::unordered_map<NzString, NzObjectRef<Type>>;
		using ManagerParams = Parameters;
};

#include <Nazara/Core/ResourceManager.inl>

#endif // NAZARA_RESOURCEMANAGER_HPP
