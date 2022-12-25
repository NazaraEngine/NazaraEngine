// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLFramebuffer.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLCommandBuffer::OpenGLCommandBuffer() :
	m_maxColorBufferCount(0),
	m_owner(nullptr)
	{
	}

	inline OpenGLCommandBuffer::OpenGLCommandBuffer(OpenGLCommandPool& owner, std::size_t poolIndex, std::size_t bindingIndex) :
	m_bindingIndex(bindingIndex),
	m_maxColorBufferCount(0),
	m_poolIndex(poolIndex),
	m_owner(&owner)
	{
	}

	inline void OpenGLCommandBuffer::BeginDebugRegion(const std::string_view& regionName, const Color& color)
	{
		BeginDebugRegionData beginDebugRegion;
		beginDebugRegion.color = color;
		beginDebugRegion.regionName = regionName;

		m_commands.emplace_back(std::move(beginDebugRegion));
	}

	inline void OpenGLCommandBuffer::BindComputePipeline(const OpenGLComputePipeline* pipeline)
	{
		m_currentComputeStates.pipeline = pipeline;
	}

	inline void OpenGLCommandBuffer::BindIndexBuffer(GLuint indexBuffer, IndexType indexType, UInt64 offset)
	{
		m_currentDrawStates.indexBuffer = indexBuffer;
		m_currentDrawStates.indexBufferOffset = offset;
		m_currentDrawStates.indexBufferType = indexType;
	}

	inline void OpenGLCommandBuffer::BindRenderPipeline(const OpenGLRenderPipeline* pipeline)
	{
		m_currentDrawStates.pipeline = pipeline;
	}

	inline void OpenGLCommandBuffer::BindShaderBinding(const OpenGLRenderPipelineLayout& pipelineLayout, UInt32 set, const OpenGLShaderBinding* binding)
	{
		if (set >= m_currentDrawStates.shaderBindings.size())
			m_currentDrawStates.shaderBindings.resize(set + 1);

		m_currentDrawStates.shaderBindings[set] = std::make_pair(&pipelineLayout, binding);
	}

	inline void OpenGLCommandBuffer::BindVertexBuffer(UInt32 binding, GLuint vertexBuffer, UInt64 offset)
	{
		if (binding >= m_currentDrawStates.vertexBuffers.size())
			m_currentDrawStates.vertexBuffers.resize(binding + 1);

		auto& vertexBufferData = m_currentDrawStates.vertexBuffers[binding];
		vertexBufferData.offset = offset;
		vertexBufferData.vertexBuffer = vertexBuffer;
	}

	inline void OpenGLCommandBuffer::BlitTexture(const OpenGLTexture& source, const Boxui& sourceBox, const OpenGLTexture& target, const Boxui& targetBox, SamplerFilter filter)
	{
		BlitTextureData blitTexture = {
			&source,
			&target,
			sourceBox,
			targetBox,
			filter
		};

		m_commands.emplace_back(std::move(blitTexture));
	}

	inline void OpenGLCommandBuffer::CopyBuffer(GLuint source, GLuint target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		CopyBufferData copyBuffer = {
			source,
			target,
			size,
			sourceOffset,
			targetOffset
		};

		m_commands.emplace_back(std::move(copyBuffer));
	}

	inline void OpenGLCommandBuffer::CopyBuffer(const UploadPool::Allocation& allocation, GLuint target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		CopyBufferFromMemoryData copyBuffer = {
			static_cast<const UInt8*>(allocation.mappedPtr) + sourceOffset,
			target,
			size,
			targetOffset
		};

		m_commands.emplace_back(std::move(copyBuffer));
	}

	inline void OpenGLCommandBuffer::CopyTexture(const OpenGLTexture& source, const Boxui& sourceBox, const OpenGLTexture& target, const Vector3ui& targetPoint)
	{
		CopyTextureData copyTexture = {
			&source,
			&target,
			sourceBox,
			targetPoint
		};

		m_commands.emplace_back(std::move(copyTexture));
	}

	inline void OpenGLCommandBuffer::Dispatch(UInt32 numGroupsX, UInt32 numGroupsY, UInt32 numGroupsZ)
	{
		if (!m_currentComputeStates.pipeline)
			throw std::runtime_error("no pipeline bound");

		DispatchData dispatch;
		dispatch.states = m_currentComputeStates;
		dispatch.numGroupsX = numGroupsX;
		dispatch.numGroupsY = numGroupsY;
		dispatch.numGroupsZ = numGroupsZ;

		m_commands.emplace_back(std::move(dispatch));
	}

	inline void OpenGLCommandBuffer::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		if (!m_currentDrawStates.pipeline)
			throw std::runtime_error("no pipeline bound");

		DrawData draw;
		draw.states = m_currentDrawStates;
		draw.firstInstance = firstInstance;
		draw.firstVertex = firstVertex;
		draw.instanceCount = instanceCount;
		draw.vertexCount = vertexCount;

		m_commands.emplace_back(std::move(draw));
	}

	inline void OpenGLCommandBuffer::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex, UInt32 firstInstance)
	{
		if (!m_currentDrawStates.pipeline)
			throw std::runtime_error("no pipeline bound");

		DrawIndexedData draw;
		draw.states = m_currentDrawStates;
		draw.firstIndex = firstIndex;
		draw.firstInstance = firstInstance;
		draw.indexCount = indexCount;
		draw.instanceCount = instanceCount;

		m_commands.emplace_back(std::move(draw));
	}

	inline void OpenGLCommandBuffer::EndDebugRegion()
	{
		m_commands.emplace_back(EndDebugRegionData{});
	}

	inline std::size_t OpenGLCommandBuffer::GetBindingIndex() const
	{
		return m_bindingIndex;
	}

	inline std::size_t OpenGLCommandBuffer::GetPoolIndex() const
	{
		return m_poolIndex;
	}

	inline const OpenGLCommandPool& OpenGLCommandBuffer::GetOwner() const
	{
		assert(m_owner);
		return *m_owner;
	}

	inline void OpenGLCommandBuffer::SetFramebuffer(const OpenGLFramebuffer& framebuffer, const OpenGLRenderPass& renderPass, const CommandBufferBuilder::ClearValues* clearValues, std::size_t clearValueCount)
	{
		m_maxColorBufferCount = std::max(m_maxColorBufferCount, framebuffer.GetColorBufferCount());

		SetFrameBufferData setFramebuffer;
		setFramebuffer.framebuffer = &framebuffer;
		setFramebuffer.renderpass = &renderPass;

		assert(clearValueCount < setFramebuffer.clearValues.size());
		std::copy(clearValues, clearValues + clearValueCount, setFramebuffer.clearValues.begin());

		m_commands.emplace_back(std::move(setFramebuffer));

		m_currentDrawStates.shouldFlipY = (framebuffer.GetType() == FramebufferType::Window);
	}

	inline void OpenGLCommandBuffer::SetScissor(const Recti& scissorRegion)
	{
		m_currentDrawStates.scissorRegion = scissorRegion;
	}

	inline void OpenGLCommandBuffer::SetViewport(const Recti& viewportRegion)
	{
		m_currentDrawStates.viewportRegion = viewportRegion;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
