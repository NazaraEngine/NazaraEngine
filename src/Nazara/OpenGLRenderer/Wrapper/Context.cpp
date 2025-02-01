// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Framebuffer.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <cstring>
#include <sstream>
#include <stdexcept>

namespace Nz::GL
{
	thread_local constinit const Context* s_currentGLContext = nullptr;

	namespace
	{
		constexpr std::array s_functionNames = {
#define NAZARA_OPENGLRENDERER_FUNCTION(name, sig) #name,
#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>
		};

		template<typename FuncType, std::size_t FuncIndex, typename>
		struct GLWrapper;

		template<typename FuncType, std::size_t FuncIndex, typename Ret, typename... Args>
		struct GLWrapper<FuncType, FuncIndex, Ret(GL_APIENTRYP)(Args...)>
		{
			static auto WrapErrorHandling()
			{
				return [](Args... args) -> Ret
				{
					const Context* context = s_currentGLContext; //< pay TLS cost once
					assert(context);

					FuncType funcPtr = reinterpret_cast<FuncType>(context->GetFunctionByIndex(FuncIndex));

					if constexpr (std::is_same_v<Ret, void>)
					{
						funcPtr(args...);

						if (context->ProcessErrorStack())
							context->PrintFunctionCall(FuncIndex, args...);
					}
					else
					{
						Ret r = funcPtr(args...);

						if (context->ProcessErrorStack())
							context->PrintFunctionCall(FuncIndex, args...);

						return r;
					}
				};
			}
		};
	}

	struct Context::BlitFramebuffers
	{
		GL::Framebuffer drawFBO;
		GL::Framebuffer readFBO;
	};

	struct Context::SymbolLoader
	{
		SymbolLoader(Context& parent) :
		context(parent)
		{
		}

		template<typename FuncType, std::size_t FuncIndex, typename Func>
		bool Load(Func& func, const char* funcName, bool mandatory, bool implementFallback = false)
		{
			const Loader& loader = context.GetLoader();
			GLFunction originalFuncPtr = loader.LoadFunction(funcName);

			func = reinterpret_cast<FuncType>(originalFuncPtr);

			if (func && wrapErrorHandling)
			{
				if constexpr (FuncIndex != UnderlyingCast(FunctionIndex::glGetError)) //< Prevent infinite recursion
				{
					using Wrapper = GLWrapper<FuncType, FuncIndex, FuncType>;
					func = Wrapper::WrapErrorHandling();
				}
			}

			if (!func)
			{
				if (!implementFallback || (!context.ImplementFallback(funcName) && !func)) //< double-check
				{
					if (mandatory)
						throw std::runtime_error("failed to load core function " + std::string(funcName));
				}
			}

			context.m_originalFunctionPointer[FuncIndex] = originalFuncPtr;

			return func != nullptr;
		}

		Context& context;
		bool wrapErrorHandling = false;
	};


	Context::Context(const OpenGLDevice* device) :
	m_vaoCache(*this),
	m_device(device),
	m_didCollectErrors(false),
	m_hadAnyError(false),
	m_hasZeroToOneDepth(false)
	{
	}

	Context::~Context()
	{
		if (m_device)
			m_device->NotifyContextDestruction(*this);
	}

