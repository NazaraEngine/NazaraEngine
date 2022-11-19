// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Framebuffer.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	thread_local const Context* s_currentGLContext = nullptr;

	namespace
	{
		constexpr std::array s_functionNames = {
#define NAZARA_OPENGLRENDERER_FUNC(name, sig) #name,
				NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_FUNC)
#undef NAZARA_OPENGLRENDERER_FUNC
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
						funcPtr(std::forward<Args>(args)...);

						if (context->ProcessErrorStack())
							context->PrintFunctionCall(FuncIndex, std::forward<Args>(args)...);
					}
					else
					{
						Ret r = funcPtr(std::forward<Args>(args)...);

						if (context->ProcessErrorStack())
							context->PrintFunctionCall(FuncIndex, std::forward<Args>(args)...);

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
				if (std::strcmp(funcName, "glGetError") != 0) //< Prevent infinite recursion
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
		if (m_state.bufferTargets[UnderlyingCast(target)] != buffer || force)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glBindBuffer(ToOpenGL(target), buffer);

			m_state.bufferTargets[UnderlyingCast(target)] = buffer;
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
			m_state.bufferTargets[UnderlyingCast(BufferTarget::Storage)] = buffer;
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
		if (unit.textureTargets[UnderlyingCast(target)] != texture)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			SetCurrentTextureUnit(textureUnit);

			glBindTexture(ToOpenGL(target), texture);

			unit.textureTargets[UnderlyingCast(target)] = texture;
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
			m_state.bufferTargets[UnderlyingCast(BufferTarget::Uniform)] = buffer;
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

	bool Context::BlitTexture(const Texture& source, const Texture& destination, const Boxui& srcBox, const Boxui& dstBox, SamplerFilter filter) const
	{
		if (!m_blitFramebuffers && !InitializeBlitFramebuffers())
			return false;

		//TODO: handle other textures types
		assert(source.GetTarget() == TextureTarget::Target2D);
		assert(destination.GetTarget() == TextureTarget::Target2D);

		// Bind framebuffers before configuring them (so they won't override each other)
		BindFramebuffer(FramebufferTarget::Draw, m_blitFramebuffers->drawFBO.GetObjectId());
		BindFramebuffer(FramebufferTarget::Read, m_blitFramebuffers->readFBO.GetObjectId());

		// Attach textures to color attachment
		m_blitFramebuffers->drawFBO.Texture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destination.GetObjectId());
		if (GLenum checkResult = m_blitFramebuffers->drawFBO.Check(); checkResult != GL_FRAMEBUFFER_COMPLETE)
		{
			NazaraError("Blit draw FBO is incomplete: " + TranslateOpenGLError(checkResult));
			return false;
		}

		m_blitFramebuffers->readFBO.Texture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, source.GetObjectId());
		if (GLenum checkResult = m_blitFramebuffers->readFBO.Check(); checkResult != GL_FRAMEBUFFER_COMPLETE)
		{
			NazaraError("Blit read FBO is incomplete: " + TranslateOpenGLError(checkResult));
			return false;
		}

		glBlitFramebuffer(srcBox.x, srcBox.y, srcBox.x + srcBox.width, srcBox.y + srcBox.height, dstBox.x, dstBox.y, dstBox.x + dstBox.width, dstBox.y + srcBox.height, GL_COLOR_BUFFER_BIT, ToOpenGL(filter));
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

	bool Context::CopyTexture(const Texture& source, const Texture& destination, const Boxui& srcBox, const Vector3ui& dstPos) const
	{
		// Use glCopyImageSubData if available
		if (glCopyImageSubData)
		{
			GLuint srcImage = source.GetObjectId();
			GLenum srcTarget = ToOpenGL(source.GetTarget());

			GLuint dstImage = destination.GetObjectId();
			GLenum dstTarget = ToOpenGL(destination.GetTarget());

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
#define NAZARA_OPENGLRENDERER_FUNC(name, sig) loader.Load<sig, UnderlyingCast(FunctionIndex:: name)>(name, #name, true, true);
#define NAZARA_OPENGLRENDERER_EXT_FUNC(name, sig) //< Do nothing
			NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_EXT_FUNC)
#undef NAZARA_OPENGLRENDERER_EXT_FUNC
#undef NAZARA_OPENGLRENDERER_FUNC
		}
		catch (const std::exception& e)
		{
			NazaraError(e.what());
			return false;
		}

		GLint majorVersion = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);

		GLint minorVersion = 0;
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

		m_params.glMajorVersion = majorVersion;
		m_params.glMinorVersion = minorVersion;

		unsigned int glVersion = majorVersion * 100 + minorVersion * 10;

		// Load extensions
		GLint extensionCount = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

		for (GLint i = 0; i < extensionCount; ++i)
			m_supportedExtensions.emplace(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));

		m_extensionStatus.fill(ExtensionStatus::NotSupported);

		// Clip control
		if (m_params.type == ContextType::OpenGL && glVersion >= 450)
			m_extensionStatus[UnderlyingCast(Extension::ClipControl)] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_clip_control"))
			m_extensionStatus[UnderlyingCast(Extension::ClipControl)] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_EXT_clip_control"))
			m_extensionStatus[UnderlyingCast(Extension::ClipControl)] = ExtensionStatus::EXT;

		// Debug output
		if ((m_params.type == ContextType::OpenGL && glVersion >= 430) || (m_params.type == ContextType::OpenGL_ES && glVersion >= 320))
			m_extensionStatus[UnderlyingCast(Extension::DebugOutput)] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_KHR_debug"))
			m_extensionStatus[UnderlyingCast(Extension::DebugOutput)] = ExtensionStatus::KHR;
		else if (m_supportedExtensions.count("GL_ARB_debug_output"))
			m_extensionStatus[UnderlyingCast(Extension::DebugOutput)] = ExtensionStatus::ARB;

		// Depth clamp
		if (m_params.type == ContextType::OpenGL && glVersion >= 320)
			m_extensionStatus[UnderlyingCast(Extension::DepthClamp)] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_depth_clamp"))
			m_extensionStatus[UnderlyingCast(Extension::DepthClamp)] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_EXT_depth_clamp"))
			m_extensionStatus[UnderlyingCast(Extension::DepthClamp)] = ExtensionStatus::EXT;
		else if (m_supportedExtensions.count("GL_NV_depth_clamp"))
			m_extensionStatus[UnderlyingCast(Extension::DepthClamp)] = ExtensionStatus::Vendor;

		// Polygon mode
		if (m_params.type == ContextType::OpenGL)
			m_extensionStatus[UnderlyingCast(Extension::PolygonMode)] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_NV_polygon_mode"))
			m_extensionStatus[UnderlyingCast(Extension::DepthClamp)] = ExtensionStatus::Vendor;

		// SPIR-V support
		if (m_params.type == ContextType::OpenGL && glVersion >= 460)
			m_extensionStatus[UnderlyingCast(Extension::SpirV)] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_gl_spirv"))
			m_extensionStatus[UnderlyingCast(Extension::SpirV)] = ExtensionStatus::ARB;

		// Storage buffers (SSBO)
		if ((m_params.type == ContextType::OpenGL && glVersion >= 430) || (m_params.type == ContextType::OpenGL_ES && glVersion >= 310))
			m_extensionStatus[UnderlyingCast(Extension::StorageBuffers)] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_shader_storage_buffer_object"))
			m_extensionStatus[UnderlyingCast(Extension::StorageBuffers)] = ExtensionStatus::ARB;

		// Texture compression (S3tc)
		if (m_supportedExtensions.count("GL_EXT_texture_compression_s3tc"))
			m_extensionStatus[UnderlyingCast(Extension::TextureCompressionS3tc)] = ExtensionStatus::EXT;

		// Texture anisotropic filter
		if (m_params.type == ContextType::OpenGL && glVersion >= 460)
			m_extensionStatus[UnderlyingCast(Extension::TextureFilterAnisotropic)] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_texture_filter_anisotropic"))
			m_extensionStatus[UnderlyingCast(Extension::TextureFilterAnisotropic)] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_EXT_texture_filter_anisotropic"))
			m_extensionStatus[UnderlyingCast(Extension::TextureFilterAnisotropic)] = ExtensionStatus::EXT;

