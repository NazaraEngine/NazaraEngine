// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Assert.hpp>

namespace Nz
{
	inline RenderSpriteChain::RenderSpriteChain(Int32 renderLayer, std::shared_ptr<MaterialProxy> materialProxy, MaterialPassFlags materialFlags, std::shared_ptr<RenderPipeline> renderPipeline, UInt32 instanceIndex, std::shared_ptr<VertexDeclaration> vertexDeclaration, std::size_t spriteCount, const void* spriteData, const Recti& scissorBox, UInt32 renderMask) :
	RenderElement(BasicRenderElement::SpriteChain, instanceIndex, renderLayer, renderMask),
	m_materialProxy(std::move(materialProxy)),
	m_renderPipeline(std::move(renderPipeline)),
	m_vertexDeclaration(std::move(vertexDeclaration)),
	m_spriteCount(spriteCount),
	m_spriteData(spriteData),
	m_materialFlags(materialFlags),
	m_scissorBox(scissorBox)
	{
		NazaraAssert(spriteCount < MaxSpritePerChain);
	}

	inline const MaterialProxy& RenderSpriteChain::GetMaterialProxy() const
	{
		return *m_materialProxy;
	}

	inline const RenderPipeline& RenderSpriteChain::GetRenderPipeline() const
	{
		return *m_renderPipeline;
	}

	inline const Recti& RenderSpriteChain::GetScissorBox() const
	{
		return m_scissorBox;
	}

	inline std::size_t RenderSpriteChain::GetSpriteCount() const
	{
		return m_spriteCount;
	}

	inline const void* RenderSpriteChain::GetSpriteData() const
	{
		return m_spriteData;
	}

	inline const VertexDeclaration* RenderSpriteChain::GetVertexDeclaration() const
	{
		return m_vertexDeclaration.get();
	}

	inline void RenderSpriteChain::Register(RenderQueueRegistry& registry) const
	{
		registry.RegisterMaterialProxy(m_materialProxy.get());
		registry.RegisterPipeline(m_renderPipeline.get());
		registry.RegisterVertexDeclaration(m_vertexDeclaration.get());
	}

	inline UInt64 RenderSpriteChain::ComputeSortKey(const RenderQueueRegistry& registry) const
	{
		UInt64 elementType = GetElementType();

		UInt64 materialProxyIndex = registry.FetchMaterialProxyIndex(m_materialProxy.get());
		UInt64 pipelineIndex = registry.FetchPipelineIndex(m_renderPipeline.get());
		UInt64 vertexDeclarationIndex = registry.FetchVertexDeclaration(m_vertexDeclaration.get());

		UInt64 layerIndex = 0;
		UInt64 matFlags = 0;

		// Opaque RQ index:
		// - Unused (previously layer) (8bits)
		// - Unused (previously sorted by distance flag) (1bit)
		// - Element type (4bits)
		// - Pipeline (16bits)
		// - MaterialPass (16bits)
		// - VertexDeclaration (8bits)
		// - ?? (11bits) - Depth?

		return (layerIndex & 0xFF)             << 56 |
			   (matFlags)                      << 55 |
			   (elementType & 0xF)             << 51 |
			   (pipelineIndex & 0xFFFF)        << 35 |
			   (materialProxyIndex & 0xFFFF)   << 19 |
			   (vertexDeclarationIndex & 0xFF) << 11;
	}
}
