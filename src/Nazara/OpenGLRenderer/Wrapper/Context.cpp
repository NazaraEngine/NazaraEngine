// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	thread_local const Context* s_currentContext = nullptr;

	namespace
	{
		template<typename FuncType, std::size_t FuncIndex, typename>
		struct GLWrapper;

		template<typename FuncType, std::size_t FuncIndex, typename Ret, typename... Args>
		struct GLWrapper<FuncType, FuncIndex, Ret(Args...)>
		{
			static auto WrapErrorHandling()
			{
				return [](Args... args) -> Ret
				{
					const Context* context = s_currentContext; //< pay TLS cost once
					assert(context);

					FuncType funcPtr = reinterpret_cast<FuncType>(context->GetFunctionByIndex(FuncIndex));

					context->ClearErrorStack();

					if constexpr (std::is_same_v<Ret, void>)
					{
						funcPtr(std::forward<Args>(args)...);

						context->ProcessErrorStack();
					}
					else
					{
						Ret r = funcPtr(std::forward<Args>(args)...);

						context->ProcessErrorStack();

						return r;
					}
				};
			}
		};
	}

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

#if defined(NAZARA_OPENGLRENDERER_DEBUG) && (!defined(NAZARA_COMPILER_MSVC) || defined(NAZARA_PLATFORM_x64))
			if (func)
			{
				if (std::strcmp(funcName, "glGetError") != 0) //< Prevent infinite recursion
				{
					using Wrapper = GLWrapper<FuncType, FuncIndex, std::remove_pointer_t<FuncType>>;
					func = Wrapper::WrapErrorHandling();
				}
			}
#endif

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
	};

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

	void Context::BindFramebuffer(GLuint fbo) const
	{
		if (m_state.boundDrawFBO != fbo || m_state.boundReadFBO != fbo)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			m_state.boundDrawFBO = fbo;
			m_state.boundReadFBO = fbo;
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

	bool Context::ClearErrorStack() const
	{
		assert(GetCurrentContext() == this);

		while (glGetError() != GL_NO_ERROR);

		return true;
	}

	bool Context::Initialize(const ContextParams& params)
	{
		if (!SetCurrentContext(this))
		{
			NazaraError("failed to activate context");
			return false;
		}

		SymbolLoader loader(*this);

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

		// Load extensions
		GLint extensionCount = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

		for (GLint i = 0; i < extensionCount; ++i)
			m_supportedExtensions.emplace(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));

		m_extensionStatus.fill(ExtensionStatus::NotSupported);

		// Depth clamp
		if (m_params.type == ContextType::OpenGL && m_params.glMajorVersion >= 3 && m_params.glMajorVersion >= 2)
			m_extensionStatus[UnderlyingCast(Extension::DepthClamp)] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_depth_clamp"))
			m_extensionStatus[UnderlyingCast(Extension::DepthClamp)] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_EXT_depth_clamp"))
			m_extensionStatus[UnderlyingCast(Extension::DepthClamp)] = ExtensionStatus::EXT;
		else if (m_supportedExtensions.count("GL_NV_depth_clamp"))
			m_extensionStatus[UnderlyingCast(Extension::DepthClamp)] = ExtensionStatus::Vendor;

		// SpirV
		if (m_params.type == ContextType::OpenGL && m_params.glMajorVersion >= 4 && m_params.glMajorVersion >= 6)
			m_extensionStatus[UnderlyingCast(Extension::SpirV)] = ExtensionStatus::Core;
		else if (m_supportedExtensions.count("GL_ARB_gl_spirv"))
			m_extensionStatus[UnderlyingCast(Extension::SpirV)] = ExtensionStatus::ARB;

		// Texture compression (S3tc)
		if (m_supportedExtensions.count("GL_EXT_texture_compression_s3tc"))
			m_extensionStatus[UnderlyingCast(Extension::TextureCompressionS3tc)] = ExtensionStatus::EXT;

		// Texture anisotropic filter
		if (m_params.type == ContextType::OpenGL && m_params.glMajorVersion >= 4 && m_params.glMajorVersion >= 6)
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

		// If we requested an OpenGL ES context but cannot create one, check for some compatibility extensions
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

		// Set debug callback (if supported)
		if (glDebugMessageCallback)
		{
			glEnable(GL_DEBUG_OUTPUT);
#ifdef NAZARA_DEBUG
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

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

				// Disable driver notifications (NVidia driver is very verbose)
				glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
			}
		}

		GLint maxTextureUnits = -1;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
		if (maxTextureUnits < 32) //< OpenGL ES 3.0 requires at least 32 textures units
			NazaraWarning("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS is " + std::to_string(maxTextureUnits) + ", >= 32 expected");

		assert(maxTextureUnits > 0);
		m_state.textureUnits.resize(maxTextureUnits);

		GLint maxUniformBufferUnits = -1;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferUnits);
		if (maxUniformBufferUnits < 24) //< OpenGL ES 3.0 requires at least 24 uniform buffers units
			NazaraWarning("GL_MAX_UNIFORM_BUFFER_BINDINGS is " + std::to_string(maxUniformBufferUnits) + ", >= 24 expected");

		assert(maxUniformBufferUnits > 0);
		m_state.uboUnits.resize(maxUniformBufferUnits);

		std::array<GLint, 4> res;

		glGetIntegerv(GL_SCISSOR_BOX, res.data());
		m_state.scissorBox = { res[0], res[1], res[2], res[3] };

		glGetIntegerv(GL_VIEWPORT, res.data());
		m_state.viewport = { res[0], res[1], res[2], res[3] };

		m_state.renderStates.frontFace = FrontFace::CounterClockwise; //< OpenGL default front face is counter-clockwise
		m_state.renderStates.depthCompare = RendererComparison::Less; //< OpenGL default depth mode is GL_LESS

		EnableVerticalSync(false);

		return true;
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

		// Face culling side
		if (renderStates.faceCulling)
		{
			if (m_state.renderStates.cullingSide != renderStates.cullingSide)
			{
				glCullFace(ToOpenGL(renderStates.cullingSide));
				m_state.renderStates.cullingSide = renderStates.cullingSide;
			}
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
		if (m_state.renderStates.colorWrite != renderStates.colorWrite)
		{
			GLboolean param = (renderStates.colorWrite) ? GL_TRUE : GL_FALSE;
			glColorMask(param, param, param, param);

			m_state.renderStates.colorWrite = renderStates.colorWrite;
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
				glEnable(GL_DEPTH_CLAMP_EXT);
			else
				glDisable(GL_DEPTH_CLAMP_EXT);

			m_state.renderStates.depthClamp = renderStates.depthClamp;
		}

		// Face culling
		if (m_state.renderStates.faceCulling != renderStates.faceCulling)
		{
			if (renderStates.faceCulling)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);

			m_state.renderStates.faceCulling = renderStates.faceCulling;
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
		return s_currentContext;
	}

	bool Context::SetCurrentContext(const Context* context)
	{
		const Context*& currentContext = s_currentContext; //< Pay TLS cost once
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
		m_vaoCache.Clear();
	}

	bool Context::ImplementFallback(const std::string_view& function)
	{
		SymbolLoader loader(*this);

		if (function == "glDebugMessageCallback")
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

		return false;
	}

	void Context::NotifyContextDestruction(Context* context)
	{
		const Context*& currentContext = s_currentContext; //< Pay TLS cost only once
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
}
