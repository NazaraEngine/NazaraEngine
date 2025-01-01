// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Algorithm.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>

namespace Nz
{
	inline RenderSubmesh::RenderSubmesh(int renderLayer, std::shared_ptr<MaterialInstance> materialInstance, MaterialPassFlags materialFlags, std::shared_ptr<RenderPipeline> renderPipeline, const WorldInstance& worldInstance, const SkeletonInstance* skeletonInstance, std::size_t indexCount, IndexType indexType, std::shared_ptr<RenderBuffer> indexBuffer, std::shared_ptr<RenderBuffer> vertexBuffer, const Recti& scissorBox) :
	RenderElement(BasicRenderElement::Submesh),
	m_indexBuffer(std::move(indexBuffer)),
	m_vertexBuffer(std::move(vertexBuffer)),
	m_materialInstance(std::move(materialInstance)),
	m_renderPipeline(std::move(renderPipeline)),
	m_indexCount(indexCount),
	m_skeletonInstance(skeletonInstance),
	m_worldInstance(worldInstance),
	m_indexType(indexType),
	m_materialFlags(materialFlags),
	m_scissorBox(scissorBox),
	m_renderLayer(renderLayer)
	{
	}

	inline UInt64 RenderSubmesh::ComputeSortingScore(const Frustumf& frustum, const RenderQueueRegistry& registry) const
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
			UInt64 vertexBufferIndex = registry.FetchVertexBuffer(m_vertexBuffer.get());

			UInt64 skeletonIndex = 0;
			if (m_skeletonInstance)
				skeletonIndex = registry.FetchSkeletonIndex(m_skeletonInstance->GetSkeleton().get());

			UInt64 matFlags = 0;

			// Opaque RQ index:
			// - Layer (8bits)
			// - Sorted by distance flag (1bit)
			// - Element type (4bits)
			// - Pipeline (16bits)
			// - MaterialPass (16bits)
			// - VertexBuffer (8bits)
			// - Skeleton (8bits)
			// - Unused (3bits)

			return (layerIndex & 0xFF)              << 56 |
			       (matFlags)                       << 55 |
			       (elementType & 0xF)              << 51 |
			       (pipelineIndex & 0xFFFF)         << 35 |
			       (materialInstanceIndex & 0xFFFF) << 19 |
			       (vertexBufferIndex & 0xFF)       << 11 |
			       (skeletonIndex     & 0xFF)       << 3;
		}
	}

	inline const RenderBuffer* RenderSubmesh::GetIndexBuffer() const
	{
		return m_indexBuffer.get();
	}

	inline std::size_t RenderSubmesh::GetIndexCount() const
	{
		return m_indexCount;
	}

	inline IndexType RenderSubmesh::GetIndexType() const
	{
		return m_indexType;
	}

	inline const MaterialInstance& RenderSubmesh::GetMaterialInstance() const
	{
		return *m_materialInstance;
	}

	inline const RenderPipeline* RenderSubmesh::GetRenderPipeline() const
	{
		return m_renderPipeline.get();
	}

	inline const Recti& RenderSubmesh::GetScissorBox() const
	{
		return m_scissorBox;
	}

	inline const SkeletonInstance* RenderSubmesh::GetSkeletonInstance() const
	{
		return m_skeletonInstance;
	}

	inline const RenderBuffer* RenderSubmesh::GetVertexBuffer() const
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
		registry.RegisterMaterialInstance(m_materialInstance.get());
		registry.RegisterPipeline(m_renderPipeline.get());
		registry.RegisterVertexBuffer(m_vertexBuffer.get());
		if (m_skeletonInstance)
			registry.RegisterSkeleton(m_skeletonInstance->GetSkeleton().get());
	}
}
