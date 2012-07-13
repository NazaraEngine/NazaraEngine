// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCELOADER_HPP
#define NAZARA_RESOURCELOADER_HPP

#include <Nazara/Core/String.hpp>
#include <utility>

class NzInputStream;

template<typename Type, typename Parameters>
class NzResourceLoader
{
	public:
		using IdentifyMemoryFunction = bool (*)(const void* data, unsigned int size, const Parameters& parameters);
		using IdentifyStreamFunction = bool (*)(NzInputStream& stream, const Parameters& parameters);
		using LoadFileFunction = bool (*)(Type* resource, const NzString& filePath, const Parameters& parameters);
		using LoadMemoryFunction = bool (*)(Type* resource, const void* data, unsigned int size, const Parameters& parameters);
		using LoadStreamFunction = bool (*)(Type* resource, NzInputStream& stream, const Parameters& parameters);

		static bool LoadFromFile(Type* resource, const NzString& filePath, const Parameters& parameters = Parameters());
		static bool LoadFromMemory(Type* resource, const void* data, unsigned int size, const Parameters& parameters = Parameters());
		static bool LoadFromStream(Type* resource, NzInputStream& stream, const Parameters& parameters = Parameters());

		static void RegisterFileLoader(const NzString& extensions, LoadFileFunction loadFile);
		static void RegisterMemoryLoader(IdentifyMemoryFunction identifyMemory, LoadMemoryFunction loadMemory);
		static void RegisterStreamLoader(IdentifyStreamFunction identifyStream, LoadStreamFunction loadStream);

		static void UnregisterFileLoader(const NzString& extensions, LoadFileFunction loadFile);
		static void UnregisterMemoryLoader(IdentifyMemoryFunction identifyMemory, LoadMemoryFunction loadMemory);
		static void UnregisterStreamLoader(IdentifyStreamFunction identifyStream, LoadStreamFunction loadStream);

		typedef std::pair<IdentifyMemoryFunction, LoadMemoryFunction> MemoryLoader;
		typedef std::pair<IdentifyStreamFunction, LoadStreamFunction> StreamLoader;
};

#include <Nazara/Utility/ResourceLoader.inl>

#endif // NAZARA_RESOURCELOADER_HPP
