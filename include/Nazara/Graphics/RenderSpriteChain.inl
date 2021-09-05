// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline RenderSpriteChain::RenderSpriteChain(int renderLayer, std::shared_ptr<RenderPipeline> renderPipeline, std::shared_ptr<VertexDeclaration> vertexDeclaration, std::shared_ptr<Texture> textureOverlay, std::size_t spriteCount, const void* spriteData, const ShaderBinding& materialBinding, const ShaderBinding& instanceBinding) :
	RenderElement(BasicRenderElement::SpriteChain),
	m_renderPipeline(std::move(renderPipeline)),
	m_vertexDeclaration(std::move(vertexDeclaration)),
	m_textureOverlay(std::move(textureOverlay)),
	m_spriteCount(spriteCount),
	m_spriteData(spriteData),
	m_instanceBinding(instanceBinding),
	m_materialBinding(materialBinding),
	m_renderLayer(renderLayer)
	{
	}

	inline UInt64 RenderSpriteChain::ComputeSortingScore(const RenderQueueRegistry& registry) const
	{
		UInt64 layerIndex = registry.FetchLayerIndex(m_renderLayer);
		UInt64 elementType = GetElementType();
		UInt64 pipelineIndex = registry.FetchPipelineIndex(m_renderPipeline.get());
		UInt64 vertexDeclarationIndex = registry.FetchVertexDeclaration(m_vertexDeclaration.get());

		// RQ index:
		// - Layer (8bits)
		// - Element type (4bits)
		// - Pipeline (16bits)
		// - VertexDeclaration (8bits)
		// - ?? (24bits) - Depth?

		return (layerIndex & 0xFF)             << 60 |
		       (elementType & 0xF)             << 52 |
		       (pipelineIndex & 0xFFFF)        << 36 |
		       (vertexDeclarationIndex & 0xFF) << 24;
	}

	inline const ShaderBinding& RenderSpriteChain::GetInstanceBinding() const
	{
		return m_instanceBinding;
	}

	inline const ShaderBinding& RenderSpriteChain::GetMaterialBinding() const
	{
		return m_materialBinding;
	}

	inline const RenderPipeline* RenderSpriteChain::GetRenderPipeline() const
	{
		return m_renderPipeline.get();
	}

	inline std::size_t RenderSpriteChain::GetSpriteCount() const
	{
		return m_spriteCount;
	}

	inline const void* RenderSpriteChain::GetSpriteData() const
	{
		return m_spriteData;
	}

	inline const Texture* RenderSpriteChain::GetTextureOverlay() const
	{
		return m_textureOverlay.get();
	}

	inline const VertexDeclaration* RenderSpriteChain::GetVertexDeclaration() const
	{
		return m_vertexDeclaration.get();
	}

	inline void RenderSpriteChain::Register(RenderQueueRegistry& registry) const
	{
		registry.RegisterLayer(m_renderLayer);
		registry.RegisterPipeline(m_renderPipeline.get());
		registry.RegisterVertexDeclaration(m_vertexDeclaration.get());
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
