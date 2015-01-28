// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/UberShader.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzUberShader::~NzUberShader() = default;

bool NzUberShader::Initialize()
{
	if (!NzUberShaderLibrary::Initialize())
	{
		NazaraError("Failed to initialise library");
		return false;
	}

	return true;
}

void NzUberShader::Uninitialize()
{
	NzUberShaderLibrary::Uninitialize();
}

NzUberShaderLibrary::LibraryMap NzUberShader::s_library;
