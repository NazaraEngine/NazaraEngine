// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/Algorithm.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline RenderSubmesh::RenderSubmesh(int renderLayer, std::shared_ptr<MaterialPass> materialPass, std::shared_ptr<RenderPipeline> renderPipeline, const WorldInstance& worldInstance, std::size_t indexCount, std::shared_ptr<AbstractBuffer> indexBuffer, std::shared_ptr<AbstractBuffer> vertexBuffer, const Recti& scissorBox) :
	RenderElement(BasicRenderElement::Submesh),
	m_indexBuffer(std::move(indexBuffer)),
	m_vertexBuffer(std::move(vertexBuffer)),
	m_materialPass(std::move(materialPass)),
	m_renderPipeline(std::move(renderPipeline)),
	m_indexCount(indexCount),
	m_worldInstance(worldInstance),
	m_scissorBox(scissorBox),
	m_renderLayer(renderLayer)
	{
	}

	inline UInt64 RenderSubmesh::ComputeSortingScore(const Frustumf& frustum, const RenderQueueRegistry& registry) const
	{
		UInt64 layerIndex = registry.FetchLayerIndex(m_renderLayer);
		
		if (m_materialPass->IsFlagEnabled(MaterialPassFlag::SortByDistance))
		{
			UInt64 matFlags = 1;

			float distanceNear = frustum.GetPlane(FrustumPlane::Near).Distance(m_worldInstance.GetWorldMatrix().GetTranslation());
			UInt64 distance = DistanceAsSortKey(distanceNear);

			// Transparent RQ index:
			// - Layer (8bits)
			// - Sorted by distance flag (1bit)
			// - Distance to near plane (32bits) - could by reduced to 24 or even 16 if required
			// - ?? (23bits)

			return (layerIndex & 0xFF) << 60 |
			       (matFlags)          << 52 |
			       (distance)          << 51;
		}
		else
		{
			UInt64 elementType = GetElementType();
			UInt64 materialPassIndex = registry.FetchMaterialPassIndex(m_materialPass.get());
			UInt64 pipelineIndex = registry.FetchPipelineIndex(m_renderPipeline.get());
			UInt64 vertexBufferIndex = registry.FetchVertexBuffer(m_vertexBuffer.get());

			UInt64 matFlags = 0;

			// Opaque RQ index:
			// - Layer (8bits)
			// - Sorted by distance flag (1bit)
			// - Element type (4bits)
			// - Pipeline (16bits)
			// - MaterialPass (16bits)
			// - VertexBuffer (8bits)
			// - ?? (8bits) - Depth?

			return (layerIndex & 0xFF)             << 60 |
			       (matFlags)                      << 52 |
			       (elementType & 0xF)             << 51 |
			       (pipelineIndex & 0xFFFF)        << 35 |
			       (materialPassIndex & 0xFFFF)    << 23 |
			       (vertexBufferIndex & 0xFF)      <<  7;
		}
	}

	inline const AbstractBuffer* RenderSubmesh::GetIndexBuffer() const
	{
		return m_indexBuffer.get();
	}

	inline std::size_t RenderSubmesh::GetIndexCount() const
	{
		return m_indexCount;
	}

	inline const MaterialPass& RenderSubmesh::GetMaterialPass() const
	{
		return *m_materialPass;
	}

	inline const RenderPipeline* RenderSubmesh::GetRenderPipeline() const
	{
		return m_renderPipeline.get();
	}

	inline const Recti& RenderSubmesh::GetScissorBox() const
	{
		return m_scissorBox;
	}

	inline const AbstractBuffer* RenderSubmesh::GetVertexBuffer() const
	{
		return m_vertexBuffer.get();
	}

	inline const WorldInstance& RenderSubmesh::GetWorldInstance() const
	{
		return m_worldInstance;
	}

	inline void RenderSubmesh::Register(RenderQueueRegistry& registry) const
	{
		registry.RegisterLayer(m_renderLayer);
		registry.RegisterMaterialPass(m_materialPass.get());
		registry.RegisterPipeline(m_renderPipeline.get());
		registry.RegisterVertexBuffer(m_vertexBuffer.get());
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
