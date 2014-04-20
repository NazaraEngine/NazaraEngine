// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/UberShaderLibrary.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzUberShader* NzUberShaderLibrary::Get(const NzString& name)
{
	auto it = s_library.find(name);
	if (it != s_library.end())
		return it->second;
	else
	{
		NazaraError("UberShader \"" + name + "\" is not present");
		return nullptr;
	}
}

bool NzUberShaderLibrary::Has(const NzString& name)
{
	return s_library.find(name) != s_library.end();
}

void NzUberShaderLibrary::Register(const NzString& name, NzUberShader* uberShader)
{
	s_library.emplace(name, uberShader);
	NazaraDebug("UberShader \"" + name + "\" registred");
}

void NzUberShaderLibrary::Unregister(const NzString& name)
{
	s_library.erase(name);
	NazaraDebug("UberShader \"" + name + "\" unregistred");
}

bool NzUberShaderLibrary::Initialize()
{
	return true; // Que faire
}

void NzUberShaderLibrary::Uninitialize()
{
	for (auto it : s_library)
		NazaraWarning("UberShader \"" + it.first + "\" has not been unregistred");

	s_library.clear();
}

std::unordered_map<NzString, NzUberShaderRef> NzUberShaderLibrary::s_library;
