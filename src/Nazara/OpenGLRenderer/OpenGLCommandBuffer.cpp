// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLVaoCache.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/VertexArray.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	namespace
	{
		void BuildAttrib(GL::OpenGLVaoSetup::Attribs& attrib, ComponentType component)
		{
			switch (component)
			{
				case ComponentType_Color:
					attrib.normalized = GL_TRUE;
					attrib.size = 4;
					attrib.type = GL_UNSIGNED_BYTE;
					return;

				case ComponentType_Float1:
				case ComponentType_Float2:
				case ComponentType_Float3:
				case ComponentType_Float4:
					attrib.normalized = GL_FALSE;
					attrib.size = (component - ComponentType_Float1 + 1);
					attrib.type = GL_FLOAT;
					return;

				case ComponentType_Int1:
				case ComponentType_Int2:
				case ComponentType_Int3:
				case ComponentType_Int4:
					attrib.normalized = GL_FALSE;
					attrib.size = (component - ComponentType_Int1 + 1);
					attrib.type = GL_INT;
					return;

				case ComponentType_Double1:
				case ComponentType_Double2:
				case ComponentType_Double3:
				case ComponentType_Double4:
				case ComponentType_Quaternion:
					break;
			}

			throw std::runtime_error("component type 0x" + NumberToString(component, 16) + " is not handled");
		}
	}

	void OpenGLCommandBuffer::Execute()
	{
		const GL::Context* context = GL::Context::GetCurrentContext();

		for (const auto& commandVariant : m_commands)
		{
			std::visit([&](auto&& command)
			{
				using T = std::decay_t<decltype(command)>;

				if constexpr (std::is_same_v<T, BeginDebugRegionData> || std::is_same_v<T, EndDebugRegionData>)
				{
					// TODO
				}
				else if constexpr (std::is_same_v<T, CopyBufferData>)
				{
					context->BindBuffer(GL::BufferTarget::CopyRead, command.source);
					context->BindBuffer(GL::BufferTarget::CopyWrite, command.target);
					context->glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, command.sourceOffset, command.targetOffset, command.size);
				}
				else if constexpr (std::is_same_v<T, CopyBufferFromMemoryData>)
				{
					context->BindBuffer(GL::BufferTarget::CopyWrite, command.target);
					context->glBufferSubData(GL_COPY_WRITE_BUFFER, command.targetOffset, command.size, command.memory);
				}
				else if constexpr (std::is_same_v<T, DrawData>)
				{
					ApplyStates(*context, command.states);
					context->glDrawArraysInstanced(ToOpenGL(command.states.pipeline->GetPipelineInfo().primitiveMode), command.firstVertex, command.vertexCount, command.instanceCount);
				}
				else if constexpr (std::is_same_v<T, DrawIndexedData>)
				{
					ApplyStates(*context, command.states);
					context->glDrawElementsInstanced(ToOpenGL(command.states.pipeline->GetPipelineInfo().primitiveMode), command.indexCount, GL_UNSIGNED_SHORT, nullptr, command.instanceCount);
				}
				else if constexpr (std::is_same_v<T, SetFrameBufferData>)
				{
					command.framebuffer->Activate();

					context = GL::Context::GetCurrentContext();

					if (command.framebuffer->GetType() == OpenGLFramebuffer::Type::FBO)
					{
						std::size_t colorBufferCount = command.framebuffer->GetColorBufferCount();
						for (std::size_t i = 0; i < colorBufferCount; ++i)
						{
							Nz::Color color = command.clearValues[i].color;
							std::array<GLuint, 4> clearColor = { color.r, color.g, color.b, color.a };

							context->glClearBufferuiv(GL_COLOR, GLint(i), clearColor.data());
						}
					}
					else
					{
						Nz::Color color = command.clearValues[0].color;
						context->glClearColor(color.r, color.g, color.b, color.a);
					}

					context->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				}
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");

			}, commandVariant);
		}
	}

	void OpenGLCommandBuffer::ApplyStates(const GL::Context& context, const DrawStates& states)
	{
		states.shaderBindings->Apply(context);
		states.pipeline->Apply(context);

		if (states.scissorRegion)
			context.SetScissorBox(states.scissorRegion->x, states.scissorRegion->y, states.scissorRegion->width, states.scissorRegion->height);

		if (states.viewportRegion)
			context.SetViewport(states.viewportRegion->x, states.viewportRegion->y, states.viewportRegion->width, states.viewportRegion->height);

		GL::OpenGLVaoSetup vaoSetup;
		vaoSetup.indexBuffer = states.indexBuffer;

		std::uint32_t locationIndex = 0;
		const std::uint8_t* originPtr = 0;

		for (const auto& bufferData : states.pipeline->GetPipelineInfo().vertexBuffers)
		{
			assert(bufferData.binding < states.vertexBuffers.size());
			const auto& vertexBufferInfo = states.vertexBuffers[bufferData.binding];

			GLsizei stride = GLsizei(bufferData.declaration->GetStride());

			for (const auto& componentInfo : bufferData.declaration->GetComponents())
			{
				auto& bufferAttribute = vaoSetup.vertexAttribs[locationIndex++].emplace();
				BuildAttrib(bufferAttribute, componentInfo.type);

				bufferAttribute.pointer = originPtr + vertexBufferInfo.offset + componentInfo.offset;
				bufferAttribute.stride = stride;
				bufferAttribute.vertexBuffer = vertexBufferInfo.vertexBuffer;
			}
		}

		const GL::VertexArray& vao = context.GetVaoCache().Get(vaoSetup);
		context.BindVertexArray(vao.GetObjectId(), true);
	}

	void OpenGLCommandBuffer::Release()
	{
		assert(m_owner);
		m_owner->Release(*this);
	}
}
