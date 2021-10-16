// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/Algorithm.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline RenderSpriteChain::RenderSpriteChain(int renderLayer, std::shared_ptr<MaterialPass> materialPass, std::shared_ptr<RenderPipeline> renderPipeline, const ViewerInstance& viewerInstance, const WorldInstance& worldInstance, std::shared_ptr<VertexDeclaration> vertexDeclaration, std::shared_ptr<Texture> textureOverlay, std::size_t spriteCount, const void* spriteData) :
	RenderElement(BasicRenderElement::SpriteChain),
	m_materialPass(std::move(materialPass)),
	m_renderPipeline(std::move(renderPipeline)),
	m_vertexDeclaration(std::move(vertexDeclaration)),
	m_textureOverlay(std::move(textureOverlay)),
	m_spriteCount(spriteCount),
	m_spriteData(spriteData),
	m_viewerInstance(viewerInstance),
	m_worldInstance(worldInstance),
	m_renderLayer(renderLayer)
	{
	}

	inline UInt64 RenderSpriteChain::ComputeSortingScore(const Frustumf& frustum, const RenderQueueRegistry& registry) const
	{
		UInt64 layerIndex = registry.FetchLayerIndex(m_renderLayer);
		UInt64 elementType = GetElementType();
		UInt64 pipelineIndex = registry.FetchPipelineIndex(m_renderPipeline.get());
		UInt64 vertexDeclarationIndex = registry.FetchVertexDeclaration(m_vertexDeclaration.get());

		if (m_materialPass->IsFlagEnabled(MaterialPassFlag::Transparent))
		{
			UInt64 matFlags = 1;

			float distanceNear = frustum.GetPlane(FrustumPlane::Near).Distance(m_worldInstance.GetWorldMatrix().GetTranslation());
			UInt64 distance = DistanceAsSortKey(distanceNear);

			// Transparent RQ index:
			// - Layer (8bits)
			// - Transparent flag (1bit)
			// - Distance to near plane (32bits)

			return (layerIndex & 0xFF) << 60 |
			       (matFlags)          << 52 |
			       (distance)          << 51;

		}
		else
		{
			UInt64 matFlags = 0;

			// Opaque RQ index:
			// - Layer (8bits)
			// - Transparent flag (1bit)
			// - Element type (4bits)
			// - Pipeline (16bits)
			// - VertexDeclaration (8bits)
			// - ?? (24bits) - Depth?

			return (layerIndex & 0xFF)             << 60 |
			       (matFlags)                      << 52 |
			       (elementType & 0xF)             << 51 |
			       (pipelineIndex & 0xFFFF)        << 35 |
			       (vertexDeclarationIndex & 0xFF) << 23;
		}
	}

	inline const MaterialPass& RenderSpriteChain::GetMaterialPass() const
	{
		return *m_materialPass;
	}

	inline const RenderPipeline& RenderSpriteChain::GetRenderPipeline() const
	{
		return *m_renderPipeline;
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

	inline const ViewerInstance& RenderSpriteChain::GetViewerInstance() const
	{
		return m_viewerInstance;
	}

	inline const WorldInstance& RenderSpriteChain::GetWorldInstance() const
	{
		return m_worldInstance;
	}

	inline void RenderSpriteChain::Register(RenderQueueRegistry& registry) const
	{
		registry.RegisterLayer(m_renderLayer);
		registry.RegisterPipeline(m_renderPipeline.get());
		registry.RegisterVertexDeclaration(m_vertexDeclaration.get());
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
