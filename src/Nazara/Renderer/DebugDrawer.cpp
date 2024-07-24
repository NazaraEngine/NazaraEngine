// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Core/Joint.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/RenderResources.hpp>
#include <NZSL/Serializer.hpp>
#include <NZSL/Ast/AstSerializer.hpp>
#include <NZSL/Math/FieldOffsets.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_debugDrawShader[] = {
			#include <Nazara/Renderer/Resources/Shaders/DebugDraw.nzslb.h>
		};
	}

	DebugDrawer::DebugDrawer(RenderDevice& renderDevice, std::size_t maxVertexPerDraw) :
	m_vertexPerBlock(maxVertexPerDraw),
	m_renderDevice(renderDevice),
	m_viewerDataUpdated(false)
	{
		nzsl::Deserializer deserializer(r_debugDrawShader, sizeof(r_debugDrawShader));
		nzsl::Ast::ModulePtr shaderModule = nzsl::Ast::DeserializeShader(deserializer);

		auto debugDrawShader = m_renderDevice.InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, *shaderModule, {});
		if (!debugDrawShader)
			throw std::runtime_error("failed to instantiate debug draw shader");

		RenderPipelineLayoutInfo layoutInfo;
		layoutInfo.bindings.assign({
			{
				0, 0, 1,
				ShaderBindingType::UniformBuffer,
				nzsl::ShaderStageType::Vertex
			}
		});

		m_renderPipelineLayout = m_renderDevice.InstantiateRenderPipelineLayout(std::move(layoutInfo));
		if (!m_renderPipelineLayout)
			throw std::runtime_error("failed to instantiate render pipeline layout");

		RenderPipelineInfo pipelineInfo;
		pipelineInfo.pipelineLayout = m_renderPipelineLayout;
		pipelineInfo.shaderModules.push_back(std::move(debugDrawShader));
		pipelineInfo.depthBuffer = true;
		pipelineInfo.depthWrite = true;

		pipelineInfo.blending = true;
		pipelineInfo.blend.srcColor = BlendFunc::SrcAlpha;
		pipelineInfo.blend.dstColor = BlendFunc::InvSrcAlpha;

		pipelineInfo.primitiveMode = PrimitiveMode::LineList;
		pipelineInfo.vertexBuffers.push_back({
			0,
			VertexDeclaration::Get(Nz::VertexLayout::XYZ_Color)
		});

		m_renderPipeline = m_renderDevice.InstantiateRenderPipeline(pipelineInfo);

		m_dataPool = std::make_shared<DataPool>();
	}

	DebugDrawer::~DebugDrawer()
	{
		m_drawCalls.clear();
		m_dataPool.reset();
		m_currentViewerData = {};
	}

	void DebugDrawer::Draw(CommandBufferBuilder& builder)
	{
		if (m_drawCalls.empty())
			return;

		builder.BindRenderShaderBinding(0, *m_currentViewerData.binding);
		builder.BindRenderPipeline(*m_renderPipeline);

		for (auto& drawCall : m_drawCalls)
		{
			builder.BindVertexBuffer(0, *drawCall.vertexBuffer);
			builder.Draw(SafeCaster(drawCall.vertexCount));
		}
	}

	void DebugDrawer::DrawSkeleton(const Skeleton& skeleton, const Color& color)
	{
		std::size_t jointCount = skeleton.GetJointCount();
		for (std::size_t i = 0; i < jointCount; ++i)
		{
			const Joint* joint = skeleton.GetJoint(i);
			const Node* parent = joint->GetParent();
			if (parent)
				DrawLine(joint->GetGlobalPosition(), parent->GetGlobalPosition(), color);
		}
	}

	void DebugDrawer::Prepare(RenderResources& renderResources)
	{
		UploadPool& uploadPool = renderResources.GetUploadPool();

		if (!m_lineVertices.empty())
		{
			std::size_t vertexCount = m_lineVertices.size();
			m_drawCalls.clear();
			m_drawCalls.reserve(vertexCount / m_vertexPerBlock + 1);

			m_pendingUploads.clear();
			m_pendingUploads.reserve(vertexCount / m_vertexPerBlock + 1);

			// Handle vertex buffers
			const VertexStruct_XYZ_Color* lineVertices = m_lineVertices.data();
			while (vertexCount > 0)
			{
				auto& drawCall = m_drawCalls.emplace_back();

				// Try to reuse vertex buffers from pool if any
				if (!m_dataPool->vertexBuffers.empty())
				{
					drawCall.vertexBuffer = std::move(m_dataPool->vertexBuffers.back());
					m_dataPool->vertexBuffers.pop_back();
				}
				else
					drawCall.vertexBuffer = m_renderDevice.InstantiateBuffer(BufferType::Vertex, m_vertexPerBlock * sizeof(VertexStruct_XYZ_Color), BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write);

				drawCall.vertexCount = std::min(vertexCount, m_vertexPerBlock);

				auto& pendingUpload = m_pendingUploads.emplace_back();
				pendingUpload.allocation = &uploadPool.Allocate(drawCall.vertexCount * sizeof(VertexStruct_XYZ_Color));
				pendingUpload.vertexBuffer = drawCall.vertexBuffer.get();
				std::memcpy(pendingUpload.allocation->mappedPtr, lineVertices, drawCall.vertexCount * sizeof(VertexStruct_XYZ_Color));

				lineVertices += drawCall.vertexCount;
				vertexCount -= drawCall.vertexCount;
			}
			m_lineVertices.clear();
		}

		// Handle viewer data
		if (m_viewerDataUpdated)
		{
			if (!m_dataPool->viewerData.empty())
			{
				m_currentViewerData = std::move(m_dataPool->viewerData.back());
				m_dataPool->viewerData.pop_back();
			}
			else
			{
				m_currentViewerData.buffer = m_renderDevice.InstantiateBuffer(BufferType::Uniform, m_viewerData.size(), BufferUsage::DeviceLocal | BufferUsage::Dynamic | BufferUsage::Write);
				m_currentViewerData.binding = m_renderPipelineLayout->AllocateShaderBinding(0);
				m_currentViewerData.binding->Update({
					{
						0,
						Nz::ShaderBinding::UniformBufferBinding {
							m_currentViewerData.buffer.get(),
							0, m_currentViewerData.buffer->GetSize()
						}
					}
				});
			}
		}

		if (m_viewerDataUpdated || !m_pendingUploads.empty())
		{
			renderResources.Execute([&](CommandBufferBuilder& builder)
			{
				builder.BeginDebugRegion("Debug drawer upload", Color::Yellow());
				{
					builder.MemoryBarrier(PipelineStage::VertexInput | PipelineStage::FragmentShader, PipelineStage::Transfer, MemoryAccess::VertexBufferRead | MemoryAccess::UniformBufferRead, MemoryAccess::TransferWrite);

					if (m_viewerDataUpdated)
					{
						const UploadPool::Allocation& viewerDataAllocation = uploadPool.Allocate(m_viewerData.size());
						std::memcpy(viewerDataAllocation.mappedPtr, m_viewerData.data(), m_viewerData.size());

						builder.CopyBuffer(viewerDataAllocation, m_currentViewerData.buffer.get());
					}

					for (auto& pendingUpload : m_pendingUploads)
						builder.CopyBuffer(*pendingUpload.allocation, pendingUpload.vertexBuffer);
					m_pendingUploads.clear();

					builder.MemoryBarrier(PipelineStage::Transfer, PipelineStage::VertexInput | PipelineStage::FragmentShader, MemoryAccess::TransferWrite, MemoryAccess::VertexBufferRead | MemoryAccess::UniformBufferRead);
				}
				builder.EndDebugRegion();
			}, QueueType::Graphics);
		}

		m_viewerDataUpdated = false;
	}

	void DebugDrawer::Reset(RenderResources& renderResources)
	{
		if (m_currentViewerData.binding)
		{
			// keep pipeline layout alive as needs to stay alive until all shader bindings have been freed
			renderResources.PushReleaseCallback([pool = m_dataPool, data = std::move(m_currentViewerData), pipelineLayout = m_renderPipelineLayout]() mutable
			{
				pool->viewerData.push_back(std::move(data));
			});
		}
		m_currentViewerData.binding = {};

		for (auto& drawCall : m_drawCalls)
		{
			renderResources.PushReleaseCallback([pool = m_dataPool, buffer = std::move(drawCall.vertexBuffer)]() mutable
			{
				pool->vertexBuffers.push_back(std::move(buffer));
			});
		}
		m_drawCalls.clear();

		m_lineVertices.clear();
	}

	void DebugDrawer::SetViewerData(const Matrix4f& viewProjMatrix)
	{
		// Setup viewer data buffer for current frame
		nzsl::FieldOffsets viewerDataFields(nzsl::StructLayout::Std140);
		std::size_t viewProjOffset = viewerDataFields.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);

		m_viewerData.resize(viewerDataFields.GetSize());
		AccessByOffset<Matrix4f&>(m_viewerData.data(), viewProjOffset) = viewProjMatrix;

		m_viewerDataUpdated = true;
	}
}
