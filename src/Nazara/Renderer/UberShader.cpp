// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/UberShader.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	UberShader::~UberShader()
	{
		OnUberShaderRelease(this);
	}

	bool UberShader::Initialize()
	{
		if (!UberShaderLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	void UberShader::Uninitialize()
	{
		UberShaderLibrary::Uninitialize();
	}

	UberShaderLibrary::LibraryMap UberShader::s_library;
}