	void Context::BindBuffer(BufferTarget target, GLuint buffer, bool force) const
	{
#ifdef NAZARA_PLATFORM_WEB
		force = true;
#endif

		if (m_state.bufferTargets[target] != buffer || force)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glBindBuffer(ToOpenGL(target), buffer);

			m_state.bufferTargets[target] = buffer;
		}
	}

	GLenum Context::BindFramebuffer(GLuint fbo) const
	{
		if (m_state.boundDrawFBO == fbo)
			return GL_DRAW_FRAMEBUFFER;
		else if (m_state.boundReadFBO == fbo)
			return GL_READ_FRAMEBUFFER;
		else
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			m_state.boundDrawFBO = fbo;
			m_state.boundReadFBO = fbo;
			return GL_FRAMEBUFFER;
		}
	}

	void Context::BindFramebuffer(FramebufferTarget target, GLuint fbo) const
	{
		auto& currentFbo = (target == FramebufferTarget::Draw) ? m_state.boundDrawFBO : m_state.boundReadFBO;
		if (currentFbo != fbo)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glBindFramebuffer((target == FramebufferTarget::Draw) ? GL_DRAW_FRAMEBUFFER : GL_READ_FRAMEBUFFER, fbo);
			currentFbo = fbo;
		}
	}

	void Context::BindImageTexture(GLuint imageUnit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) const
	{
		if (imageUnit >= m_state.imageUnits.size())
			throw std::runtime_error("unsupported image unit #" + std::to_string(imageUnit));

		layer = (layered == GL_TRUE) ? layer : 0;

		auto& unit = m_state.imageUnits[imageUnit];
		if (unit.texture != texture || unit.level != level || unit.layered != layered || unit.layer != layer || unit.access != access || unit.format != format)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			if (!glBindImageTexture)
				throw std::runtime_error("image binding is not supported");

			glBindImageTexture(imageUnit, texture, level, layered, layer, access, format);
			unit.access = access;
			unit.format = format;
			unit.layer = layer;
			unit.layered = layered;
			unit.level = level;
			unit.texture = texture;
		}
	}

	void Context::BindProgram(GLuint program) const
	{
		if (m_state.boundProgram != program)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glUseProgram(program);
			m_state.boundProgram = program;
		}
	}

	void Context::BindSampler(UInt32 textureUnit, GLuint sampler) const
	{
		if (textureUnit >= m_state.textureUnits.size())
			throw std::runtime_error("unsupported texture unit #" + std::to_string(textureUnit));

		auto& unit = m_state.textureUnits[textureUnit];
		if (unit.sampler != sampler)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glBindSampler(textureUnit, sampler);
			unit.sampler = sampler;
		}
	}

	void Context::BindStorageBuffer(UInt32 storageUnit, GLuint buffer, GLintptr offset, GLsizeiptr size) const
	{
		if (storageUnit >= m_state.storageUnits.size())
			throw std::runtime_error("unsupported storage buffer unit #" + std::to_string(storageUnit));

		auto& unit = m_state.storageUnits[storageUnit];
		if (unit.buffer != buffer || unit.offset != offset || unit.size != size)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, storageUnit, buffer, offset, size);

			unit.buffer = buffer;
			unit.offset = offset;
			unit.size = size;

			// glBindBufferRange does replace the currently bound buffer
			m_state.bufferTargets[BufferTarget::Storage] = buffer;
		}
	}

	void Context::BindTexture(TextureTarget target, GLuint texture) const
	{
		BindTexture(m_state.currentTextureUnit, target, texture);
	}

	void Context::BindTexture(UInt32 textureUnit, TextureTarget target, GLuint texture) const
	{
		if (textureUnit >= m_state.textureUnits.size())
			throw std::runtime_error("unsupported texture unit #" + std::to_string(textureUnit));

		auto& unit = m_state.textureUnits[textureUnit];
		if (unit.textureTargets[target] != texture)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			SetCurrentTextureUnit(textureUnit);

			glBindTexture(ToOpenGL(target), texture);

			unit.textureTargets[target] = texture;
		}
	}

	void Context::BindUniformBuffer(UInt32 uboUnit, GLuint buffer, GLintptr offset, GLsizeiptr size) const
	{
		if (uboUnit >= m_state.uboUnits.size())
			throw std::runtime_error("unsupported uniform buffer unit #" + std::to_string(uboUnit));

		auto& unit = m_state.uboUnits[uboUnit];
		if (unit.buffer != buffer || unit.offset != offset || unit.size != size)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glBindBufferRange(GL_UNIFORM_BUFFER, uboUnit, buffer, offset, size);

			unit.buffer = buffer;
			unit.offset = offset;
			unit.size = size;

			// glBindBufferRange does replace the currently bound buffer
			m_state.bufferTargets[BufferTarget::Uniform] = buffer;
		}
	}

	void Context::BindVertexArray(GLuint vertexArray, bool force) const
	{
		if (m_state.boundVertexArray != vertexArray || force)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glBindVertexArray(vertexArray);
			m_state.boundVertexArray = vertexArray;
		}
	}

	bool Context::BlitTexture(const OpenGLTexture& source, const OpenGLTexture& destination, const Boxui& srcBox, const Boxui& dstBox, SamplerFilter filter) const
	{
		if (!m_blitFramebuffers && !InitializeBlitFramebuffers())
			return false;

		// Bind framebuffers before configuring them (so they won't override each other)
		BindFramebuffer(FramebufferTarget::Draw, m_blitFramebuffers->drawFBO.GetObjectId());
		BindFramebuffer(FramebufferTarget::Read, m_blitFramebuffers->readFBO.GetObjectId());

		auto BindTexture = [](GL::Framebuffer& framebuffer, const OpenGLTexture& texture)
		{
			if (texture.RequiresTextureViewEmulation())
			{
				const TextureViewInfo& texViewInfo = texture.GetTextureViewInfo();
				if (texViewInfo.viewType != ImageType::E2D)
					throw std::runtime_error("unrestricted texture views can only be used as 2D texture attachment");

				const OpenGLTexture& parentTexture = *texture.GetParentTexture();

				switch (parentTexture.GetType())
				{
					case ImageType::Cubemap:
					{
						constexpr std::array<GLenum, 6> faceTargets = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
						assert(texViewInfo.baseArrayLayer < faceTargets.size());

						GLenum texTarget = faceTargets[texViewInfo.baseArrayLayer];
						framebuffer.Texture2D(GL_COLOR_ATTACHMENT0, texTarget, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel);
						break;
					}

					case ImageType::E1D:
					case ImageType::E2D:
						framebuffer.Texture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel);
						break;

					case ImageType::E1D_Array:
					case ImageType::E2D_Array:
					case ImageType::E3D:
						framebuffer.TextureLayer(GL_COLOR_ATTACHMENT0, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel, texViewInfo.baseArrayLayer);
						break;
				}
			}
			else
			{
				if (texture.GetTexture().GetTarget() != TextureTarget::Target2D)
					throw std::runtime_error("blit is not yet supported from/to other texture type than 2D textures");

				framebuffer.Texture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.GetTexture().GetObjectId(), 0);
			}
		};

		// Attach textures to color attachment
		BindTexture(m_blitFramebuffers->readFBO, source);
		BindTexture(m_blitFramebuffers->drawFBO, destination);

		// Validate framebuffer completeness
		if (GLenum checkResult = m_blitFramebuffers->drawFBO.Check(); checkResult != GL_FRAMEBUFFER_COMPLETE)
		{
			NazaraError("blit draw FBO is incomplete: {0}", TranslateOpenGLError(checkResult));
			return false;
		}

		if (GLenum checkResult = m_blitFramebuffers->readFBO.Check(); checkResult != GL_FRAMEBUFFER_COMPLETE)
		{
			NazaraError("blit read FBO is incomplete: {0}", TranslateOpenGLError(checkResult));
			return false;
		}

		glBlitFramebuffer(srcBox.x, srcBox.y, srcBox.x + srcBox.width, srcBox.y + srcBox.height, dstBox.x, dstBox.y, dstBox.x + dstBox.width, dstBox.y + srcBox.height, GL_COLOR_BUFFER_BIT, ToOpenGL(filter));
		return true;
	}

	bool Context::BlitTextureToWindow(const OpenGLTexture& texture, const Boxui& srcBox, const Boxui& dstBox, SamplerFilter filter) const
	{
		if (!m_blitFramebuffers && !InitializeBlitFramebuffers())
			return false;

		// Bind framebuffers before configuring them (so they won't override each other)
		BindFramebuffer(FramebufferTarget::Draw, 0);
		BindFramebuffer(FramebufferTarget::Read, m_blitFramebuffers->readFBO.GetObjectId());

		// Attach textures to color attachment
		BindTextureToFramebuffer(m_blitFramebuffers->readFBO, texture);

		// Validate framebuffer completeness
		if (GLenum checkResult = m_blitFramebuffers->readFBO.Check(); checkResult != GL_FRAMEBUFFER_COMPLETE)
		{
			NazaraError("blit read FBO is incomplete: {0}", TranslateOpenGLError(checkResult));
			return false;
		}

		glBlitFramebuffer(srcBox.x, srcBox.y, srcBox.x + srcBox.width, srcBox.y + srcBox.height, dstBox.x, dstBox.y + srcBox.height, dstBox.x + dstBox.width, dstBox.y, GL_COLOR_BUFFER_BIT, ToOpenGL(filter));
		return true;
	}

	bool Context::ClearErrorStack() const
	{
		assert(GetCurrentContext() == this);

		while (glGetError() != GL_NO_ERROR);

		m_didCollectErrors = false;
		m_hadAnyError = false;

		return true;
	}

	bool Context::CopyTexture(const OpenGLTexture& source, const OpenGLTexture& destination, const Boxui& srcBox, const Vector3ui& dstPos) const
	{
		// Use glCopyImageSubData if available
		// TODO: Emulate texture views (which aren't available on GL ES, even though glCopyImageSubData is)
		if (glCopyImageSubData && !source.RequiresTextureViewEmulation() && !destination.RequiresTextureViewEmulation())
		{
			GLuint srcImage = source.GetTexture().GetObjectId();
			GLenum srcTarget = ToOpenGL(source.GetTexture().GetTarget());

			GLuint dstImage = destination.GetTexture().GetObjectId();
			GLenum dstTarget = ToOpenGL(destination.GetTexture().GetTarget());

			glCopyImageSubData(srcImage, srcTarget, 0, GLint(srcBox.x), GLint(srcBox.y), GLint(srcBox.z), dstImage, dstTarget, 0, GLint(dstPos.x), GLint(dstPos.y), GLint(dstPos.z), GLsizei(srcBox.width), GLsizei(srcBox.height), GLsizei(srcBox.depth));

			return true;
		}
		else
		{
			// If glCopyImageSubData is not available, fallback to framebuffer blit
			return BlitTexture(source, destination, srcBox, Boxui(dstPos.x, dstPos.y, dstPos.z, srcBox.width, srcBox.height, srcBox.depth), SamplerFilter::Nearest);
		}
	}

	bool Context::Initialize(const ContextParams& params)
	{
		if (!SetCurrentContext(this))
		{
			NazaraError("failed to activate context");
			return false;
		}

		SymbolLoader loader(*this);
		loader.wrapErrorHandling = params.wrapErrorHandling;

		try
		{
#define NAZARA_OPENGLRENDERER_FUNCTION(name, sig) loader.Load<sig, UnderlyingCast(FunctionIndex:: name)>(name, #name, true, true);
#define NAZARA_OPENGLRENDERER_GL_FUNCTION(ver, name, sign)
#define NAZARA_OPENGLRENDERER_GL_GLES_FUNCTION(glVer, glesVer, name, sign)

#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>
		}
		catch (const std::exception& e)
		{
			NazaraError("{}", e.what());
			return false;
		}

		m_params.glMajorVersion = GetInteger<unsigned int>(GL_MAJOR_VERSION);
		m_params.glMinorVersion = GetInteger<unsigned int>(GL_MINOR_VERSION);

		unsigned int glVersion = m_params.glMajorVersion * 100 + m_params.glMinorVersion * 10;

		// Load extensions
		GLint extensionCount = GetInteger<GLint>(GL_NUM_EXTENSIONS);
		for (GLint i = 0; i < extensionCount; ++i)
			m_supportedExtensions.emplace(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));

		m_extensionStatus.fill(ExtensionStatus::NotSupported);

		// Clip control
		if (m_params.type == ContextType::OpenGL && glVersion >= 450)
			m_extensionStatus[Extension::ClipControl] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_clip_control"))
			m_extensionStatus[Extension::ClipControl] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_EXT_clip_control"))
			m_extensionStatus[Extension::ClipControl] = ExtensionStatus::EXT;

		// Compute shaders
		if ((m_params.type == ContextType::OpenGL && glVersion >= 430) || (m_params.type == ContextType::OpenGL_ES && glVersion >= 310))
			m_extensionStatus[Extension::ComputeShader] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_compute_shader"))
			m_extensionStatus[Extension::ComputeShader] = ExtensionStatus::ARB;

		// Debug output
		if ((m_params.type == ContextType::OpenGL && glVersion >= 430) || (m_params.type == ContextType::OpenGL_ES && glVersion >= 320))
			m_extensionStatus[Extension::DebugOutput] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_KHR_debug"))
			m_extensionStatus[Extension::DebugOutput] = ExtensionStatus::KHR;
		else if (m_supportedExtensions.count("GL_ARB_debug_output"))
			m_extensionStatus[Extension::DebugOutput] = ExtensionStatus::ARB;

		// Depth clamp
		if (m_params.type == ContextType::OpenGL && glVersion >= 320)
			m_extensionStatus[Extension::DepthClamp] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_depth_clamp"))
			m_extensionStatus[Extension::DepthClamp] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_EXT_depth_clamp"))
			m_extensionStatus[Extension::DepthClamp] = ExtensionStatus::EXT;
		else if (m_supportedExtensions.count("GL_NV_depth_clamp"))
			m_extensionStatus[Extension::DepthClamp] = ExtensionStatus::Vendor;

		// Polygon mode
		if (m_params.type == ContextType::OpenGL)
			m_extensionStatus[Extension::PolygonMode] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_NV_polygon_mode"))
			m_extensionStatus[Extension::DepthClamp] = ExtensionStatus::Vendor;

		// Shader image load formatted
		if (m_supportedExtensions.count("GL_EXT_shader_image_load_formatted"))
			m_extensionStatus[Extension::ShaderImageLoadFormatted] = ExtensionStatus::EXT;

		// Shader image load/store
		if ((m_params.type == ContextType::OpenGL && glVersion >= 420) || (m_params.type == ContextType::OpenGL_ES && glVersion >= 310))
			m_extensionStatus[Extension::ShaderImageLoadStore] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_shader_image_load_store"))
			m_extensionStatus[Extension::ShaderImageLoadStore] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_EXT_shader_image_load_store"))
			m_extensionStatus[Extension::ShaderImageLoadStore] = ExtensionStatus::EXT;

		// SPIR-V support
		if (m_params.type == ContextType::OpenGL && glVersion >= 460)
			m_extensionStatus[Extension::SpirV] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_gl_spirv"))
			m_extensionStatus[Extension::SpirV] = ExtensionStatus::ARB;

		// Storage buffers (SSBO)
		if ((m_params.type == ContextType::OpenGL && glVersion >= 430) || (m_params.type == ContextType::OpenGL_ES && glVersion >= 310))
			m_extensionStatus[Extension::StorageBuffers] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_shader_storage_buffer_object"))
			m_extensionStatus[Extension::StorageBuffers] = ExtensionStatus::ARB;

		// Texture compression (S3tc)
		if (m_supportedExtensions.count("GL_EXT_texture_compression_s3tc"))
			m_extensionStatus[Extension::TextureCompressionS3tc] = ExtensionStatus::EXT;

		// Texture anisotropic filter
		if (m_params.type == ContextType::OpenGL && glVersion >= 460)
			m_extensionStatus[Extension::TextureFilterAnisotropic] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_texture_filter_anisotropic"))
			m_extensionStatus[Extension::TextureFilterAnisotropic] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_EXT_texture_filter_anisotropic"))
			m_extensionStatus[Extension::TextureFilterAnisotropic] = ExtensionStatus::EXT;

		// Texture view
		if (m_params.type == ContextType::OpenGL && glVersion >= 430)
			m_extensionStatus[Extension::TextureView] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_texture_view"))
			m_extensionStatus[Extension::TextureView] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_OES_texture_view"))
			m_extensionStatus[Extension::TextureView] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_EXT_texture_view"))
			m_extensionStatus[Extension::TextureView] = ExtensionStatus::EXT;

