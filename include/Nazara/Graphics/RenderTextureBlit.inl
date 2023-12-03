// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline RenderTextureBlit::RenderTextureBlit(const Vector2ui& textureSize, std::shared_ptr<Texture> targetTexture, SamplerFilter filter) :
	RenderTextureBlit(textureSize, std::move(targetTexture), filter, PipelineStage::FragmentShader, MemoryAccess::ShaderRead, TextureLayout::ColorInput)
	{
	}
	
	RenderTextureBlit::RenderTextureBlit(const Vector2ui& textureSize, std::shared_ptr<Texture> texture, SamplerFilter filter, PipelineStage targetPipelineStage, MemoryAccessFlags targetMemoryFlags, TextureLayout targetLayout) :
	m_targetTexture(std::move(texture)),
	m_targetMemoryFlags(targetMemoryFlags),
	m_targetPipelineStage(targetPipelineStage),
	m_samplerFilter(filter),
	m_targetLayout(targetLayout),
	m_textureSize(textureSize)
	{
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
