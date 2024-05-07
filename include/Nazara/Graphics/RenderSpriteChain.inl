// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Algorithm.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>

namespace Nz
{
	inline RenderSpriteChain::RenderSpriteChain(int renderLayer, std::shared_ptr<MaterialInstance> materialInstance, MaterialPassFlags materialFlags, std::shared_ptr<RenderPipeline> renderPipeline, const WorldInstance& worldInstance, std::shared_ptr<VertexDeclaration> vertexDeclaration, std::shared_ptr<TextureAsset> textureOverlay, std::size_t spriteCount, const void* spriteData, const Recti& scissorBox) :
	RenderElement(BasicRenderElement::SpriteChain),
	m_materialInstance(std::move(materialInstance)),
	m_renderPipeline(std::move(renderPipeline)),
	m_vertexDeclaration(std::move(vertexDeclaration)),
	m_textureOverlay(std::move(textureOverlay)),
	m_spriteCount(spriteCount),
	m_spriteData(spriteData),
	m_worldInstance(worldInstance),
	m_materialFlags(materialFlags),
	m_scissorBox(scissorBox),
	m_renderLayer(renderLayer)
	{
	}

	inline UInt64 RenderSpriteChain::ComputeSortingScore(const Frustumf& frustum, const RenderQueueRegistry& registry) const
	{
		UInt64 layerIndex = registry.FetchLayerIndex(m_renderLayer);

		if (m_materialFlags.Test(MaterialPassFlag::SortByDistance))
		{
			UInt64 matFlags = 1;

			float distanceNear = frustum.GetPlane(FrustumPlane::Near).SignedDistance(m_worldInstance.GetWorldMatrix().GetTranslation());
			UInt64 distance = DistanceAsSortKey(distanceNear);

			// Transparent RQ index:
			// - Layer (8bits)
			// - Sorted by distance flag (1bit)
			// - Distance to near plane (32bits) - could by reduced to 24 or even 16 if required
			// - ?? (23bits)

			return (layerIndex & 0xFF) << 56 |
			       (matFlags)          << 55 |
			       (distance)          << 23;

		}
		else
		{
			UInt64 elementType = GetElementType();
			UInt64 materialInstanceIndex = registry.FetchMaterialInstanceIndex(m_materialInstance.get());
			UInt64 pipelineIndex = registry.FetchPipelineIndex(m_renderPipeline.get());
			UInt64 vertexDeclarationIndex = registry.FetchVertexDeclaration(m_vertexDeclaration.get());

			UInt64 matFlags = 0;

			// Opaque RQ index:
			// - Layer (8bits)
			// - Sorted by distance flag (1bit)
			// - Element type (4bits)
			// - Pipeline (16bits)
			// - MaterialPass (16bits)
			// - VertexDeclaration (8bits)
			// - ?? (11bits) - Depth?

			return (layerIndex & 0xFF)              << 56 |
			       (matFlags)                       << 55 |
			       (elementType & 0xF)              << 51 |
			       (pipelineIndex & 0xFFFF)         << 35 |
			       (materialInstanceIndex & 0xFFFF) << 19 |
			       (vertexDeclarationIndex & 0xFF)  << 11;
		}
	}

	inline const MaterialInstance& RenderSpriteChain::GetMaterialInstance() const
	{
		return *m_materialInstance;
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

	inline const TextureAsset* RenderSpriteChain::GetTextureOverlay() const
	{
		return m_textureOverlay.get();
	}

	inline const VertexDeclaration* RenderSpriteChain::GetVertexDeclaration() const
	{
		return m_vertexDeclaration.get();
	}

	inline const WorldInstance& RenderSpriteChain::GetWorldInstance() const
	{
		return m_worldInstance;
	}

	inline void RenderSpriteChain::Register(RenderQueueRegistry& registry) const
	{
		registry.RegisterLayer(m_renderLayer);
		registry.RegisterMaterialInstance(m_materialInstance.get());
		registry.RegisterPipeline(m_renderPipeline.get());
		registry.RegisterVertexDeclaration(m_vertexDeclaration.get());
	}
}

