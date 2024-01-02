// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLComputePipeline.hpp>
#include <Nazara/OpenGLRenderer/OpenGLFboFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/OpenGLRenderer/OpenGLVaoCache.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/VertexArray.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <NazaraUtils/StackVector.hpp>
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
					attrib.normalized = GL_FALSE;
					attrib.size = 4;
					attrib.type = GL_FLOAT;
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
					break;
			}

			throw std::runtime_error("component type 0x" + NumberToString(UnderlyingCast(component), 16) + " is not handled");
		}
	}

	void OpenGLCommandBuffer::Execute()
	{
		const GL::Context* context = GL::Context::GetCurrentContext();

		for (const auto& commandVariant : m_commands)
		{
			switch (commandVariant.index())
			{
#define NAZARA_OPENGL_COMMAND_CALLBACK(Command) \
				case TypeListFind<CommandList, Command>: \
					Execute(context, std::get<TypeListFind<CommandList, Command>>(commandVariant)); \
					break;

				NAZARA_OPENGL_FOREACH_COMMANDS(NAZARA_OPENGL_COMMAND_CALLBACK, NAZARA_OPENGL_COMMAND_CALLBACK)
#undef NAZARA_OPENGL_COMMAND_CALLBACK
			}
		}
	}

	void OpenGLCommandBuffer::UpdateDebugName(std::string_view /*name*/)
	{
		// No OpenGL object to name
	}

	void OpenGLCommandBuffer::ApplyBindings(const GL::Context& context, const ShaderBindings& states)
	{
		unsigned int setIndex = 0;
		for (const auto& [pipelineLayout, shaderBinding] : states.shaderBindings)
		{
			if (shaderBinding)
				shaderBinding->Apply(*pipelineLayout, setIndex, context);
			else
				NazaraWarningFmt("no shader binding for set #{0}", setIndex);

			setIndex++;
		}
	}

	void OpenGLCommandBuffer::ApplyStates(const GL::Context& context, const DrawStates& states)
	{
		states.pipeline->Apply(context, states.shouldFlipY);

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
		context.BindVertexArray(vao.GetObjectId());
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const BeginDebugRegionCommand& command)
	{
		if (context->glPushDebugGroup)
			context->glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(command.regionName.size()), command.regionName.data());
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const BlitTextureCommand& command)
	{
		context->BlitTexture(*command.source, *command.target, command.sourceBox, command.targetBox, command.filter);
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const BlitTextureToWindowCommand& command)
	{
		context->BlitTextureToWindow(*command.source,  command.sourceBox, command.targetBox, command.filter);
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* /*context*/, const BuildTextureMipmapsCommand& command)
	{
		command.texture->GenerateMipmaps(command.baseLevel, command.levelCount);
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const CopyBufferCommand& command)
	{
		context->BindBuffer(GL::BufferTarget::CopyRead, command.source);
		context->BindBuffer(GL::BufferTarget::CopyWrite, command.target);
		context->glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, command.sourceOffset, command.targetOffset, command.size);
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const CopyBufferFromMemoryCommand& command)
	{
		context->BindBuffer(GL::BufferTarget::CopyWrite, command.target);
		context->glBufferSubData(GL_COPY_WRITE_BUFFER, command.targetOffset, command.size, command.memory);
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const CopyTextureCommand& command)
	{
		context->CopyTexture(*command.source, *command.target, command.sourceBox, command.targetPoint);
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const DispatchCommand& command)
	{
		if (!context->glDispatchCompute)
			throw std::runtime_error("compute shaders are not supported on this device");

		command.states.pipeline->Apply(*context);
		ApplyBindings(*context, command.bindings);
		context->glDispatchCompute(command.numGroupsX, command.numGroupsY, command.numGroupsZ);
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const DrawCommand& command)
	{
		ApplyStates(*context, command.states);
		ApplyBindings(*context, command.bindings);
		context->glDrawArraysInstanced(ToOpenGL(command.states.pipeline->GetPipelineInfo().primitiveMode), command.firstVertex, command.vertexCount, command.instanceCount);
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const DrawIndexedCommand& command)
	{
		const UInt8* origin = 0; //< For an easy way to cast an integer to a pointer
		origin += command.states.indexBufferOffset;

		switch (command.states.indexBufferType)
		{
			case IndexType::U8:  origin += command.firstIndex * sizeof(UInt8); break;
			case IndexType::U16: origin += command.firstIndex * sizeof(UInt16); break;
			case IndexType::U32: origin += command.firstIndex * sizeof(UInt32); break;
		}

		ApplyStates(*context, command.states);
		ApplyBindings(*context, command.bindings);
		context->glDrawElementsInstanced(ToOpenGL(command.states.pipeline->GetPipelineInfo().primitiveMode), command.indexCount, ToOpenGL(command.states.indexBufferType), origin, command.instanceCount);
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const EndDebugRegionCommand& /*command*/)
	{
		if (context->glPopDebugGroup)
			context->glPopDebugGroup();
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const InsertDebugLabelCommand& command)
	{
		if (context->glDebugMessageInsert)
			context->glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, SafeCast<GLsizei>(command.label.size()), command.label.data());
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context* context, const MemoryBarrier& command)
	{
		if (context->glMemoryBarrier)
			context->glMemoryBarrier(command.barriers);
	}

	inline void OpenGLCommandBuffer::Execute(const GL::Context*& context, const SetFrameBufferCommand& command)
	{
		command.framebuffer->Activate();

		StackArray<std::size_t> colorIndexes = NazaraStackArrayNoInit(std::size_t, m_maxColorBufferCount);

		std::size_t colorBufferCount = command.framebuffer->GetColorBufferCount();
		assert(colorBufferCount <= colorIndexes.size());

		colorIndexes.fill(0);
		std::size_t colorIndex = 0;

		GLbitfield clearFields = 0;
		std::optional<std::size_t> depthStencilIndex;

		std::size_t attachmentCount = command.renderpass->GetAttachmentCount();

		for (std::size_t i = 0; i < attachmentCount; ++i)
		{
			const auto& attachmentInfo = command.renderpass->GetAttachment(i);
			switch (PixelFormatInfo::GetContent(attachmentInfo.format))
			{
				case PixelFormatContent::Undefined:
					break;

				case PixelFormatContent::ColorRGBA:
					colorIndexes[colorIndex++] = i;
					break;

				case PixelFormatContent::Depth:
					if (!depthStencilIndex)
						depthStencilIndex = i;
					break;

				case PixelFormatContent::DepthStencil:
					if (!depthStencilIndex)
						depthStencilIndex = i;
					break;

				case PixelFormatContent::Stencil:
					//FIXME: I'm not sure stencil is properly handled here
					if (!depthStencilIndex)
						depthStencilIndex = i;
					break;
			}
		}

		StackVector<GLenum> invalidateAttachments;

		if (command.framebuffer->GetType() == FramebufferType::Texture)
		{
			const OpenGLFboFramebuffer& fboFramebuffer = static_cast<const OpenGLFboFramebuffer&>(*command.framebuffer);

			invalidateAttachments = NazaraStackVector(GLenum, colorBufferCount + 1);

			for (std::size_t i = 0; i < colorBufferCount; ++i)
			{
				std::size_t attachmentIndex = colorIndexes[i];

				Color color = command.clearValues[attachmentIndex].color;
				std::array<GLfloat, 4> clearColor = { color.r, color.g, color.b, color.a };

				const auto& attachmentInfo = command.renderpass->GetAttachment(attachmentIndex);
				if (attachmentInfo.loadOp == AttachmentLoadOp::Clear)
				{
					context->ResetColorWriteMasks();

					// Reset scissor as it affects clear commands if enabled (disabling it would work too but it seems more expansive)
					const Vector2ui& attachmentSize = fboFramebuffer.GetAttachmentSize(i);
					context->SetScissorBox(0, 0, attachmentSize.x, attachmentSize.y);

					context->glClearBufferfv(GL_COLOR, GLint(i), clearColor.data());
				}
				else if (attachmentInfo.loadOp == AttachmentLoadOp::Discard)
					invalidateAttachments.push_back(static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i));
			}

			if (depthStencilIndex)
			{
				std::size_t attachmentIndex = *depthStencilIndex;
				const auto& clearValues = command.clearValues[attachmentIndex];

				const auto& depthStencilAttachment = command.renderpass->GetAttachment(attachmentIndex);

				// Reset scissor as it affects clear commands if enabled (disabling it would work too but it seems more expansive)
				if (depthStencilAttachment.loadOp == AttachmentLoadOp::Clear || depthStencilAttachment.stencilLoadOp == AttachmentLoadOp::Clear)
				{
					const Vector2ui& attachmentSize = fboFramebuffer.GetAttachmentSize(attachmentIndex);
					context->SetScissorBox(0, 0, attachmentSize.x, attachmentSize.y);
				}

				if (depthStencilAttachment.loadOp == AttachmentLoadOp::Clear && depthStencilAttachment.stencilLoadOp == AttachmentLoadOp::Clear)
				{
					context->ResetDepthWriteMasks();
					context->ResetStencilWriteMasks();
					context->glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValues.depth, clearValues.stencil);
				}
				else if (depthStencilAttachment.loadOp == AttachmentLoadOp::Clear)
				{
					context->ResetDepthWriteMasks();
					context->glClearBufferfv(GL_DEPTH, 0, &clearValues.depth);

					if (depthStencilAttachment.stencilLoadOp == AttachmentLoadOp::Discard)
						invalidateAttachments.push_back(GL_STENCIL_ATTACHMENT);
				}
				else if (depthStencilAttachment.stencilLoadOp == AttachmentLoadOp::Clear)
				{
					context->ResetStencilWriteMasks();
					context->glClearBufferuiv(GL_STENCIL, 0, &clearValues.stencil);

					if (depthStencilAttachment.loadOp == AttachmentLoadOp::Discard)
						invalidateAttachments.push_back(GL_DEPTH_ATTACHMENT);
				}
				else if (depthStencilAttachment.loadOp == AttachmentLoadOp::Discard && depthStencilAttachment.stencilLoadOp == AttachmentLoadOp::Discard)
					invalidateAttachments.push_back(GL_DEPTH_STENCIL_ATTACHMENT);
			}
		}
		else
		{
			assert(command.framebuffer->GetType() == FramebufferType::Window);

			// Window framebuffers have their own contexts
			context = GL::Context::GetCurrentContext();

			// glDrawBuffers doesn't accept GL_BACK on OpenGL non-ES, and glDrawBuffer must be used instead
			if (context->GetParams().type != GL::ContextType::OpenGL_ES && context->glDrawBuffer)
				context->glDrawBuffer(GL_BACK);
			else
			{
				GLenum buffer = GL_BACK;
				context->glDrawBuffers(1, &buffer);
			}

			invalidateAttachments = NazaraStackVector(GLenum, 3); //< color + depth + stencil

			if (colorIndex > 0)
			{
				assert(colorBufferCount <= 1);

				std::size_t colorAttachmentIndex = colorIndexes.front();

				const auto& colorAttachment = command.renderpass->GetAttachment(colorAttachmentIndex);
				if (colorAttachment.loadOp == AttachmentLoadOp::Clear)
				{
					context->ResetColorWriteMasks();

					const Color& color = command.clearValues[colorAttachmentIndex].color;
					context->glClearColor(color.r, color.g, color.b, color.a);

					clearFields |= GL_COLOR_BUFFER_BIT;
				}
				else if (colorAttachment.loadOp == AttachmentLoadOp::Discard)
					invalidateAttachments.push_back(GL_COLOR);
			}

			if (depthStencilIndex)
			{
				std::size_t attachmentIndex = *depthStencilIndex;
				const auto& clearValues = command.clearValues[attachmentIndex];

				const auto& depthStencilAttachment = command.renderpass->GetAttachment(attachmentIndex);
				if (depthStencilAttachment.loadOp == AttachmentLoadOp::Clear)
				{
					context->ResetDepthWriteMasks();
					context->glClearDepthf(clearValues.depth);
					clearFields |= GL_DEPTH_BUFFER_BIT;
				}
				else if (depthStencilAttachment.loadOp == AttachmentLoadOp::Discard)
					invalidateAttachments.push_back(GL_DEPTH);

				if (depthStencilAttachment.stencilLoadOp == AttachmentLoadOp::Clear && PixelFormatInfo::GetContent(depthStencilAttachment.format) == PixelFormatContent::DepthStencil)
				{
					context->ResetStencilWriteMasks();
					context->glClearStencil(clearValues.stencil);
					clearFields |= GL_STENCIL_BUFFER_BIT;
				}
				else if (depthStencilAttachment.stencilLoadOp == AttachmentLoadOp::Discard)
					invalidateAttachments.push_back(GL_STENCIL);
			}

			if (clearFields)
			{
				// Reset scissor as it affects clear commands if enabled (disabling it would work too but it seems more expansive)
				const Vector2ui& size = command.framebuffer->GetSize();
				context->SetScissorBox(0, 0, size.x, size.y);

				context->glClear(clearFields);
			}
		}

		if (!invalidateAttachments.empty())
			context->glInvalidateFramebuffer(GL_FRAMEBUFFER, GLsizei(invalidateAttachments.size()), invalidateAttachments.data());
	}

	void OpenGLCommandBuffer::Release()
	{
		assert(m_owner);
		m_owner->Release(*this);
	}
}
