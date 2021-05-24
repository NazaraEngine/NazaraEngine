// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/Core/StackArray.hpp>
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
				case ComponentType::Color:
					attrib.normalized = GL_TRUE;
					attrib.size = 4;
					attrib.type = GL_UNSIGNED_BYTE;
					return;

				case ComponentType::Float1:
				case ComponentType::Float2:
				case ComponentType::Float3:
				case ComponentType::Float4:
					attrib.normalized = GL_FALSE;
					attrib.size = (UnderlyingCast(component) - UnderlyingCast(ComponentType::Float1) + 1);
					attrib.type = GL_FLOAT;
					return;

				case ComponentType::Int1:
				case ComponentType::Int2:
				case ComponentType::Int3:
				case ComponentType::Int4:
					attrib.normalized = GL_FALSE;
					attrib.size = (UnderlyingCast(component) - UnderlyingCast(ComponentType::Int1) + 1);
					attrib.type = GL_INT;
					return;

				case ComponentType::Double1:
				case ComponentType::Double2:
				case ComponentType::Double3:
				case ComponentType::Double4:
				case ComponentType::Quaternion:
					break;
			}

			throw std::runtime_error("component type 0x" + NumberToString(UnderlyingCast(component), 16) + " is not handled");
		}
	}

	void OpenGLCommandBuffer::Execute()
	{
		const GL::Context* context = GL::Context::GetCurrentContext();

		StackArray<GLenum> fboDrawBuffers = NazaraStackArrayNoInit(GLenum, m_maxColorBufferCount);
		for (std::size_t i = 0; i < m_maxColorBufferCount; ++i)
			fboDrawBuffers[i] = GLenum(GL_COLOR_ATTACHMENT0 + i);

		for (const auto& commandVariant : m_commands)
		{
			std::visit([&](auto&& command)
			{
				using T = std::decay_t<decltype(command)>;

				if constexpr (std::is_same_v<T, BeginDebugRegionData>)
				{
					if (context->glPushDebugGroup)
						context->glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, command.regionName.size(), command.regionName.data());
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
				else if constexpr (std::is_same_v<T, EndDebugRegionData>)
				{
					if (context->glPopDebugGroup)
						context->glPopDebugGroup();
				}
				else if constexpr (std::is_same_v<T, SetFrameBufferData>)
				{
					command.framebuffer->Activate();

					context = GL::Context::GetCurrentContext();

					if (command.framebuffer->GetType() == OpenGLFramebuffer::Type::FBO)
					{
						std::size_t colorBufferCount = command.framebuffer->GetColorBufferCount();
						assert(colorBufferCount <= fboDrawBuffers.size());

						context->glDrawBuffers(GLsizei(colorBufferCount), fboDrawBuffers.data());

						//FIXME: Don't clear when not needed
						for (std::size_t i = 0; i < colorBufferCount; ++i)
						{
							Nz::Color color = command.clearValues[i].color;
							std::array<GLfloat, 4> clearColor = { color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f };

							context->glClearBufferfv(GL_COLOR, GLint(i), clearColor.data());
						}

						context->glClear(GL_DEPTH_BUFFER_BIT);
					}
					else
					{
						GLenum buffer = GL_BACK;
						context->glDrawBuffers(1, &buffer);

						//FIXME: Don't clear when not needed
						Nz::Color color = command.clearValues[0].color;
						context->glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
						context->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					}
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

		states.pipeline->FlipY(states.shouldFlipY);

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
