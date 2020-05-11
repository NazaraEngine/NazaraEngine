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
#include <sstream>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	thread_local const Context* s_currentContext = nullptr;

	Context::~Context()
	{
		if (m_device)
			m_device->NotifyContextDestruction(*this);
	}

	void Context::BindBuffer(BufferTarget target, GLuint buffer) const
	{
		if (m_state.bufferTargets[UnderlyingCast(target)] != buffer)
		{
			if (!SetCurrentContext(this))
				throw std::runtime_error("failed to activate context");

			glBindBuffer(ToOpenGL(target), buffer);

			m_state.bufferTargets[UnderlyingCast(target)] = buffer;
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

	bool Context::Initialize(const ContextParams& params)
	{
		if (!Activate())
		{
			NazaraError("failed to activate context");
			return false;
		}

		const Loader& loader = GetLoader();

		auto LoadSymbol = [&](auto& func, const char* funcName, bool mandatory)
		{
			func = reinterpret_cast<std::decay_t<decltype(func)>>(loader.LoadFunction(funcName));
			if (!func && !ImplementFallback(funcName) && !func && mandatory) //< Not a mistake
				throw std::runtime_error("failed to load core function " + std::string(funcName));
		};

		try
		{
#define NAZARA_OPENGLRENDERER_FUNC(name, sig) LoadSymbol(name, #name, true);
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

		// Retrieve OpenGL version
		auto DecodeDigit = [](char c) -> int
		{
			if (c >= '0' && c <= '9')
				return c - '0';
			else
				return -1;
		};

		std::string_view versionString = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		if (versionString.size() > 2 && DecodeDigit(versionString[0]) >= 0 && versionString[1] == '.' && DecodeDigit(versionString[2]) >= 0)
		{
			m_params.glMajorVersion = DecodeDigit(versionString[0]);
			m_params.glMinorVersion = DecodeDigit(versionString[2]);
		}
		else
			NazaraWarning("Failed to decode OpenGL version: " + std::string(versionString));

		// Load extensions
		std::string_view extensionList = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
		SplitString(extensionList, " ", [&](std::string_view extension)
		{
			m_supportedExtensions.emplace(extension);
			return true;
		});

		m_extensionStatus.fill(ExtensionStatus::NotSupported);

		// TextureFilterAnisotropic
		if (m_supportedExtensions.count("GL_ARB_texture_filter_anisotropic"))
			m_extensionStatus[UnderlyingCast(Extension::TextureFilterAnisotropic)] = ExtensionStatus::ARB;
		else if (m_supportedExtensions.count("GL_EXT_texture_filter_anisotropic"))
			m_extensionStatus[UnderlyingCast(Extension::TextureFilterAnisotropic)] = ExtensionStatus::EXT;

		// SpirV
		if (m_supportedExtensions.count("GL_ARB_gl_spirv"))
			m_extensionStatus[UnderlyingCast(Extension::SpirV)] = ExtensionStatus::ARB;

#define NAZARA_OPENGLRENDERER_FUNC(name, sig)
#define NAZARA_OPENGLRENDERER_EXT_FUNC(name, sig) LoadSymbol(name, #name, false);
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
			glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
			{
				const Context* context = static_cast<const Context*>(userParam);
				context->HandleDebugMessage(source, type, id, severity, length, message);
			}, this);
		}

		GLint maxTextureUnits = -1;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
		if (maxTextureUnits < 32) //< OpenGL ES 3.0 requires at least 32 textures units
			NazaraWarning("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS is " + std::to_string(maxTextureUnits) + ", >= 32 expected");

		assert(maxTextureUnits > 0);
		m_state.textureUnits.resize(maxTextureUnits);

		return true;
	}

	void Context::UpdateStates(const RenderStates& renderStates) const
	{
		if (!SetCurrentContext(this))
			throw std::runtime_error("failed to activate context");

		if (renderStates.blending)
		{
			if (m_state.renderStates.dstBlend != renderStates.dstBlend ||
			    m_state.renderStates.srcBlend != renderStates.srcBlend)
			{
				glBlendFunc(ToOpenGL(renderStates.srcBlend), ToOpenGL(renderStates.dstBlend));
				m_state.renderStates.dstBlend = renderStates.dstBlend;
				m_state.renderStates.srcBlend = renderStates.srcBlend;
			}
		}

		if (renderStates.depthBuffer)
		{
			if (m_state.renderStates.depthCompare != renderStates.depthCompare)
			{
				glDepthFunc(ToOpenGL(m_state.renderStates.depthCompare));
				m_state.renderStates.depthCompare = renderStates.depthCompare;
			}

			if (m_state.renderStates.depthWrite != renderStates.depthWrite)
			{
				glDepthMask((renderStates.depthWrite) ? GL_TRUE : GL_FALSE);
				m_state.renderStates.depthWrite = renderStates.depthWrite;
			}
		}

		if (renderStates.faceCulling)
		{
			if (m_state.renderStates.cullingSide != renderStates.cullingSide)
			{
				glCullFace(ToOpenGL(m_state.renderStates.cullingSide));
				m_state.renderStates.cullingSide = renderStates.cullingSide;
			}
		}

		/*
		TODO: Use glPolyonMode if available (OpenGL)
		if (m_state.renderStates.faceFilling != renderStates.faceFilling)
		{
			glPolygonMode(GL_FRONT_AND_BACK, FaceFilling[states.faceFilling]);
			m_state.renderStates.faceFilling = renderStates.faceFilling;
		}*/

		if (renderStates.stencilTest)
		{
			auto ApplyStencilStates = [&](bool front)
			{
				auto& currentStencilData = (front) ? m_state.renderStates.stencilFront : m_state.renderStates.stencilBack;
				auto& newStencilData = (front) ? renderStates.stencilFront : renderStates.stencilBack;

				if (currentStencilData.compare != newStencilData.compare ||
				    currentStencilData.reference != newStencilData.reference ||
				    currentStencilData.writeMask != newStencilData.writeMask)
				{
					glStencilFuncSeparate((front) ? GL_FRONT : GL_BACK, ToOpenGL(newStencilData.compare), newStencilData.reference, newStencilData.writeMask);
					currentStencilData.compare = newStencilData.compare;
					currentStencilData.reference = newStencilData.reference;
					currentStencilData.writeMask = newStencilData.writeMask;
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
			};

			ApplyStencilStates(true);
			ApplyStencilStates(false);
		}

		if (!NumberEquals(m_state.renderStates.lineWidth, renderStates.lineWidth, 0.001f))
		{
			glLineWidth(renderStates.lineWidth);
			m_state.renderStates.lineWidth = renderStates.lineWidth;
		}

		/*if (!NumberEquals(m_state.renderStates.pointSize, renderStates.pointSize, 0.001f))
		{
			glPointSize(renderStates.pointSize);
			m_state.renderStates.pointSize = renderStates.pointSize;
		}*/

		if (m_state.renderStates.blending != renderStates.blending)
		{
			if (renderStates.blending)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);

			m_state.renderStates.blending = renderStates.blending;
		}

		if (m_state.renderStates.colorWrite != renderStates.colorWrite)
		{
			GLboolean param = (renderStates.colorWrite) ? GL_TRUE : GL_FALSE;
			glColorMask(param, param, param, param);

			m_state.renderStates.colorWrite = renderStates.colorWrite;
		}

		if (m_state.renderStates.depthBuffer != renderStates.depthBuffer)
		{
			if (renderStates.depthBuffer)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			m_state.renderStates.depthBuffer = renderStates.depthBuffer;
		}

		if (m_state.renderStates.faceCulling != renderStates.faceCulling)
		{
			if (renderStates.faceCulling)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);

			m_state.renderStates.faceCulling = renderStates.faceCulling;
		}

		if (m_state.renderStates.scissorTest != renderStates.scissorTest)
		{
			if (renderStates.scissorTest)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);

			m_state.renderStates.scissorTest = renderStates.scissorTest;
		}

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
		const Context*& currentContext = s_currentContext; //< Pay TLS cost only once
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

	bool Context::ImplementFallback(const std::string_view& function)
	{
		const Loader& loader = GetLoader();

		auto LoadSymbol = [&](auto& func, const char* funcName) -> bool
		{
			func = reinterpret_cast<std::decay_t<decltype(func)>>(loader.LoadFunction(funcName));
			return func;
		};

		if (function == "glDebugMessageCallback")
		{
			if (!LoadSymbol(glDebugMessageCallback, "glDebugMessageCallbackARB"))
				return LoadSymbol(glDebugMessageCallback, "DebugMessageCallbackAMD");

			return true;
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
		ss << "OpenGL debug message (ID: 0x" << std::to_string(id) << "):\n";
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
				// Peut être rajouté par une extension
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
				// Peut être rajouté par une extension
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

			default:
				ss << "Unknown";
				break;
		}
		ss << '\n';

		ss << "Message: " << std::string_view(message, length) << '\n';

		NazaraNotice(ss.str());
	}
}
