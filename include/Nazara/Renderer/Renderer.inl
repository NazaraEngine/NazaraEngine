// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RendererImpl* Renderer::GetRendererImpl()
	{
		return s_rendererImpl.get();
	}

	inline bool Renderer::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Renderer::SetParameters(const ParameterList& parameters)
	{
		s_initializationParameters = parameters;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
