// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline RenderSubmesh::RenderSubmesh(int renderLayer, std::shared_ptr<RenderPipeline> renderPipeline, std::size_t indexCount, std::shared_ptr<AbstractBuffer> indexBuffer, std::shared_ptr<AbstractBuffer> vertexBuffer, const ShaderBinding& instanceBinding, const ShaderBinding& materialBinding) :
	RenderElement(BasicRenderElement::Submesh),
	m_indexBuffer(std::move(indexBuffer)),
	m_vertexBuffer(std::move(vertexBuffer)),
	m_renderPipeline(std::move(renderPipeline)),
	m_indexCount(indexCount),
	m_instanceBinding(instanceBinding),
	m_materialBinding(materialBinding),
	m_renderLayer(renderLayer)
	{
	}

	inline UInt64 RenderSubmesh::ComputeSortingScore(const RenderQueueRegistry& registry) const
	{
		UInt64 elementType = GetElementType();
		UInt64 layerIndex = registry.FetchLayerIndex(m_renderLayer);
		UInt64 pipelineIndex = registry.FetchPipelineIndex(m_renderPipeline.get());
		UInt64 vertexBufferIndex = registry.FetchVertexBuffer(m_vertexBuffer.get());

		// RQ index:
		// - Element type (4bits)
		// - Layer (8bits)
		// - Pipeline (16bits)
		// - VertexBuffer (8bits)

		return (elementType & 0xF)        << 60 |
		       (layerIndex & 0xFF)        << 52 |
		       (pipelineIndex & 0xFFFF)   << 36 |
		       (vertexBufferIndex & 0xFF) << 24;
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
		return m_instanceBinding;
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
