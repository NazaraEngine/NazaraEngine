// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderLibrary.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzShader* NzShaderLibrary::Get(const NzString& name)
{
	auto it = s_library.find(name);
	if (it != s_library.end())
		return it->second;
	else
	{
		NazaraError("Shader \"" + name + "\" is not present");
		return nullptr;
	}
}

bool NzShaderLibrary::Has(const NzString& name)
{
	return s_library.find(name) != s_library.end();
}

bool NzShaderLibrary::Initialize()
{
	return true; // Que faire
}

void NzShaderLibrary::Register(const NzString& name, NzShader* shader)
{
	s_library.emplace(name, shader);
	NazaraDebug("Shader \"" + name + "\" registred");
}

void NzShaderLibrary::Uninitialize()
{
	for (auto it : s_library)
		NazaraWarning("Shader \"" + it.first + "\" has not been unregistred");

	s_library.clear();
}

void NzShaderLibrary::Unregister(const NzString& name)
{
	s_library.erase(name);
	NazaraDebug("Shader \"" + name + "\" unregistred");
}

std::unordered_map<NzString, NzShaderRef> NzShaderLibrary::s_library;