#define NAZARA_OPENGLRENDERER_FUNCTION(name, sig)
#define NAZARA_OPENGLRENDERER_GL_FUNCTION(ver, name, sig) \
		if (m_params.type == ContextType::OpenGL && glVersion >= ver) \
			loader.Load<sig, UnderlyingCast(FunctionIndex:: name)>(name, #name, false, true); \
		else \
			ImplementFallback(#name);

#define NAZARA_OPENGLRENDERER_GL_GLES_FUNCTION(glVer, glesVer, name, sig) \
		if ((m_params.type == ContextType::OpenGL && glVersion >= glVer) || (m_params.type == ContextType::OpenGL_ES && glVersion >= glesVer)) \
			loader.Load<sig, UnderlyingCast(FunctionIndex:: name)>(name, #name, false, true); \
		else \
			ImplementFallback(#name);

#include <Nazara/OpenGLRenderer/Wrapper/CoreFunctions.hpp>

		// Match Vulkan convention if supported (so we don't have to inject code in the shader to fix it)
		if (glClipControl)
		{
			// TODO: Using GL_UPPER_LEFT causes some issues I still have to figure out
			glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
			m_hasZeroToOneDepth = true;
		}

		// Always enable cubemap sampling (as it's the guaranteed behavior on Vulkan and OpenGL ES 3.0)
		if (m_params.type == ContextType::OpenGL && (glVersion >= 320 || m_supportedExtensions.count("GL_ARB_seamless_cube_map")))
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		// If we requested an OpenGL ES context but couldn't create one, check for some compatibility extensions
		if (params.type == ContextType::OpenGL_ES && m_params.type != params.type)
		{
			if (m_supportedExtensions.count("GL_ARB_ES3_2_compatibility"))
			{
				m_params.type = ContextType::OpenGL_ES;
				m_params.glMajorVersion = 3;
				m_params.glMinorVersion = 2;
			}
			else if (m_supportedExtensions.count("GL_ARB_ES3_1_compatibility"))
			{
				m_params.type = ContextType::OpenGL_ES;
				m_params.glMajorVersion = 3;
				m_params.glMinorVersion = 1;
			}
			else if (m_supportedExtensions.count("GL_ARB_ES3_compatibility"))
			{
				m_params.type = ContextType::OpenGL_ES;
				m_params.glMajorVersion = 3;
				m_params.glMinorVersion = 0;
			}
			else
				NazaraWarning("desktop support for OpenGL ES is missing, falling back to OpenGL...");
		}

		// Set debug callback (if supported and enabled)
		if (glDebugMessageCallback && params.validationLevel != RenderAPIValidationLevel::None)
		{
			m_params.validationLevel = params.validationLevel;

			glEnable(GL_DEBUG_OUTPUT);

			// Always enable synchronous debug output for debug libraries
#ifndef NAZARA_DEBUG
			if (m_params.validationLevel == RenderAPIValidationLevel::Debug)
#endif
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
			{
				const Context* context = static_cast<const Context*>(userParam);
				context->HandleDebugMessage(source, type, id, severity, length, message);
			}, this);

			if (glDebugMessageControl)
			{
				// Disable push/pop debug groups and markers notifications
				if (glPushDebugGroup)
					glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

				if (glPopDebugGroup)
					glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_POP_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

				if (glDebugMessageInsert)
					glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_MARKER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

				// Handle verbosity level
				if (m_params.validationLevel < RenderAPIValidationLevel::Debug)
					// Disable driver notifications except in debug (NVidia driver is very verbose)
					glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
				else if (m_params.validationLevel < RenderAPIValidationLevel::Verbose)
					glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
				else if (m_params.validationLevel < RenderAPIValidationLevel::Warnings)
					glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
			}
		}
		else
			m_params.validationLevel = m_params.validationLevel;

		unsigned int maxTextureUnits = GetInteger<unsigned int>(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
		if (maxTextureUnits < 32) //< OpenGL ES 3.0 requires at least 32 textures units
			NazaraWarning("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS is {0}, expected >= 32", maxTextureUnits);

		assert(maxTextureUnits > 0);
		m_state.textureUnits.resize(maxTextureUnits);

		unsigned int maxUniformBufferUnits = GetInteger<unsigned int>(GL_MAX_UNIFORM_BUFFER_BINDINGS);
		if (maxUniformBufferUnits < 24) //< OpenGL ES 3.0 requires at least 24 uniform buffers units
			NazaraWarning("GL_MAX_UNIFORM_BUFFER_BINDINGS is {0}, expected >= 24", maxUniformBufferUnits);

		assert(maxUniformBufferUnits > 0);
		m_state.uboUnits.resize(maxUniformBufferUnits);

		if (IsExtensionSupported(Extension::ShaderImageLoadStore))
		{
			unsigned int maxImageUnits = GetInteger<unsigned int>(GL_MAX_IMAGE_UNITS);
			assert(maxImageUnits > 0);
			m_state.imageUnits.resize(maxImageUnits);
		}

		if (IsExtensionSupported(Extension::StorageBuffers))
		{
			unsigned int maxStorageBufferUnits = GetInteger<unsigned int>(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
			if (maxStorageBufferUnits < 8) //< OpenGL ES 3.1 requires at least 8 storage buffers units
				NazaraWarning("GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS is {0}, expected >= 8", maxUniformBufferUnits);

			assert(maxStorageBufferUnits > 0);
			m_state.storageUnits.resize(maxStorageBufferUnits);
		}

		std::array<GLint, 4> res;

		glGetIntegerv(GL_SCISSOR_BOX, res.data());
		m_state.scissorBox = { res[0], res[1], res[2], res[3] };

		glGetIntegerv(GL_VIEWPORT, res.data());
		m_state.viewport = { res[0], res[1], res[2], res[3] };

		// Set default OpenGL states
		m_state.renderStates.depthBuffer = false;
		m_state.renderStates.depthCompare = RendererComparison::Less;
		m_state.renderStates.faceCulling = FaceCulling::None;
		m_state.renderStates.frontFace = FrontFace::CounterClockwise;
		m_state.renderStates.scissorTest = false;
		m_state.renderStates.stencilTest = false;

		return true;
	}

	template<typename... Args>
	void Context::PrintFunctionCall(std::size_t funcIndex, Args... args) const
	{
		std::stringstream ss;
		ss << s_functionNames[funcIndex] << "(";
		if constexpr (sizeof...(args) > 0)
		{
			bool first = true;
			auto PrintParam = [&](auto value)
			{
				if (!first)
					ss << ", ";

				ss << +value;
				first = false;
			};

			(PrintParam(args), ...);
		}
		ss << ")";
		NazaraDebug(ss.str());
	}

	bool Context::ProcessErrorStack() const
	{
		assert(GetCurrentContext() == this);

		bool hasAnyError = false;

		GLuint lastError;
		while ((lastError = glGetError()) != GL_NO_ERROR)
		{
			hasAnyError = true;

			NazaraError("OpenGL error: {0}", TranslateOpenGLError(lastError));
		}

		m_didCollectErrors = true;
		m_hadAnyError = hasAnyError;

		return hasAnyError;
	}

	void Context::SetCurrentTextureUnit(UInt32 textureUnit) const
	{
		if (m_state.currentTextureUnit != textureUnit)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glActiveTexture(GL_TEXTURE0 + textureUnit);
			m_state.currentTextureUnit = textureUnit;
		}
	}

	void Context::SetScissorBox(GLint x, GLint y, GLsizei width, GLsizei height) const
	{
		if (m_state.scissorBox.x != x ||
		    m_state.scissorBox.y != y ||
		    m_state.scissorBox.width != width ||
		    m_state.scissorBox.height != height)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glScissor(x, y, width, height);

			m_state.scissorBox.x = x;
			m_state.scissorBox.y = y;
			m_state.scissorBox.width = width;
			m_state.scissorBox.height = height;
		}
	}

	void Context::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) const
	{
		if (m_state.viewport.x != x ||
		    m_state.viewport.y != y ||
		    m_state.viewport.width != width ||
		    m_state.viewport.height != height)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glViewport(x, y, width, height);

			m_state.viewport.x = x;
			m_state.viewport.y = y;
			m_state.viewport.width = width;
			m_state.viewport.height = height;
		}
	}

	void Context::UpdateStates(const RenderStates& renderStates, bool isViewportFlipped) const
	{
		if (!SetCurrentContext(this))
			throw std::runtime_error("failed to activate context");

		// Depth bias
		if (renderStates.depthBias)
		{
			if (!NumberEquals(m_state.renderStates.depthBiasConstantFactor, renderStates.depthBiasConstantFactor) ||
			    !NumberEquals(m_state.renderStates.depthBiasSlopeFactor, renderStates.depthBiasSlopeFactor))
			{
				glPolygonOffset(renderStates.depthBiasSlopeFactor, renderStates.depthBiasConstantFactor);
				m_state.renderStates.depthBiasConstantFactor = renderStates.depthBiasConstantFactor;
				m_state.renderStates.depthBiasSlopeFactor = renderStates.depthBiasSlopeFactor;
			}
		}

		// Depth compare and depth write
		if (renderStates.depthBuffer)
		{
			if (m_state.renderStates.depthCompare != renderStates.depthCompare)
			{
				glDepthFunc(ToOpenGL(renderStates.depthCompare));
				m_state.renderStates.depthCompare = renderStates.depthCompare;
			}

			if (m_state.renderStates.depthWrite != renderStates.depthWrite)
			{
				glDepthMask((renderStates.depthWrite) ? GL_TRUE : GL_FALSE);
				m_state.renderStates.depthWrite = renderStates.depthWrite;
			}
		}

		// Face culling
		if (m_state.renderStates.faceCulling != renderStates.faceCulling)
		{
			bool wasEnabled = (m_state.renderStates.faceCulling != FaceCulling::None);
			bool isEnabled = (renderStates.faceCulling != FaceCulling::None);

			if (isEnabled)
			{
				if (!wasEnabled)
					glEnable(GL_CULL_FACE);

				glCullFace(ToOpenGL(renderStates.faceCulling));
			}
			else if (wasEnabled)
				glDisable(GL_CULL_FACE);

			m_state.renderStates.faceCulling = renderStates.faceCulling;
		}

		// Front face
		FrontFace targetFrontFace = renderStates.frontFace;
		if (!isViewportFlipped)
			targetFrontFace = (targetFrontFace == FrontFace::Clockwise) ? FrontFace::CounterClockwise : FrontFace::Clockwise;

		if (m_state.renderStates.frontFace != targetFrontFace)
		{
			glFrontFace(ToOpenGL(targetFrontFace));
			m_state.renderStates.frontFace = targetFrontFace;
		}

		// Face filling
		if (m_state.renderStates.faceFilling != renderStates.faceFilling)
		{
			assert(glPolygonMode);

			glPolygonMode(GL_FRONT_AND_BACK, ToOpenGL(renderStates.faceFilling));
			m_state.renderStates.faceFilling = renderStates.faceFilling;
		}

		// Front and back stencil states
		if (renderStates.stencilTest)
		{
			auto ApplyStencilStates = [&](bool front)
			{
				auto& currentStencilData = (front) ? m_state.renderStates.stencilFront : m_state.renderStates.stencilBack;
				auto& newStencilData = (front) ? renderStates.stencilFront : renderStates.stencilBack;

				if (currentStencilData.compare != newStencilData.compare ||
				    currentStencilData.reference != newStencilData.reference ||
				    currentStencilData.compareMask != newStencilData.compareMask)
				{
					glStencilFuncSeparate((front) ? GL_FRONT : GL_BACK, ToOpenGL(newStencilData.compare), newStencilData.reference, newStencilData.compareMask);
					currentStencilData.compare = newStencilData.compare;
					currentStencilData.compareMask = newStencilData.compareMask;
					currentStencilData.reference = newStencilData.reference;
				}

				if (currentStencilData.depthFail != newStencilData.depthFail ||
				    currentStencilData.fail != newStencilData.fail ||
				    currentStencilData.pass != newStencilData.pass)
				{
					glStencilOpSeparate((front) ? GL_FRONT : GL_BACK, ToOpenGL(newStencilData.fail), ToOpenGL(newStencilData.depthFail), ToOpenGL(newStencilData.pass));
					currentStencilData.depthFail = newStencilData.depthFail;
					currentStencilData.fail = newStencilData.fail;
					currentStencilData.pass = newStencilData.pass;
				}

				if (currentStencilData.writeMask != newStencilData.writeMask)
				{
					glStencilMaskSeparate((front) ? GL_FRONT : GL_BACK, newStencilData.writeMask);
					currentStencilData.writeMask = newStencilData.writeMask;
				}
			};

			ApplyStencilStates(true);
			ApplyStencilStates(false);
		}

		// Line width
		if (!NumberEquals(m_state.renderStates.lineWidth, renderStates.lineWidth, 0.001f))
		{
			glLineWidth(renderStates.lineWidth);
			m_state.renderStates.lineWidth = renderStates.lineWidth;
		}

		// Point size (TODO)
		/*if (!NumberEquals(m_state.renderStates.pointSize, renderStates.pointSize, 0.001f))
		{
			glPointSize(renderStates.pointSize);
			m_state.renderStates.pointSize = renderStates.pointSize;
		}*/

		// Blend states
		if (m_state.renderStates.blending != renderStates.blending)
		{
			if (renderStates.blending)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);

			m_state.renderStates.blending = renderStates.blending;
		}

		if (renderStates.blending)
		{
			auto& currentBlend = m_state.renderStates.blend;
			const auto& targetBlend = renderStates.blend;

			if (currentBlend.modeColor != targetBlend.modeColor || currentBlend.modeAlpha != targetBlend.modeAlpha)
			{
				glBlendEquationSeparate(ToOpenGL(targetBlend.modeColor), ToOpenGL(targetBlend.modeAlpha));
				currentBlend.modeAlpha = targetBlend.modeAlpha;
				currentBlend.modeColor = targetBlend.modeColor;
			}

			if (currentBlend.dstAlpha != targetBlend.dstAlpha || currentBlend.dstColor != targetBlend.dstColor ||
				currentBlend.srcAlpha != targetBlend.srcAlpha || currentBlend.srcColor != targetBlend.srcColor)
			{
				glBlendFuncSeparate(ToOpenGL(targetBlend.srcColor), ToOpenGL(targetBlend.dstColor), ToOpenGL(targetBlend.srcAlpha), ToOpenGL(targetBlend.dstAlpha));
				currentBlend.dstAlpha = targetBlend.dstAlpha;
				currentBlend.dstColor = targetBlend.dstColor;
				currentBlend.srcAlpha = targetBlend.srcAlpha;
				currentBlend.srcColor = targetBlend.srcColor;
			}
		}

		// Color write
		if (m_state.renderStates.colorWriteMask != renderStates.colorWriteMask)
		{
			glColorMask(renderStates.colorWriteMask.Test(ColorComponent::Red), renderStates.colorWriteMask.Test(ColorComponent::Green), renderStates.colorWriteMask.Test(ColorComponent::Blue), renderStates.colorWriteMask.Test(ColorComponent::Alpha));
			m_state.renderStates.colorWriteMask = renderStates.colorWriteMask;
		}

		// Depth bias
		if (m_state.renderStates.depthBias != renderStates.depthBias)
		{
			// TODO: Handle line and points
			if (renderStates.depthBias)
				glEnable(GL_POLYGON_OFFSET_FILL);
			else
				glDisable(GL_POLYGON_OFFSET_FILL);

			m_state.renderStates.depthBias = renderStates.depthBias;
		}

		// Depth buffer
		if (m_state.renderStates.depthBuffer != renderStates.depthBuffer)
		{
			if (renderStates.depthBuffer)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			m_state.renderStates.depthBuffer = renderStates.depthBuffer;
		}

		// Depth clamp
		if (m_state.renderStates.depthClamp != renderStates.depthClamp)
		{
			assert(IsExtensionSupported(Extension::DepthClamp));

			if (renderStates.depthClamp)
				glEnable(GL_DEPTH_CLAMP);
			else
				glDisable(GL_DEPTH_CLAMP);

			m_state.renderStates.depthClamp = renderStates.depthClamp;
		}

		// Scissor test
		if (m_state.renderStates.scissorTest != renderStates.scissorTest)
		{
			if (renderStates.scissorTest)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);

			m_state.renderStates.scissorTest = renderStates.scissorTest;
		}

		// Stencil test
		if (m_state.renderStates.stencilTest != renderStates.stencilTest)
		{
			if (renderStates.stencilTest)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);

			m_state.renderStates.stencilTest = renderStates.stencilTest;
		}
	}

	const Context* Context::GetCurrentContext()
	{
		return s_currentGLContext;
	}

	bool Context::SetCurrentContext(const Context* context)
	{
		const Context*& currentContext = s_currentGLContext; //< Pay TLS cost once
		if (currentContext == context)
			return true;

		if (currentContext)
		{
			currentContext->Desactivate();
			currentContext = nullptr;
		}

		if (context)
		{
			if (!context->Activate())
				return false;

			currentContext = context;
		}

		return true;
	}

	void Context::OnContextRelease()
	{
		OnContextDestruction(this);

		m_blitFramebuffers.reset();
		m_vaoCache.Clear();
	}

	bool Context::ImplementFallback(std::string_view function)
	{
		SymbolLoader loader(*this);

		if (function == "glClipControl")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glClipControl);

			return IsExtensionSupported("GL_EXT_clip_control") && loader.Load<PFNGLCLIPCONTROLEXTPROC, functionIndex>(glClipControl, "glClipControlEXT", false);
		}
		else if (function == "glDebugMessageCallback")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glDebugMessageCallback);

			if (IsExtensionSupported("GL_KHR_debug"))
			{
				if (loader.Load<PFNGLDEBUGMESSAGECALLBACKKHRPROC, functionIndex>(glDebugMessageCallback, "glDebugMessageCallbackKHR", false))
					return true;
			}

			if (m_params.type == ContextType::OpenGL && IsExtensionSupported("GL_ARB_debug_output"))
			{
				if (loader.Load<PFNGLDEBUGMESSAGECALLBACKKHRPROC, functionIndex>(glDebugMessageCallback, "glDebugMessageCallbackARB", false))
					return true;
			}
		}
		else if (function == "glDrawElementsInstancedBaseVertex")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glDrawElementsInstancedBaseVertex);

			if (m_params.type == ContextType::OpenGL_ES)
			{
				if (IsExtensionSupported("GL_OES_draw_elements_base_vertex"))
					return loader.Load<PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC, functionIndex>(glDrawElementsInstancedBaseVertex, "glDrawElementsInstancedBaseVertexOES", false);

				if (IsExtensionSupported("GL_EXT_draw_elements_base_vertex"))
					return loader.Load<PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC, functionIndex>(glDrawElementsInstancedBaseVertex, "glDrawElementsInstancedBaseVertexEXT", false);
			}
		}
		else if (function == "glPolygonMode")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glPolygonMode);

			return IsExtensionSupported("GL_NV_polygon_mode") && loader.Load<PFNGLPOLYGONMODENVPROC, functionIndex>(glPolygonMode, "glPolygonModeNV", false);
		}
		else if (function == "glSpecializeShader")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glSpecializeShader);

			if (m_params.type == ContextType::OpenGL && IsExtensionSupported("GL_ARB_spirv_extensions"))
				return loader.Load<PFNGLSPECIALIZESHADERPROC, functionIndex>(glSpecializeShader, "glSpecializeShaderARB", false);
		}
		else if (function == "glTextureView")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glTextureView);

			if (m_params.type == ContextType::OpenGL && IsExtensionSupported("GL_ARB_texture_view"))
			{
				if (loader.Load<PFNGLTEXTUREVIEWPROC, functionIndex>(glTextureView, "glTextureView", false))
					return true;
			}

			if (IsExtensionSupported("GL_EXT_texture_view"))
			{
				if (loader.Load<PFNGLTEXTUREVIEWPROC, functionIndex>(glTextureView, "glTextureViewEXT", false))
					return true;
			}

			if (IsExtensionSupported("GL_OES_texture_view"))
			{
				if (loader.Load<PFNGLTEXTUREVIEWPROC, functionIndex>(glTextureView, "glTextureViewOES", false))
					return true;
			}
		}
		else if (function == "glVertexAttribLPointer")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glVertexAttribLPointer);

			if (m_params.type == ContextType::OpenGL && IsExtensionSupported("GL_EXT_vertex_attrib_64bit"))
				return loader.Load<PFNGLVERTEXATTRIBLPOINTERPROC, functionIndex>(glVertexAttribLPointer, "glVertexAttribLPointerEXT", false);
		}

		return false;
	}

	void Context::NotifyContextDestruction(Context* context)
	{
		const Context*& currentContext = s_currentGLContext; //< Pay TLS cost only once
		if (currentContext == context)
			currentContext = nullptr;
	}

	void Context::HandleDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const
	{
		auto SourceStr = [](GLenum source)
		{
			using namespace std::literals;

			switch (source)
			{
				case GL_DEBUG_SOURCE_API:             return "OpenGL API"sv;
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Operating system"sv;
				case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader compiler"sv;
				case GL_DEBUG_SOURCE_THIRD_PARTY:     return "Third party"sv;
				case GL_DEBUG_SOURCE_APPLICATION:     return "Application"sv;
				case GL_DEBUG_SOURCE_OTHER:           return "Other"sv;
				default:
					// Extension type
					break;
			}

			return "Unknown"sv;
		};

		auto TypeStr = [](GLenum type)
		{
			using namespace std::literals;

			switch (type)
			{
				case GL_DEBUG_TYPE_ERROR:               return "Error"sv;
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated behavior"sv;
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined behavior"sv;
				case GL_DEBUG_TYPE_PORTABILITY:         return "Portability"sv;
				case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance"sv;
				case GL_DEBUG_TYPE_OTHER:               return "Other"sv;
				default:
					// Extension type
					break;
			}

			return "Unknown"sv;
		};

		auto SeverityStr = [](GLenum severity)
		{
			using namespace std::literals;

			switch (severity)
			{
				case GL_DEBUG_SEVERITY_HIGH:         return "High"sv;
				case GL_DEBUG_SEVERITY_MEDIUM:       return "Medium"sv;
				case GL_DEBUG_SEVERITY_LOW:          return "Low"sv;
				case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification"sv;
				default:
					// Extension severity
					break;
			}

			return "Unknown"sv;
		};

		NazaraNotice(
R"(OpenGL debug message (ID: {0:#x})
- Context: {1}
- Source: {2}
- Type: {3}
- Severity: {4}
- Message: {5}
)", id, fmt::ptr(this), SourceStr(source), TypeStr(type), SeverityStr(severity), std::string_view(message, length));
	}

	bool Context::InitializeBlitFramebuffers() const
	{
		m_blitFramebuffers = std::make_unique<BlitFramebuffers>();
		if (!m_blitFramebuffers->drawFBO.Create(*this))
		{
			NazaraError("failed to initialize draw FBO");
			return false;
		}

		if (!m_blitFramebuffers->readFBO.Create(*this))
		{
			NazaraError("failed to initialize read FBO");
			return false;
		}

		return true;
	}

	void Context::BindTextureToFramebuffer(Framebuffer& framebuffer, const OpenGLTexture& texture)
	{
		if (texture.RequiresTextureViewEmulation())
		{
			const TextureViewInfo& texViewInfo = texture.GetTextureViewInfo();
			if (texViewInfo.viewType != ImageType::E2D)
				throw std::runtime_error("unrestricted texture views can only be used as 2D texture attachment");

			const OpenGLTexture& parentTexture = *texture.GetParentTexture();

			switch (parentTexture.GetType())
			{
				case ImageType::Cubemap:
				{
					constexpr std::array<GLenum, 6> faceTargets = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
					assert(texViewInfo.baseArrayLayer < faceTargets.size());

					GLenum texTarget = faceTargets[texViewInfo.baseArrayLayer];
					framebuffer.Texture2D(GL_COLOR_ATTACHMENT0, texTarget, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel);
					break;
				}

				case ImageType::E1D:
				case ImageType::E2D:
					framebuffer.Texture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel);
					break;

				case ImageType::E1D_Array:
				case ImageType::E2D_Array:
				case ImageType::E3D:
					framebuffer.TextureLayer(GL_COLOR_ATTACHMENT0, parentTexture.GetTexture().GetObjectId(), texViewInfo.baseMipLevel, texViewInfo.baseArrayLayer);
					break;
			}
		}
		else
		{
			if (texture.GetTexture().GetTarget() != TextureTarget::Target2D)
				throw std::runtime_error("blit is not yet supported from/to other texture type than 2D textures");

			framebuffer.Texture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.GetTexture().GetObjectId(), 0);
		}
	}
}
