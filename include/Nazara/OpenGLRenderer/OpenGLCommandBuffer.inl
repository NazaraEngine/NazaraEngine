// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLFramebuffer.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
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

	inline void OpenGLCommandBuffer::SetFramebuffer(const OpenGLFramebuffer& framebuffer, const RenderPass& /*renderPass*/, std::initializer_list<CommandBufferBuilder::ClearValues> clearValues)
	{
		SetFrameBufferData setFramebuffer;
		setFramebuffer.framebuffer = &framebuffer;

		m_commands.emplace_back(std::move(setFramebuffer));
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
