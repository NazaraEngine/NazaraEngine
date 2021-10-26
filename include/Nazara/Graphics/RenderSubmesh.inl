// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/Algorithm.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline RenderSubmesh::RenderSubmesh(int renderLayer, std::shared_ptr<RenderPipeline> renderPipeline, std::size_t indexCount, std::shared_ptr<AbstractBuffer> indexBuffer, std::shared_ptr<AbstractBuffer> vertexBuffer, const WorldInstance& worldInstance, const ShaderBinding& materialBinding, const MaterialPassFlags& matFlags) :
	RenderElement(BasicRenderElement::Submesh),
	m_indexBuffer(std::move(indexBuffer)),
	m_vertexBuffer(std::move(vertexBuffer)),
	m_renderPipeline(std::move(renderPipeline)),
	m_indexCount(indexCount),
	m_matFlags(matFlags),
	m_materialBinding(materialBinding),
	m_worldInstance(worldInstance),
	m_renderLayer(renderLayer)
	{
	}

	inline UInt64 RenderSubmesh::ComputeSortingScore(const Nz::Frustumf& frustum, const RenderQueueRegistry& registry) const
	{
		UInt64 layerIndex = registry.FetchLayerIndex(m_renderLayer);
		UInt64 elementType = GetElementType();
		UInt64 pipelineIndex = registry.FetchPipelineIndex(m_renderPipeline.get());
		UInt64 vertexBufferIndex = registry.FetchVertexBuffer(m_vertexBuffer.get());
		
		if (m_matFlags.Test(MaterialPassFlag::Transparent))
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
			// - VertexBuffer (8bits)
			// - ?? (24bits) - Depth?

			return (layerIndex & 0xFF)             << 60 |
			       (matFlags)                      << 52 |
			       (elementType & 0xF)             << 51 |
			       (pipelineIndex & 0xFFFF)        << 35 |
			       (vertexBufferIndex & 0xFF)      << 23;
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

	inline const RenderPipeline* RenderSubmesh::GetRenderPipeline() const
	{
		return m_renderPipeline.get();
	}

	inline const ShaderBinding& RenderSubmesh::GetInstanceBinding() const
	{
		return m_worldInstance.GetShaderBinding();
	}

	inline const ShaderBinding& RenderSubmesh::GetMaterialBinding() const
	{
		return m_materialBinding;
	}

	inline const AbstractBuffer* RenderSubmesh::GetVertexBuffer() const
	{
		return m_vertexBuffer.get();
	}

	inline void RenderSubmesh::Register(RenderQueueRegistry& registry) const
	{
		registry.RegisterLayer(m_renderLayer);
		registry.RegisterPipeline(m_renderPipeline.get());
		registry.RegisterVertexBuffer(m_vertexBuffer.get());
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
