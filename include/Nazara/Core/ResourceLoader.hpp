// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCELOADER_HPP
#define NAZARA_RESOURCELOADER_HPP

#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/String.hpp>
#include <list>
#include <tuple>

class NzInputStream;

template<typename Type, typename Parameters>
class NzResourceLoader
{
	public:
		using ExtensionGetter = bool (*)(const NzString& extension);
		using FileLoader = bool (*)(Type* resource, const NzString& filePath, const Parameters& parameters);
		using StreamChecker = nzTernary (*)(NzInputStream& stream, const Parameters& parameters);
		using StreamLoader = bool (*)(Type* resource, NzInputStream& stream, const Parameters& parameters);

		static bool IsExtensionSupported(const NzString& extension);

		static bool LoadFromFile(Type* resource, const NzString& filePath, const Parameters& parameters = Parameters());
		static bool LoadFromMemory(Type* resource, const void* data, unsigned int size, const Parameters& parameters = Parameters());
		static bool LoadFromStream(Type* resource, NzInputStream& stream, const Parameters& parameters = Parameters());

		static void RegisterLoader(ExtensionGetter extensionGetter, StreamChecker checkFunc, StreamLoader streamLoader, FileLoader fileLoader = nullptr);
		static void UnregisterLoader(ExtensionGetter extensionGetter, StreamChecker checkFunc, StreamLoader streamLoader, FileLoader fileLoader = nullptr);

		using Loader = std::tuple<ExtensionGetter, StreamChecker, StreamLoader, FileLoader>;
		using LoaderList = std::list<Loader>;
};

#include <Nazara/Core/ResourceLoader.inl>

#endif // NAZARA_RESOURCELOADER_HPP
