// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCELOADER_HPP
#define NAZARA_RESOURCELOADER_HPP

#include <Nazara/Core/String.hpp>
#include <list>
#include <set>
#include <tuple>

class NzInputStream;

template<typename Type, typename Parameters>
class NzResourceLoader
{
	public:
		using CheckFunction = bool (*)(NzInputStream& stream, const Parameters& parameters);
		using LoadFunction = bool (*)(Type* resource, NzInputStream& stream, const Parameters& parameters);

		static bool LoadFromFile(Type* resource, const NzString& filePath, const Parameters& parameters = Parameters());
		static bool LoadFromMemory(Type* resource, const void* data, unsigned int size, const Parameters& parameters = Parameters());
		static bool LoadFromStream(Type* resource, NzInputStream& stream, const Parameters& parameters = Parameters());

		static void RegisterLoader(const NzString& fileExtensions, CheckFunction checkFunc, LoadFunction loadfunc);
		static void UnregisterLoader(const NzString& fileExtensions, CheckFunction checkFunc, LoadFunction loadfunc);

		using Loader = std::tuple<std::set<NzString>, CheckFunction, LoadFunction>;
		using LoaderList = std::list<Loader>;
};

#include <Nazara/Core/ResourceLoader.inl>

#endif // NAZARA_RESOURCELOADER_HPP