#define NAZARA_OPENGLRENDERER_FUNC(name, sig)
#define NAZARA_OPENGLRENDERER_EXT_FUNC(name, sig) loader.Load<sig, UnderlyingCast(FunctionIndex:: name)>(name, #name, false);
		NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(NAZARA_OPENGLRENDERER_FUNC, NAZARA_OPENGLRENDERER_EXT_FUNC)
#undef NAZARA_OPENGLRENDERER_EXT_FUNC
#undef NAZARA_OPENGLRENDERER_FUNC

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
				// Disable push/pop debug groups notifications
				if (glPushDebugGroup)
					glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

				if (glPopDebugGroup)
					glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_POP_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

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

		GLint maxTextureUnits = -1;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
		if (maxTextureUnits < 32) //< OpenGL ES 3.0 requires at least 32 textures units
			NazaraWarning("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS is " + std::to_string(maxTextureUnits) + ", expected >= 32");

		assert(maxTextureUnits > 0);
		m_state.textureUnits.resize(maxTextureUnits);

		GLint maxUniformBufferUnits = -1;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferUnits);
		if (maxUniformBufferUnits < 24) //< OpenGL ES 3.0 requires at least 24 uniform buffers units
			NazaraWarning("GL_MAX_UNIFORM_BUFFER_BINDINGS is " + std::to_string(maxUniformBufferUnits) + ", expected >= 24");

		assert(maxUniformBufferUnits > 0);
		m_state.uboUnits.resize(maxUniformBufferUnits);

		if (IsExtensionSupported(Extension::StorageBuffers))
		{
			GLint maxStorageBufferUnits = -1;
			glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxStorageBufferUnits);
			if (maxStorageBufferUnits < 8) //< OpenGL ES 3.1 requires at least 8 storage buffers units
				NazaraWarning("GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS is " + std::to_string(maxUniformBufferUnits) + ", expected >= 8");

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

		EnableVerticalSync(false);

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

			NazaraError("OpenGL error: " + TranslateOpenGLError(lastError));
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
				glPolygonOffset(renderStates.depthBiasConstantFactor, renderStates.depthBiasSlopeFactor);
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
		m_blitFramebuffers.reset();
		m_vaoCache.Clear();
	}

	bool Context::ImplementFallback(const std::string_view& function)
	{
		SymbolLoader loader(*this);

		if (function == "glClipControl")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glClipControl);

			return loader.Load<PFNGLCLIPCONTROLEXTPROC, functionIndex>(glClipControl, "glClipControlEXT", false); //< from GL_EXT_clip_control

		}
		else if (function == "glDebugMessageCallback")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glDebugMessageCallback);

			return loader.Load<PFNGLDEBUGMESSAGECALLBACKKHRPROC, functionIndex>(glDebugMessageCallback, "glDebugMessageCallbackKHR", false) || //< from GL_KHR_debug
			       loader.Load<PFNGLDEBUGMESSAGECALLBACKPROC, functionIndex>(glDebugMessageCallback, "glDebugMessageCallbackARB", false);      //< from GL_ARB_debug_output
		}
		else if (function == "glPolygonMode")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glPolygonMode);

			return loader.Load<PFNGLPOLYGONMODENVPROC, functionIndex>(glPolygonMode, "glPolygonModeNV", false); //< from GL_NV_polygon_mode
		}
		else if (function == "glSpecializeShader")
		{
			constexpr std::size_t functionIndex = UnderlyingCast(FunctionIndex::glSpecializeShader);
			return loader.Load<PFNGLSPECIALIZESHADERPROC, functionIndex>(glSpecializeShader, "glSpecializeShaderARB", false); //< from GL_ARB_spirv_extensions
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
		std::stringstream ss;
		ss << "OpenGL debug message (ID: 0x" << id << "):\n";
		ss << "Sent by context: " << this;
		ss << "\n-Source: ";
		switch (source)
		{
			case GL_DEBUG_SOURCE_API:
				ss << "OpenGL API";
				break;

			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
				ss << "Operating system";
				break;

			case GL_DEBUG_SOURCE_SHADER_COMPILER:
				ss << "Shader compiler";
				break;

			case GL_DEBUG_SOURCE_THIRD_PARTY:
				ss << "Third party";
				break;

			case GL_DEBUG_SOURCE_APPLICATION:
				ss << "Application";
				break;

			case GL_DEBUG_SOURCE_OTHER:
				ss << "Other";
				break;

			default:
				// Extension source
				ss << "Unknown";
				break;
		}
		ss << '\n';

		ss << "-Type: ";
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:
				ss << "Error";
				break;

			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
				ss << "Deprecated behavior";
				break;

			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				ss << "Undefined behavior";
				break;

			case GL_DEBUG_TYPE_PORTABILITY:
				ss << "Portability";
				break;

			case GL_DEBUG_TYPE_PERFORMANCE:
				ss << "Performance";
				break;

			case GL_DEBUG_TYPE_OTHER:
				ss << "Other";
				break;

			default:
				// Extension type
				ss << "Unknown";
				break;
		}
		ss << '\n';

		ss << "-Severity: ";
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:
				ss << "High";
				break;

			case GL_DEBUG_SEVERITY_MEDIUM:
				ss << "Medium";
				break;

			case GL_DEBUG_SEVERITY_LOW:
				ss << "Low";
				break;

			case GL_DEBUG_SEVERITY_NOTIFICATION:
				ss << "Notification";
				break;

			default:
				ss << "Unknown";
				break;
		}
		ss << '\n';

		ss << "Message: " << std::string_view(message, length) << '\n';

		NazaraNotice(ss.str());
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
}
