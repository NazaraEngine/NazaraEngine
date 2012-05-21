// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCELOADER_HPP
#define NAZARA_RESOURCELOADER_HPP

#include <Nazara/Core/String.hpp>
#include <list>
#include <map>
#include <utility>

class NzInputStream;

template<typename Type, typename Parameters>
class NzResourceLoader
{
	public:
		typedef bool (*IsMemoryLoadingSupportedFunction)(const void* data, unsigned int size, const Parameters& parameters);
		typedef bool (*IsStreamLoadingSupportedFunction)(NzInputStream& stream, const Parameters& parameters);
		typedef bool (*LoadFileFunction)(Type* resource, const NzString& filePath, const Parameters& parameters);
		typedef bool (*LoadMemoryFunction)(Type* resource, const void* data, unsigned int size, const Parameters& parameters);
		typedef bool (*LoadStreamFunction)(Type* resource, NzInputStream& stream, const Parameters& parameters);

	protected:
		static bool LoadResourceFromFile(Type* resource, const NzString& filePath, const Parameters& parameters);
		static bool LoadResourceFromMemory(Type* resource, const void* data, unsigned int size, const Parameters& parameters);
		static bool LoadResourceFromStream(Type* resource, NzInputStream& stream, const Parameters& parameters);
		static void RegisterResourceFileLoader(const NzString& extensions, LoadFileFunction loadFile);
		static void RegisterResourceMemoryLoader(IsMemoryLoadingSupportedFunction isLoadingSupported, LoadMemoryFunction loadMemory);
		static void RegisterResourceStreamLoader(IsStreamLoadingSupportedFunction isLoadingSupported, LoadStreamFunction loadStream);
		static void UnregisterResourceFileLoader(const NzString& extensions, LoadFileFunction loadFile);
		static void UnregisterResourceMemoryLoader(IsMemoryLoadingSupportedFunction isLoadingSupported, LoadMemoryFunction loadMemory);
		static void UnregisterResourceStreamLoader(IsStreamLoadingSupportedFunction isLoadingSupported, LoadStreamFunction loadStream);

	private:
		typedef std::pair<IsMemoryLoadingSupportedFunction, LoadMemoryFunction> MemoryLoader;
		typedef std::pair<IsStreamLoadingSupportedFunction, LoadStreamFunction> StreamLoader;

		static std::list<MemoryLoader> s_memoryLoaders;
		static std::list<StreamLoader> s_streamLoaders;
		static std::multimap<NzString, LoadFileFunction> s_fileLoaders;
};

#include <Nazara/Utility/ResourceLoader.inl>

#endif // NAZARA_RESOURCELOADER_HPP
