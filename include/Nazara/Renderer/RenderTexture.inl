// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderTexture::RenderTexture(std::shared_ptr<Texture> targetTexture) :
	RenderTexture(std::move(targetTexture), PipelineStage::FragmentShader, MemoryAccess::ColorRead, TextureLayout::ColorInput)
	{
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
