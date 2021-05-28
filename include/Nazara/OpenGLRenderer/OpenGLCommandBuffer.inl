// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
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

	inline void OpenGLCommandBuffer::BeginDebugRegion(const std::string_view& regionName, const Nz::Color& color)
	{
		BeginDebugRegionData beginDebugRegion;
		beginDebugRegion.color = color;
		beginDebugRegion.regionName = regionName;

		m_commands.emplace_back(std::move(beginDebugRegion));
	}

	inline void OpenGLCommandBuffer::BindIndexBuffer(GLuint indexBuffer, UInt64 offset)
	{
		m_currentStates.indexBuffer = indexBuffer;
		m_currentStates.indexBufferOffset = offset;
	}

	inline void OpenGLCommandBuffer::BindPipeline(const OpenGLRenderPipeline* pipeline)
	{
		m_currentStates.pipeline = pipeline;
	}

	inline void OpenGLCommandBuffer::BindShaderBinding(const OpenGLShaderBinding* binding)
	{
		m_currentStates.shaderBindings = binding;
	}

	inline void OpenGLCommandBuffer::BindVertexBuffer(UInt32 binding, GLuint vertexBuffer, UInt64 offset)
	{
		if (binding >= m_currentStates.vertexBuffers.size())
			m_currentStates.vertexBuffers.resize(binding + 1);

		auto& vertexBufferData = m_currentStates.vertexBuffers[binding];
		vertexBufferData.offset = offset;
		vertexBufferData.vertexBuffer = vertexBuffer;
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

	inline void OpenGLCommandBuffer::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		if (!m_currentStates.pipeline)
			throw std::runtime_error("no pipeline bound");

		DrawData draw;
		draw.states = m_currentStates;
		draw.firstInstance = firstInstance;
		draw.firstVertex = firstVertex;
		draw.instanceCount = instanceCount;
		draw.vertexCount = vertexCount;

		m_commands.emplace_back(std::move(draw));
	}

	inline void OpenGLCommandBuffer::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		if (!m_currentStates.pipeline)
			throw std::runtime_error("no pipeline bound");

		DrawIndexedData draw;
		draw.states = m_currentStates;
		draw.firstInstance = firstInstance;
		draw.firstVertex = firstVertex;
		draw.indexCount = indexCount;
		draw.instanceCount = instanceCount;

		m_commands.emplace_back(std::move(draw));
	}

	inline void OpenGLCommandBuffer::EndDebugRegion()
	{
		m_commands.emplace_back(EndDebugRegionData{});
	}

	inline std::size_t Nz::OpenGLCommandBuffer::GetBindingIndex() const
	{
		return m_bindingIndex;
	}

	inline std::size_t Nz::OpenGLCommandBuffer::GetPoolIndex() const
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

		m_currentStates.shouldFlipY = (framebuffer.GetType() == OpenGLFramebuffer::Type::Window);
	}

	inline void OpenGLCommandBuffer::SetScissor(Nz::Recti scissorRegion)
	{
		m_currentStates.scissorRegion = scissorRegion;
	}

	inline void OpenGLCommandBuffer::SetViewport(Nz::Recti viewportRegion)
	{
		m_currentStates.viewportRegion = viewportRegion;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
