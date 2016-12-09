// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#if defined(NAZARA_PLATFORM_GLX)
#include <Nazara/Utility/X11/Display.hpp>
#endif // NAZARA_PLATFORM_GLX
#include <cstring>
#include <set>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	namespace
	{
		#ifdef NAZARA_PLATFORM_WINDOWS
		HMODULE openGLlibrary;
		#endif

		OpenGLFunc LoadEntry(const char* name, bool launchException = true)
		{
			#if defined(NAZARA_PLATFORM_WINDOWS)
			OpenGLFunc entry = reinterpret_cast<OpenGLFunc>(wglGetProcAddress(name));
			if (!entry) // wglGetProcAddress ne fonctionne pas sur les fonctions OpenGL <= 1.1
				entry = reinterpret_cast<OpenGLFunc>(GetProcAddress(openGLlibrary, name));
			#elif defined(NAZARA_PLATFORM_GLX)
			OpenGLFunc entry = reinterpret_cast<OpenGLFunc>(GLX::glXGetProcAddress(reinterpret_cast<const unsigned char*>(name)));
			#else
			#error OS not handled
			#endif

			if (!entry && launchException)
			{
				std::ostringstream oss;
				oss << "failed to load \"" << name << '"';

				throw std::runtime_error(oss.str());
			}

			return entry;
		}

		bool LoadLibrary()
		{
			#ifdef NAZARA_PLATFORM_WINDOWS
			openGLlibrary = ::LoadLibraryA("opengl32.dll");

			return openGLlibrary != nullptr;
			#else
			return true;
			#endif
		}

		void UnloadLibrary()
		{
			#ifdef NAZARA_PLATFORM_WINDOWS
			FreeLibrary(openGLlibrary);
			#endif
		}

		enum GarbageResourceType
		{
			GarbageResourceType_FrameBuffer,
			GarbageResourceType_VertexArray
		};

		struct ContextStates
		{
			std::vector<std::pair<GarbageResourceType, GLuint>> garbage; // Les ressources à supprimer dès que possible
			GLuint buffersBinding[BufferType_Max + 1] = {0};
			GLuint currentProgram = 0;
			GLuint samplers[32] = {0}; // 32 est pour l'instant la plus haute limite (GL_TEXTURE31)
			GLuint texturesBinding[32] = {0}; // 32 est pour l'instant la plus haute limite (GL_TEXTURE31)
			Recti currentScissorBox = Recti(0, 0, 0, 0);
			Recti currentViewport = Recti(0, 0, 0, 0);
			RenderStates renderStates; // Toujours synchronisé avec OpenGL
			const RenderTarget* currentTarget = nullptr;
			bool scissorBoxUpdated = true;
			bool viewportUpdated = true;
			unsigned int textureUnit = 0;
		};

		std::set<String> s_openGLextensionSet;
		std::unordered_map<const Context*, ContextStates> s_contexts;
		thread_local ContextStates* s_contextStates = nullptr;
		String s_rendererName;
		String s_vendorName;
		bool s_initialized = false;
		bool s_openGLextensions[OpenGLExtension_Max + 1] = {false};
		unsigned int s_glslVersion = 0;
		unsigned int s_openglVersion = 0;

		bool LoadExtensionsString(const String& extensionString)
		{
			if (extensionString.IsEmpty())
			{
				NazaraError("Unable to get extension string");
				return false;
			}

			// On peut sûrement faire plus rapide mais comme ça ne se fait qu'une fois et que String implémente le COW...
			std::vector<String> ext;
			extensionString.Split(ext);

			for (std::vector<String>::iterator it = ext.begin(); it != ext.end(); ++it)
				s_openGLextensionSet.insert(*it);

			return true;
		}

		bool LoadExtensions3()
		{
			GLint extensionCount = 0;
			glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

			if (extensionCount <= 0)
			{
				NazaraError("Unable to get extension count");
				return false;
			}

			for (int i = 0; i < extensionCount; ++i)
			{
				String extension(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));
				if (extension.IsEmpty())
				{
					NazaraWarning("Unable to get extension #" + String::Number(i));
					continue;
				}

				s_openGLextensionSet.insert(extension);
			}

			return true;
		}
	}

	void OpenGL::ApplyStates(const RenderStates& states)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		RenderStates& currentRenderStates = s_contextStates->renderStates;

		// Les fonctions de blend n'a aucun intérêt sans blending
		if (states.blending)
		{
			if (currentRenderStates.dstBlend != states.dstBlend ||
				currentRenderStates.srcBlend != states.srcBlend)
			{
				glBlendFunc(BlendFunc[states.srcBlend], BlendFunc[states.dstBlend]);
				currentRenderStates.dstBlend = states.dstBlend;
				currentRenderStates.srcBlend = states.srcBlend;
			}
		}

		if (states.depthBuffer)
		{
			// La comparaison de profondeur n'a aucun intérêt sans depth buffer
			if (currentRenderStates.depthFunc != states.depthFunc)
			{
				glDepthFunc(RendererComparison[states.depthFunc]);
				currentRenderStates.depthFunc = states.depthFunc;
			}

			// Le DepthWrite n'a aucune importance si le DepthBuffer est désactivé
			if (currentRenderStates.depthWrite != states.depthWrite)
			{
				glDepthMask((states.depthWrite) ? GL_TRUE : GL_FALSE);
				currentRenderStates.depthWrite = states.depthWrite;
			}
		}

		// Inutile de changer le mode de face culling s'il n'est pas actif
		if (states.faceCulling)
		{
			if (currentRenderStates.cullingSide != states.cullingSide)
			{
				glCullFace(FaceSide[states.cullingSide]);
				currentRenderStates.cullingSide = states.cullingSide;
			}
		}

		if (currentRenderStates.faceFilling != states.faceFilling)
		{
			glPolygonMode(GL_FRONT_AND_BACK, FaceFilling[states.faceFilling]);
			currentRenderStates.faceFilling = states.faceFilling;
		}

		// Ici encore, ça ne sert à rien de se soucier des fonctions de stencil sans qu'il soit activé
		if (states.stencilTest)
		{
			if (currentRenderStates.stencilCompare.back != states.stencilCompare.back ||
				currentRenderStates.stencilReference.back != states.stencilReference.back ||
				currentRenderStates.stencilWriteMask.back != states.stencilWriteMask.back)
			{
				glStencilFuncSeparate(GL_BACK, RendererComparison[states.stencilCompare.back], states.stencilReference.back, states.stencilWriteMask.back);
				currentRenderStates.stencilCompare.back = states.stencilCompare.back;
				currentRenderStates.stencilReference.back = states.stencilReference.back;
				currentRenderStates.stencilWriteMask.back = states.stencilWriteMask.back;
			}

			if (currentRenderStates.stencilDepthFail.back != states.stencilDepthFail.back ||
				currentRenderStates.stencilFail.back != states.stencilFail.back ||
				currentRenderStates.stencilPass.back != states.stencilPass.back)
			{
				glStencilOpSeparate(GL_BACK, StencilOperation[states.stencilFail.back], StencilOperation[states.stencilDepthFail.back], StencilOperation[states.stencilPass.back]);
				currentRenderStates.stencilDepthFail.back = states.stencilDepthFail.back;
				currentRenderStates.stencilFail.back = states.stencilFail.back;
				currentRenderStates.stencilPass.back = states.stencilPass.back;
			}

			if (currentRenderStates.stencilCompare.front != states.stencilCompare.front ||
				currentRenderStates.stencilReference.front != states.stencilReference.front ||
				currentRenderStates.stencilWriteMask.front != states.stencilWriteMask.front)
			{
				glStencilFuncSeparate(GL_FRONT, RendererComparison[states.stencilCompare.front], states.stencilReference.front, states.stencilWriteMask.front);
				currentRenderStates.stencilCompare.front = states.stencilCompare.front;
				currentRenderStates.stencilReference.front = states.stencilReference.front;
				currentRenderStates.stencilWriteMask.front = states.stencilWriteMask.front;
			}

			if (currentRenderStates.stencilDepthFail.front != states.stencilDepthFail.front ||
				currentRenderStates.stencilFail.front != states.stencilFail.front ||
				currentRenderStates.stencilPass.front != states.stencilPass.front)
			{
				glStencilOpSeparate(GL_FRONT, StencilOperation[states.stencilFail.front], StencilOperation[states.stencilDepthFail.front], StencilOperation[states.stencilPass.front]);
				currentRenderStates.stencilDepthFail.front = states.stencilDepthFail.front;
				currentRenderStates.stencilFail.front = states.stencilFail.front;
				currentRenderStates.stencilPass.front = states.stencilPass.front;
			}
		}

		if (!NumberEquals(currentRenderStates.lineWidth, states.lineWidth, 0.001f))
		{
			glLineWidth(states.lineWidth);
			currentRenderStates.lineWidth = states.lineWidth;
		}

		if (!NumberEquals(currentRenderStates.pointSize, states.pointSize, 0.001f))
		{
			glPointSize(states.pointSize);
			currentRenderStates.pointSize = states.pointSize;
		}

		// Paramètres de rendu
		if (currentRenderStates.blending != states.blending)
		{
			if (states.blending)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);

			currentRenderStates.blending = states.blending;
		}

		if (currentRenderStates.colorWrite != states.colorWrite)
		{
			GLboolean param = (states.colorWrite) ? GL_TRUE : GL_FALSE;
			glColorMask(param, param, param, param);

			currentRenderStates.colorWrite = states.colorWrite;
		}

		if (currentRenderStates.depthBuffer != states.depthBuffer)
		{
			if (states.depthBuffer)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			currentRenderStates.depthBuffer = states.depthBuffer;
		}

		if (currentRenderStates.faceCulling != states.faceCulling)
		{
			if (states.faceCulling)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);

			currentRenderStates.faceCulling = states.faceCulling;
		}

		if (currentRenderStates.scissorTest != states.scissorTest)
		{
			if (states.scissorTest)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);

			currentRenderStates.scissorTest = states.scissorTest;
		}

		if (currentRenderStates.stencilTest != states.stencilTest)
		{
			if (states.stencilTest)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);

			currentRenderStates.stencilTest = states.stencilTest;
		}
	}

	void OpenGL::BindBuffer(BufferType type, GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		if (s_contextStates->buffersBinding[type] != id)
		{
			glBindBuffer(BufferTarget[type], id);
			s_contextStates->buffersBinding[type] = id;
		}
	}

	void OpenGL::BindProgram(GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		if (s_contextStates->currentProgram != id)
		{
			glUseProgram(id);
			s_contextStates->currentProgram = id;
		}
	}

	void OpenGL::BindSampler(GLuint unit, GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!glBindSampler)
		{
			NazaraError("Sampler are not supported");
			return;
		}

		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		if (s_contextStates->samplers[unit] != id)
		{
			glBindSampler(unit, id);
			s_contextStates->samplers[unit] = id;
		}
	}

	void OpenGL::BindScissorBox(const Recti& scissorBox)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		#if NAZARA_RENDERER_SAFE
		if (scissorBox.width < 0)
		{
			NazaraError("Scissor box width must be positive");
			return;
		}

		if (scissorBox.height < 0)
		{
			NazaraError("Scissor box height must be positive");
			return;
		}
		#endif

		if (s_contextStates->currentScissorBox != scissorBox)
		{
			if (s_contextStates->currentTarget)
			{
				unsigned int height = s_contextStates->currentTarget->GetHeight();
				glScissor(scissorBox.x, height - scissorBox.height - scissorBox.y, scissorBox.width, scissorBox.height);
				s_contextStates->scissorBoxUpdated = true;
			}
			else
				s_contextStates->scissorBoxUpdated = false; // Sinon on attend d'avoir un target

			s_contextStates->currentScissorBox = scissorBox;
		}
	}

	void OpenGL::BindTexture(ImageType type, GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		if (s_contextStates->texturesBinding[s_contextStates->textureUnit] != id)
		{
			glBindTexture(TextureTarget[type], id);
			s_contextStates->texturesBinding[s_contextStates->textureUnit] = id;
		}
	}

	void OpenGL::BindTexture(unsigned int textureUnit, ImageType type, GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		if (s_contextStates->texturesBinding[textureUnit] != id)
		{
			BindTextureUnit(textureUnit);

			glBindTexture(TextureTarget[type], id);
			s_contextStates->texturesBinding[textureUnit] = id;
		}
	}

	void OpenGL::BindTextureUnit(unsigned int textureUnit)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		if (s_contextStates->textureUnit != textureUnit)
		{
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			s_contextStates->textureUnit = textureUnit;
		}
	}

	void OpenGL::BindViewport(const Recti& viewport)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		#if NAZARA_RENDERER_SAFE
		if (viewport.width < 0)
		{
			NazaraError("Viewport width must be positive");
			return;
		}

		if (viewport.height < 0)
		{
			NazaraError("Viewport height must be positive");
			return;
		}
		#endif

		if (s_contextStates->currentViewport != viewport)
		{
			if (s_contextStates->currentTarget)
			{
				unsigned int height = s_contextStates->currentTarget->GetHeight();
				glViewport(viewport.x, height - viewport.height - viewport.y, viewport.width, viewport.height);
				s_contextStates->viewportUpdated = true;
			}
			else
				s_contextStates->viewportUpdated = false; // Sinon on attend d'avoir un target

			s_contextStates->currentViewport = viewport;
		}
	}

	void OpenGL::DeleteBuffer(BufferType type, GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		glDeleteBuffers(1, &id);
		if (s_contextStates->buffersBinding[type] == id)
			s_contextStates->buffersBinding[type] = 0;
	}

	void OpenGL::DeleteFrameBuffer(const Context* context, GLuint id)
	{
		// Si le contexte est actif, ne nous privons pas
		if (Context::GetCurrent() == context)
			glDeleteFramebuffers(1, &id);
		else
			s_contexts[context].garbage.emplace_back(GarbageResourceType_FrameBuffer, id);
	}

	void OpenGL::DeleteProgram(GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		glDeleteProgram(id);
		if (s_contextStates->currentProgram == id)
			s_contextStates->currentProgram = 0;
	}

	void OpenGL::DeleteSampler(GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!glDeleteSamplers)
		{
			NazaraError("Sampler are not supported");
			return;
		}

		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		glDeleteSamplers(1, &id);

		for (GLuint& binding : s_contextStates->samplers)
		{
			if (binding == id)
				binding = 0;
		}
	}

	void OpenGL::DeleteTexture(GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		glDeleteTextures(1, &id);

		for (GLuint& binding : s_contextStates->texturesBinding)
		{
			if (binding == id)
				binding = 0;
		}
	}

	void OpenGL::DeleteVertexArray(const Context* context, GLuint id)
	{
		// Si le contexte est actif, ne nous privons pas
		if (Context::GetCurrent() == context)
			glDeleteFramebuffers(1, &id);
		else
			s_contexts[context].garbage.emplace_back(GarbageResourceType_VertexArray, id);
	}

	GLuint OpenGL::GetCurrentBuffer(BufferType type)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return 0;
		}
		#endif

		return s_contextStates->buffersBinding[type];
	}

	GLuint OpenGL::GetCurrentProgram()
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return 0;
		}
		#endif

		return s_contextStates->currentProgram;
	}

	Recti OpenGL::GetCurrentScissorBox()
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return Recti();
		}
		#endif

		return s_contextStates->currentScissorBox;
	}

	const RenderTarget* OpenGL::GetCurrentTarget()
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return nullptr;
		}
		#endif

		return s_contextStates->currentTarget;
	}

	GLuint OpenGL::GetCurrentTexture()
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return 0;
		}
		#endif

		return s_contextStates->texturesBinding[s_contextStates->textureUnit];
	}

	GLuint OpenGL::GetCurrentTexture(unsigned int textureUnit)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return 0;
		}
		#endif

		return s_contextStates->texturesBinding[textureUnit];
	}

	unsigned int OpenGL::GetCurrentTextureUnit()
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return 0;
		}
		#endif

		return s_contextStates->textureUnit;
	}

	Recti OpenGL::GetCurrentViewport()
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return Recti();
		}
		#endif

		return s_contextStates->currentViewport;
	}

	OpenGLFunc OpenGL::GetEntry(const String& entryPoint)
	{
		return LoadEntry(entryPoint.GetConstBuffer(), false);
	}

	unsigned int OpenGL::GetGLSLVersion()
	{
		return s_glslVersion;
	}

	String OpenGL::GetRendererName()
	{
		return s_rendererName;
	}

	String OpenGL::GetVendorName()
	{
		return s_vendorName;
	}

	unsigned int OpenGL::GetVersion()
	{
		return s_openglVersion;
	}

	bool OpenGL::Initialize()
	{
		if (s_initialized)
			return true;

		#if defined(NAZARA_PLATFORM_GLX)
		Initializer<X11> display;
		if (!display)
		{
			NazaraError("Failed to load display library");
			return false;
		}
		#endif

		if (!LoadLibrary())
		{
			NazaraError("Failed to load OpenGL library");
			return false;
		}

		s_initialized = true;

		// En cas d'erreur, on libèrera OpenGL
		CallOnExit onExit(OpenGL::Uninitialize);

		// Le chargement des fonctions OpenGL nécessite un contexte OpenGL
		ContextParameters parameters;
		parameters.majorVersion = 2;
		parameters.minorVersion = 0;
		parameters.shared = false;

		/*
			Note: Même le contexte de chargement nécessite quelques fonctions de base pour correctement s'initialiser
			Pour cette raison, deux contextes sont créés, le premier sert à récupérer les fonctions permetttant
			de créer le second avec les bons paramètres.

			Non sérieusement si vous avez une meilleure idée, contactez-moi
		*/

		/****************************Chargement OpenGL****************************/

		///FIXME: I'm really thinking this is a mistake and GLX has no need to be initialized differently (Lynix)
		#if defined(NAZARA_PLATFORM_LINUX)
		glXCreateContextAttribs = reinterpret_cast<GLX::PFNGLXCREATECONTEXTATTRIBSARBPROC>(LoadEntry("glXCreateContextAttribsARB", false));
		#endif

		Context loadContext;
		if (!loadContext.Create(parameters))
		{
			NazaraError("Failed to create load context");
			return false;
		}

		#if defined(NAZARA_PLATFORM_WINDOWS)
		wglCreateContextAttribs = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(LoadEntry("wglCreateContextAttribsARB", false));
		wglChoosePixelFormat = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(LoadEntry("wglChoosePixelFormatARB", false));
		if (!wglChoosePixelFormat)
			wglChoosePixelFormat = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATEXTPROC>(LoadEntry("wglChoosePixelFormatEXT", false));
		#endif

		// Récupération de la version d'OpenGL et du GLSL
		// Ce code se base sur le fait que la carte graphique renverra un contexte de compatibilité avec la plus haute version supportée
		// Ce qui semble vrai chez AMD, NVidia et Intel, mais j'aimerai une preuve que ça sera toujours le cas...
		glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(LoadEntry("glGetString", false));
		if (!glGetString)
		{
			NazaraError("Unable to load OpenGL: failed to load glGetString");
			return false;
		}

		const GLubyte* version;
		unsigned int major;
		unsigned int minor;

		version = glGetString(GL_VERSION);
		if (!version)
		{
			NazaraError("Unable to retrieve OpenGL version");
			return false;
		}

		major = version[0] - '0';
		minor = version[2] - '0';

		if (major == 0 || major > 9)
		{
			NazaraError("Unable to retrieve OpenGL major version");
			return false;
		}

		if (minor > 9) // 0 est une valeur correcte ici (ex: OpenGL 3.0)
		{
			NazaraWarning("Unable to retrieve OpenGL minor version (assuming 0)");
			minor = 0;
		}

		s_openglVersion = major * 100 + minor * 10; // Donnera 330 pour OpenGL 3.3, 410 pour OpenGL 4.1, bien plus facile à comparer

		NazaraDebug("OpenGL version: " + String::Number(major) + '.' + String::Number(minor));

		// Le moteur nécessite OpenGL 3.3, autant s'arrêter là si ce n'est pas le cas
		if (s_openglVersion < 330)
		{
			NazaraError("OpenGL " + String::Number(major) + '.' + String::Number(minor) + " detected (3.3 required). Please upgrade your drivers or your video card");
			return false;
		}

		// Récupération de la version du GLSL, même technique
		version = glGetString(GL_SHADING_LANGUAGE_VERSION);
		if (!version)
		{
			NazaraError("Unable to retrieve GLSL version");
			return false;
		}

		major = version[0] - '0';
		minor = version[2] - '0';

		if (major == 0 || major > 9)
		{
			NazaraError("Unable to retrieve GLSL major version");
			return false;
		}

		if (minor > 9) // 0 est une valeur correcte ici (ex: GLSL 4.0)
		{
			NazaraWarning("Unable to retrieve GLSL minor version (using 0)");
			minor = 0;
		}

		s_glslVersion = major * 100 + minor * 10; // GLSL 3.3 => 330

		// Normalement rejeté il y a un moment déjà, mais on doit s'attendre à tout de la part d'un driver...
		if (s_glslVersion < 330)
		{
			NazaraError("GLSL version is too low, please upgrade your drivers or your video card");
			return false;
		}

		parameters.debugMode = true; // Certaines extensions n'apparaissent qu'avec un contexte de debug (e.g. ARB_debug_output)
		parameters.majorVersion = ContextParameters::defaultMajorVersion = major;
		parameters.minorVersion = ContextParameters::defaultMinorVersion = minor;

		if (!loadContext.Create(parameters)) // Destruction implicite du premier contexte
		{
			NazaraError("Failed to create load context");
			return false;
		}

		/****************************************Noyau****************************************/

		try
		{
			glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(LoadEntry("glActiveTexture"));
			glAttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(LoadEntry("glAttachShader"));
			glBeginConditionalRender = reinterpret_cast<PFNGLBEGINCONDITIONALRENDERPROC>(LoadEntry("glBeginConditionalRender"));
			glBeginQuery = reinterpret_cast<PFNGLBEGINQUERYPROC>(LoadEntry("glBeginQuery"));
			glBindAttribLocation = reinterpret_cast<PFNGLBINDATTRIBLOCATIONPROC>(LoadEntry("glBindAttribLocation"));
			glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(LoadEntry("glBindBuffer"));
			glBindFragDataLocation = reinterpret_cast<PFNGLBINDFRAGDATALOCATIONPROC>(LoadEntry("glBindFragDataLocation"));
			glBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(LoadEntry("glBindFramebuffer"));
			glBindRenderbuffer = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(LoadEntry("glBindRenderbuffer"));
			glBindSampler = reinterpret_cast<PFNGLBINDSAMPLERPROC>(LoadEntry("glBindSampler"));
			glBindTexture = reinterpret_cast<PFNGLBINDTEXTUREPROC>(LoadEntry("glBindTexture"));
			glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(LoadEntry("glBindVertexArray"));
			glBlendFunc = reinterpret_cast<PFNGLBLENDFUNCPROC>(LoadEntry("glBlendFunc"));
			glBlendFuncSeparate = reinterpret_cast<PFNGLBLENDFUNCSEPARATEPROC>(LoadEntry("glBlendFuncSeparate"));
			glBlitFramebuffer = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(LoadEntry("glBlitFramebuffer"));
			glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(LoadEntry("glBufferData"));
			glBufferSubData = reinterpret_cast<PFNGLBUFFERSUBDATAPROC>(LoadEntry("glBufferSubData"));
			glClear = reinterpret_cast<PFNGLCLEARPROC>(LoadEntry("glClear"));
			glClearColor = reinterpret_cast<PFNGLCLEARCOLORPROC>(LoadEntry("glClearColor"));
			glClearDepth = reinterpret_cast<PFNGLCLEARDEPTHPROC>(LoadEntry("glClearDepth"));
			glClearStencil = reinterpret_cast<PFNGLCLEARSTENCILPROC>(LoadEntry("glClearStencil"));
			glCheckFramebufferStatus = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(LoadEntry("glCheckFramebufferStatus"));
			glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(LoadEntry("glCreateProgram"));
			glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(LoadEntry("glCreateShader"));
			glColorMask = reinterpret_cast<PFNGLCOLORMASKPROC>(LoadEntry("glColorMask"));
			glCompressedTexSubImage1D = reinterpret_cast<PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC>(LoadEntry("glCompressedTexSubImage1D"));
			glCompressedTexSubImage2D = reinterpret_cast<PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC>(LoadEntry("glCompressedTexSubImage2D"));
			glCompressedTexSubImage3D = reinterpret_cast<PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC>(LoadEntry("glCompressedTexSubImage3D"));
			glCullFace = reinterpret_cast<PFNGLCULLFACEPROC>(LoadEntry("glCullFace"));
			glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(LoadEntry("glCompileShader"));
			glCopyTexSubImage2D = reinterpret_cast<PFNGLCOPYTEXSUBIMAGE2DPROC>(LoadEntry("glCopyTexSubImage2D"));
			glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(LoadEntry("glDeleteBuffers"));
			glDeleteFramebuffers = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(LoadEntry("glDeleteFramebuffers"));
			glDeleteQueries = reinterpret_cast<PFNGLDELETEQUERIESPROC>(LoadEntry("glDeleteQueries"));
			glDeleteProgram = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(LoadEntry("glDeleteProgram"));
			glDeleteRenderbuffers = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(LoadEntry("glDeleteRenderbuffers"));
			glDeleteSamplers = reinterpret_cast<PFNGLDELETESAMPLERSPROC>(LoadEntry("glDeleteSamplers"));
			glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(LoadEntry("glDeleteShader"));
			glDeleteTextures = reinterpret_cast<PFNGLDELETETEXTURESPROC>(LoadEntry("glDeleteTextures"));
			glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(LoadEntry("glDeleteVertexArrays"));
			glDepthFunc = reinterpret_cast<PFNGLDEPTHFUNCPROC>(LoadEntry("glDepthFunc"));
			glDepthMask = reinterpret_cast<PFNGLDEPTHMASKPROC>(LoadEntry("glDepthMask"));
			glDisable = reinterpret_cast<PFNGLDISABLEPROC>(LoadEntry("glDisable"));
			glDisableVertexAttribArray = reinterpret_cast<PFNGLDISABLEVERTEXATTRIBARRAYPROC>(LoadEntry("glDisableVertexAttribArray"));
			glDrawArrays = reinterpret_cast<PFNGLDRAWARRAYSPROC>(LoadEntry("glDrawArrays"));
			glDrawArraysInstanced = reinterpret_cast<PFNGLDRAWARRAYSINSTANCEDPROC>(LoadEntry("glDrawArraysInstanced"));
			glDrawBuffer = reinterpret_cast<PFNGLDRAWBUFFERPROC>(LoadEntry("glDrawBuffer"));
			glDrawBuffers = reinterpret_cast<PFNGLDRAWBUFFERSPROC>(LoadEntry("glDrawBuffers"));
			glDrawElements = reinterpret_cast<PFNGLDRAWELEMENTSPROC>(LoadEntry("glDrawElements"));
			glDrawElementsInstanced = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDPROC>(LoadEntry("glDrawElementsInstanced"));
			glEnable = reinterpret_cast<PFNGLENABLEPROC>(LoadEntry("glEnable"));
			glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(LoadEntry("glEnableVertexAttribArray"));
			glEndConditionalRender = reinterpret_cast<PFNGLENDCONDITIONALRENDERPROC>(LoadEntry("glEndConditionalRender"));
			glEndQuery = reinterpret_cast<PFNGLENDQUERYPROC>(LoadEntry("glEndQuery"));
			glFlush = reinterpret_cast<PFNGLFLUSHPROC>(LoadEntry("glFlush"));
			glFramebufferRenderbuffer = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(LoadEntry("glFramebufferRenderbuffer"));
			glFramebufferTexture = reinterpret_cast<PFNGLFRAMEBUFFERTEXTUREPROC>(LoadEntry("glFramebufferTexture"));
			glFramebufferTexture1D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE1DPROC>(LoadEntry("glFramebufferTexture1D"));
			glFramebufferTexture2D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(LoadEntry("glFramebufferTexture2D"));
			glFramebufferTexture3D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE3DPROC>(LoadEntry("glFramebufferTexture3D"));
			glFramebufferTextureLayer = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURELAYERPROC>(LoadEntry("glFramebufferTextureLayer"));
			glGenerateMipmap = reinterpret_cast<PFNGLGENERATEMIPMAPPROC>(LoadEntry("glGenerateMipmap"));
			glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(LoadEntry("glGenBuffers"));
			glGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(LoadEntry("glGenFramebuffers"));
			glGenQueries = reinterpret_cast<PFNGLGENQUERIESPROC>(LoadEntry("glGenQueries"));
			glGenRenderbuffers = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(LoadEntry("glGenRenderbuffers"));
			glGenSamplers = reinterpret_cast<PFNGLGENSAMPLERSPROC>(LoadEntry("glGenSamplers"));
			glGenTextures = reinterpret_cast<PFNGLGENTEXTURESPROC>(LoadEntry("glGenTextures"));
			glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(LoadEntry("glGenVertexArrays"));
			glGetActiveUniform = reinterpret_cast<PFNGLGETACTIVEUNIFORMPROC>(LoadEntry("glGetActiveUniform"));
			glGetBooleanv = reinterpret_cast<PFNGLGETBOOLEANVPROC>(LoadEntry("glGetBooleanv"));
			glGetBufferParameteriv = reinterpret_cast<PFNGLGETBUFFERPARAMETERIVPROC>(LoadEntry("glGetBufferParameteriv"));
			glGetError = reinterpret_cast<PFNGLGETERRORPROC>(LoadEntry("glGetError"));
			glGetFloatv = reinterpret_cast<PFNGLGETFLOATVPROC>(LoadEntry("glGetFloatv"));
			glGetIntegerv = reinterpret_cast<PFNGLGETINTEGERVPROC>(LoadEntry("glGetIntegerv"));
			glGetQueryiv = reinterpret_cast<PFNGLGETQUERYIVPROC>(LoadEntry("glGetQueryiv"));
			glGetQueryObjectiv = reinterpret_cast<PFNGLGETQUERYOBJECTIVPROC>(LoadEntry("glGetQueryObjectiv"));
			glGetQueryObjectuiv = reinterpret_cast<PFNGLGETQUERYOBJECTUIVPROC>(LoadEntry("glGetQueryObjectuiv"));
			glGetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(LoadEntry("glGetProgramiv"));
			glGetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(LoadEntry("glGetProgramInfoLog"));
			glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(LoadEntry("glGetShaderInfoLog"));
			glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(LoadEntry("glGetShaderiv"));
			glGetShaderSource = reinterpret_cast<PFNGLGETSHADERSOURCEPROC>(LoadEntry("glGetShaderSource"));
			glGetStringi = reinterpret_cast<PFNGLGETSTRINGIPROC>(LoadEntry("glGetStringi"));
			glGetTexImage = reinterpret_cast<PFNGLGETTEXIMAGEPROC>(LoadEntry("glGetTexImage"));
			glGetTexLevelParameterfv = reinterpret_cast<PFNGLGETTEXLEVELPARAMETERFVPROC>(LoadEntry("glGetTexLevelParameterfv"));
			glGetTexLevelParameteriv = reinterpret_cast<PFNGLGETTEXLEVELPARAMETERIVPROC>(LoadEntry("glGetTexLevelParameteriv"));
			glGetTexParameterfv = reinterpret_cast<PFNGLGETTEXPARAMETERFVPROC>(LoadEntry("glGetTexParameterfv"));
			glGetTexParameteriv = reinterpret_cast<PFNGLGETTEXPARAMETERIVPROC>(LoadEntry("glGetTexParameteriv"));
			glGetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(LoadEntry("glGetUniformLocation"));
			glIsEnabled = reinterpret_cast<PFNGLISENABLEDPROC>(LoadEntry("glIsEnabled"));
			glLineWidth = reinterpret_cast<PFNGLLINEWIDTHPROC>(LoadEntry("glLineWidth"));
			glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(LoadEntry("glLinkProgram"));
			glMapBuffer = reinterpret_cast<PFNGLMAPBUFFERPROC>(LoadEntry("glMapBuffer"));
			glMapBufferRange = reinterpret_cast<PFNGLMAPBUFFERRANGEPROC>(LoadEntry("glMapBufferRange"));
			glPixelStorei = reinterpret_cast<PFNGLPIXELSTOREIPROC>(LoadEntry("glPixelStorei"));
			glPointSize = reinterpret_cast<PFNGLPOINTSIZEPROC>(LoadEntry("glPointSize"));
			glPolygonMode = reinterpret_cast<PFNGLPOLYGONMODEPROC>(LoadEntry("glPolygonMode"));
			glReadPixels = reinterpret_cast<PFNGLREADPIXELSPROC>(LoadEntry("glReadPixels"));
			glRenderbufferStorage = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(LoadEntry("glRenderbufferStorage"));
			glSamplerParameterf = reinterpret_cast<PFNGLSAMPLERPARAMETERFPROC>(LoadEntry("glSamplerParameterf"));
			glSamplerParameteri = reinterpret_cast<PFNGLSAMPLERPARAMETERIPROC>(LoadEntry("glSamplerParameteri"));
			glScissor = reinterpret_cast<PFNGLSCISSORPROC>(LoadEntry("glScissor"));
			glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(LoadEntry("glShaderSource"));
			glStencilFunc = reinterpret_cast<PFNGLSTENCILFUNCPROC>(LoadEntry("glStencilFunc"));
			glStencilFuncSeparate = reinterpret_cast<PFNGLSTENCILFUNCSEPARATEPROC>(LoadEntry("glStencilFuncSeparate"));
			glStencilOp = reinterpret_cast<PFNGLSTENCILOPPROC>(LoadEntry("glStencilOp"));
			glStencilOpSeparate = reinterpret_cast<PFNGLSTENCILOPSEPARATEPROC>(LoadEntry("glStencilOpSeparate"));
			glTexImage2D = reinterpret_cast<PFNGLTEXIMAGE2DPROC>(LoadEntry("glTexImage2D"));
			glTexImage3D = reinterpret_cast<PFNGLTEXIMAGE3DPROC>(LoadEntry("glTexImage3D"));
			glTexParameterf = reinterpret_cast<PFNGLTEXPARAMETERFPROC>(LoadEntry("glTexParameterf"));
			glTexParameteri = reinterpret_cast<PFNGLTEXPARAMETERIPROC>(LoadEntry("glTexParameteri"));
			glTexSubImage1D = reinterpret_cast<PFNGLTEXSUBIMAGE1DPROC>(LoadEntry("glTexSubImage1D"));
			glTexSubImage2D = reinterpret_cast<PFNGLTEXSUBIMAGE2DPROC>(LoadEntry("glTexSubImage2D"));
			glTexSubImage3D = reinterpret_cast<PFNGLTEXSUBIMAGE3DPROC>(LoadEntry("glTexSubImage3D"));
			glUniform1f = reinterpret_cast<PFNGLUNIFORM1FPROC>(LoadEntry("glUniform1f"));
			glUniform1i = reinterpret_cast<PFNGLUNIFORM1IPROC>(LoadEntry("glUniform1i"));
			glUniform1fv = reinterpret_cast<PFNGLUNIFORM1FVPROC>(LoadEntry("glUniform1fv"));
			glUniform1iv = reinterpret_cast<PFNGLUNIFORM1IVPROC>(LoadEntry("glUniform1iv"));
			glUniform2fv = reinterpret_cast<PFNGLUNIFORM2FVPROC>(LoadEntry("glUniform2fv"));
			glUniform2iv = reinterpret_cast<PFNGLUNIFORM2IVPROC>(LoadEntry("glUniform2iv"));
			glUniform3fv = reinterpret_cast<PFNGLUNIFORM3FVPROC>(LoadEntry("glUniform3fv"));
			glUniform3iv = reinterpret_cast<PFNGLUNIFORM3IVPROC>(LoadEntry("glUniform3iv"));
			glUniform4fv = reinterpret_cast<PFNGLUNIFORM4FVPROC>(LoadEntry("glUniform4fv"));
			glUniform4iv = reinterpret_cast<PFNGLUNIFORM4IVPROC>(LoadEntry("glUniform4iv"));
			glUniformMatrix4fv = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(LoadEntry("glUniformMatrix4fv"));
			glUnmapBuffer = reinterpret_cast<PFNGLUNMAPBUFFERPROC>(LoadEntry("glUnmapBuffer"));
			glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(LoadEntry("glUseProgram"));
			glValidateProgram = reinterpret_cast<PFNGLVALIDATEPROGRAMPROC>(LoadEntry("glValidateProgram"));
			glVertexAttrib4f = reinterpret_cast<PFNGLVERTEXATTRIB4FPROC>(LoadEntry("glVertexAttrib4f"));
			glVertexAttribDivisor = reinterpret_cast<PFNGLVERTEXATTRIBDIVISORPROC>(LoadEntry("glVertexAttribDivisor"));
			glVertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(LoadEntry("glVertexAttribPointer"));
			glVertexAttribIPointer = reinterpret_cast<PFNGLVERTEXATTRIBIPOINTERPROC>(LoadEntry("glVertexAttribIPointer"));
			glViewport = reinterpret_cast<PFNGLVIEWPORTPROC>(LoadEntry("glViewport"));
		}
		catch (const std::exception& e)
		{
			NazaraError("Unable to load OpenGL: " + String(e.what()));
			return false;
		}

		/****************************************Extensions****************************************/

		// Fonctions optionnelles
		glBindFragDataLocation = reinterpret_cast<PFNGLBINDFRAGDATALOCATIONPROC>(LoadEntry("glBindFragDataLocation"));

		glDrawTexture = reinterpret_cast<PFNGLDRAWTEXTURENVPROC>(LoadEntry("glDrawTextureNV", false));
		glInvalidateBufferData = reinterpret_cast<PFNGLINVALIDATEBUFFERDATAPROC>(LoadEntry("glInvalidateBufferData", false));
		glVertexAttribLPointer = reinterpret_cast<PFNGLVERTEXATTRIBLPOINTERPROC>(LoadEntry("glVertexAttribLPointer", false));

		#if defined(NAZARA_PLATFORM_WINDOWS)
		wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(LoadEntry("wglGetExtensionsStringARB", false));
		wglGetExtensionsStringEXT = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(LoadEntry("wglGetExtensionsStringEXT", false));
		wglSwapInterval = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(LoadEntry("wglSwapIntervalEXT", false));
		#elif defined(NAZARA_PLATFORM_GLX)
		glXSwapIntervalEXT = reinterpret_cast<GLX::PFNGLXSWAPINTERVALEXTPROC>(LoadEntry("glXSwapIntervalEXT", false));
		NzglXSwapIntervalMESA = reinterpret_cast<GLX::PFNGLXSWAPINTERVALMESAPROC>(LoadEntry("glXSwapIntervalMESA", false));
		glXSwapIntervalSGI = reinterpret_cast<GLX::PFNGLXSWAPINTERVALSGIPROC>(LoadEntry("glXSwapIntervalSGI", false));
		#endif

		if (!glGetStringi || !LoadExtensions3())
		{
			NazaraWarning("Failed to load OpenGL 3 extension system, falling back to OpenGL 2 extension system...");

			if (!LoadExtensionsString(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))))
				NazaraWarning("Failed to load extension system");
		}

		#ifdef NAZARA_PLATFORM_WINDOWS
		{
			bool loaded;
			if (wglGetExtensionsStringARB)
				loaded = LoadExtensionsString(reinterpret_cast<const char*>(wglGetExtensionsStringARB(wglGetCurrentDC())));
			else if (wglGetExtensionsStringEXT)
				loaded = LoadExtensionsString(reinterpret_cast<const char*>(wglGetExtensionsStringEXT()));
			else
				loaded = false;

			if (!loaded)
				NazaraWarning("Failed to load wgl extension string");
		}
		#endif

		// AnisotropicFilter
		s_openGLextensions[OpenGLExtension_AnisotropicFilter] = IsSupported("GL_EXT_texture_filter_anisotropic");

		// DebugOutput
		if (s_openglVersion >= 430 || IsSupported("GL_KHR_debug"))
		{
			try
			{
				glDebugMessageCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(LoadEntry("glDebugMessageCallback"));
				glDebugMessageControl = reinterpret_cast<PFNGLDEBUGMESSAGECONTROLPROC>(LoadEntry("glDebugMessageControl"));
				glDebugMessageInsert = reinterpret_cast<PFNGLDEBUGMESSAGEINSERTPROC>(LoadEntry("glDebugMessageInsert"));
				glGetDebugMessageLog = reinterpret_cast<PFNGLGETDEBUGMESSAGELOGPROC>(LoadEntry("glGetDebugMessageLog"));

				s_openGLextensions[OpenGLExtension_DebugOutput] = true;
			}
			catch (const std::exception& e)
			{
				NazaraWarning("Failed to load GL_KHR_debug: " + String(e.what()));
			}
		}

		if (!s_openGLextensions[OpenGLExtension_DebugOutput] && IsSupported("GL_ARB_debug_output"))
		{
			try
			{
				glDebugMessageCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKARBPROC>(LoadEntry("glDebugMessageCallbackARB"));
				glDebugMessageControl = reinterpret_cast<PFNGLDEBUGMESSAGECONTROLARBPROC>(LoadEntry("glDebugMessageControlARB"));
				glDebugMessageInsert = reinterpret_cast<PFNGLDEBUGMESSAGEINSERTARBPROC>(LoadEntry("glDebugMessageInsertARB"));
				glGetDebugMessageLog = reinterpret_cast<PFNGLGETDEBUGMESSAGELOGARBPROC>(LoadEntry("glGetDebugMessageLogARB"));

				s_openGLextensions[OpenGLExtension_DebugOutput] = true;
			}
			catch (const std::exception& e)
			{
				NazaraWarning("Failed to load GL_ARB_debug_output: " + String(e.what()));
			}
		}

		// FP64
		if (s_openglVersion >= 400 || IsSupported("GL_ARB_gpu_shader_fp64"))
		{
			try
			{
				glUniform1d = reinterpret_cast<PFNGLUNIFORM1DPROC>(LoadEntry("glUniform1d"));
				glUniform1dv = reinterpret_cast<PFNGLUNIFORM1DVPROC>(LoadEntry("glUniform1dv"));
				glUniform2dv = reinterpret_cast<PFNGLUNIFORM2DVPROC>(LoadEntry("glUniform2dv"));
				glUniform3dv = reinterpret_cast<PFNGLUNIFORM3DVPROC>(LoadEntry("glUniform3dv"));
				glUniform4dv = reinterpret_cast<PFNGLUNIFORM4DVPROC>(LoadEntry("glUniform4dv"));

				s_openGLextensions[OpenGLExtension_FP64] = true;
			}
			catch (const std::exception& e)
			{
				NazaraWarning("Failed to load ARB_gpu_shader_fp64: " + String(e.what()));
			}
		}

		// GetProgramBinary
		if (s_openglVersion >= 410 || IsSupported("GL_ARB_get_program_binary"))
		{
			try
			{
				glGetProgramBinary = reinterpret_cast<PFNGLGETPROGRAMBINARYPROC>(LoadEntry("glGetProgramBinary"));
				glProgramBinary = reinterpret_cast<PFNGLPROGRAMBINARYPROC>(LoadEntry("glProgramBinary"));
				glProgramParameteri = reinterpret_cast<PFNGLPROGRAMPARAMETERIPROC>(LoadEntry("glProgramParameteri"));

				s_openGLextensions[OpenGLExtension_GetProgramBinary] = true;
			}
			catch (const std::exception& e)
			{
				NazaraWarning("Failed to load ARB_get_program_binary: (" + String(e.what()) + ")");
			}
		}

		// SeparateShaderObjects
		if (s_openglVersion >= 400 || IsSupported("GL_ARB_separate_shader_objects"))
		{
			try
			{
				glProgramUniform1f = reinterpret_cast<PFNGLPROGRAMUNIFORM1FPROC>(LoadEntry("glProgramUniform1f"));
				glProgramUniform1i = reinterpret_cast<PFNGLPROGRAMUNIFORM1IPROC>(LoadEntry("glProgramUniform1i"));
				glProgramUniform1fv = reinterpret_cast<PFNGLPROGRAMUNIFORM1FVPROC>(LoadEntry("glProgramUniform1fv"));
				glProgramUniform1iv = reinterpret_cast<PFNGLPROGRAMUNIFORM1IVPROC>(LoadEntry("glProgramUniform1iv"));
				glProgramUniform2fv = reinterpret_cast<PFNGLPROGRAMUNIFORM2FVPROC>(LoadEntry("glProgramUniform2fv"));
				glProgramUniform2iv = reinterpret_cast<PFNGLPROGRAMUNIFORM2IVPROC>(LoadEntry("glProgramUniform2iv"));
				glProgramUniform3fv = reinterpret_cast<PFNGLPROGRAMUNIFORM3FVPROC>(LoadEntry("glProgramUniform3fv"));
				glProgramUniform3iv = reinterpret_cast<PFNGLPROGRAMUNIFORM3IVPROC>(LoadEntry("glProgramUniform3iv"));
				glProgramUniform4fv = reinterpret_cast<PFNGLPROGRAMUNIFORM4FVPROC>(LoadEntry("glProgramUniform4fv"));
				glProgramUniform4iv = reinterpret_cast<PFNGLPROGRAMUNIFORM4IVPROC>(LoadEntry("glProgramUniform4iv"));
				glProgramUniformMatrix4fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4FVPROC>(LoadEntry("glProgramUniformMatrix4fv"));

				// Si ARB_gpu_shader_fp64 est supporté, alors cette extension donne également accès aux fonctions utilisant des double
				if (s_openGLextensions[OpenGLExtension_FP64])
				{
					glProgramUniform1d = reinterpret_cast<PFNGLPROGRAMUNIFORM1DPROC>(LoadEntry("glProgramUniform1d"));
					glProgramUniform1dv = reinterpret_cast<PFNGLPROGRAMUNIFORM2DVPROC>(LoadEntry("glProgramUniform1dv"));
					glProgramUniform2dv = reinterpret_cast<PFNGLPROGRAMUNIFORM2DVPROC>(LoadEntry("glProgramUniform2dv"));
					glProgramUniform3dv = reinterpret_cast<PFNGLPROGRAMUNIFORM3DVPROC>(LoadEntry("glProgramUniform3dv"));
					glProgramUniform4dv = reinterpret_cast<PFNGLPROGRAMUNIFORM4DVPROC>(LoadEntry("glProgramUniform4dv"));
					glProgramUniformMatrix4dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4DVPROC>(LoadEntry("glProgramUniformMatrix4dv"));
				}

				s_openGLextensions[OpenGLExtension_SeparateShaderObjects] = true;
			}
			catch (const std::exception& e)
			{
				NazaraWarning("Failed to load ARB_separate_shader_objects: (" + String(e.what()) + ")");
			}
		}

		// Shader_ImageLoadStore
		s_openGLextensions[OpenGLExtension_Shader_ImageLoadStore] = (s_openglVersion >= 420 || IsSupported("GL_ARB_shader_image_load_store"));

		// TextureCompression_s3tc
		s_openGLextensions[OpenGLExtension_TextureCompression_s3tc] = IsSupported("GL_EXT_texture_compression_s3tc");

		// TextureStorage
		if (s_openglVersion >= 420 || IsSupported("GL_ARB_texture_storage"))
		{
			try
			{
				glTexStorage1D = reinterpret_cast<PFNGLTEXSTORAGE1DPROC>(LoadEntry("glTexStorage1D"));
				glTexStorage2D = reinterpret_cast<PFNGLTEXSTORAGE2DPROC>(LoadEntry("glTexStorage2D"));
				glTexStorage3D = reinterpret_cast<PFNGLTEXSTORAGE3DPROC>(LoadEntry("glTexStorage3D"));

				s_openGLextensions[OpenGLExtension_TextureStorage] = true;
			}
			catch (const std::exception& e)
			{
				NazaraWarning("Failed to load ARB_texture_storage: " + String(e.what()));
			}
		}

		/******************************Initialisation*****************************/

		s_contextStates = nullptr;
		s_rendererName = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		s_vendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

		// On initialise les vrais contextes OpenGL
		if (!Context::Initialize())
		{
			NazaraError("Failed to initialize contexts");
			return false;
		}

		// Le contexte OpenGL n'est plus assuré à partir d'ici
		onExit.Reset();

		return true;
	}

	bool OpenGL::IsInitialized()
	{
		return s_initialized;
	}

	bool OpenGL::IsSupported(OpenGLExtension extension)
	{
		return s_openGLextensions[extension];
	}

	bool OpenGL::IsSupported(const String& string)
	{
		return s_openGLextensionSet.find(string) != s_openGLextensionSet.end();
	}

	void OpenGL::SetBuffer(BufferType type, GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		s_contextStates->buffersBinding[type] = id;
	}

	void OpenGL::SetScissorBox(const Recti& scissorBox)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		s_contextStates->currentScissorBox = scissorBox;
	}

	void OpenGL::SetProgram(GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		s_contextStates->currentProgram = id;
	}

	void OpenGL::SetTarget(const RenderTarget* renderTarget)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		s_contextStates->currentTarget = renderTarget;
		if (renderTarget)
		{
			if (!s_contextStates->scissorBoxUpdated)
			{
				const Recti& scissorBox = s_contextStates->currentViewport;

				unsigned int height = s_contextStates->currentTarget->GetHeight();
				glScissor(scissorBox.x, height - scissorBox.height - scissorBox.y, scissorBox.width, scissorBox.height);

				s_contextStates->scissorBoxUpdated = true;
			}

			if (!s_contextStates->viewportUpdated)
			{
				const Recti& viewport = s_contextStates->currentViewport;

				unsigned int height = s_contextStates->currentTarget->GetHeight();
				glViewport(viewport.x, height - viewport.height - viewport.y, viewport.width, viewport.height);

				s_contextStates->viewportUpdated = true;
			}
		}
	}

	void OpenGL::SetTexture(GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		s_contextStates->texturesBinding[s_contextStates->textureUnit] = id;
	}

	void OpenGL::SetTexture(unsigned int textureUnit, GLuint id)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		s_contextStates->texturesBinding[textureUnit] = id;
	}

	void OpenGL::SetTextureUnit(unsigned int textureUnit)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		s_contextStates->textureUnit = textureUnit;
	}

	void OpenGL::SetViewport(const Recti& viewport)
	{
		#ifdef NAZARA_DEBUG
		if (!s_contextStates)
		{
			NazaraError("No context activated");
			return;
		}
		#endif

		s_contextStates->currentViewport = viewport;
	}

	bool OpenGL::TranslateFormat(PixelFormatType pixelFormat, Format* format, FormatType type)
	{
		// Par défaut
		format->swizzle[0] = GL_RED;
		format->swizzle[1] = GL_GREEN;
		format->swizzle[2] = GL_BLUE;
		format->swizzle[3] = GL_ALPHA;

		switch (pixelFormat)
		{
			case PixelFormatType_A8:
				if (type == FormatType_Texture) // Format supporté uniquement par les textures
				{
					if (GetVersion() >= 300)
					{
						format->dataFormat = GL_RED;
						format->dataType = GL_UNSIGNED_BYTE;
						format->internalFormat = GL_R8;

						// Simulation du format
						format->swizzle[0] = GL_ONE;
						format->swizzle[1] = GL_ONE;
						format->swizzle[2] = GL_ONE;
						format->swizzle[3] = GL_RED;
					}
					else
					{
						// Le bon vieux format GL_ALPHA
						format->dataFormat = GL_ALPHA;
						format->dataType = GL_UNSIGNED_BYTE;
						format->internalFormat = GL_ALPHA;
					}

					return true;
				}
				else
					return false;

			case PixelFormatType_BGR8:
				format->dataFormat = GL_BGR;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_RGB8;
				return true;

			case PixelFormatType_BGRA8:
				format->dataFormat = GL_BGRA;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_RGBA8;
				return true;

			case PixelFormatType_DXT1:
				format->dataFormat = GL_RGB;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
				return true;

			case PixelFormatType_DXT3:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				return true;

			case PixelFormatType_DXT5:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				return true;

			case PixelFormatType_L8:
				if (type == FormatType_Texture) // Format supporté uniquement par les textures
				{
					if (GetVersion() >= 300)
					{
						format->dataFormat = GL_RED;
						format->dataType = GL_UNSIGNED_BYTE;
						format->internalFormat = GL_R8;

						// Simulation du format
						format->swizzle[0] = GL_RED;
						format->swizzle[1] = GL_RED;
						format->swizzle[2] = GL_RED;
						format->swizzle[3] = GL_ONE;
					}
					else
					{
						format->dataFormat = 0x1909; // GL_LUMINANCE
						format->dataType = GL_UNSIGNED_BYTE;
						format->internalFormat = 0x1909; // GL_LUMINANCE
					}

					return true;
				}
				else
					return false;

			case PixelFormatType_LA8:
				if (type == FormatType_Texture) // Format supporté uniquement par les textures
				{
					if (GetVersion() >= 300)
					{
						format->dataFormat = GL_RG;
						format->dataType = GL_UNSIGNED_BYTE;
						format->internalFormat = GL_RG8;

						// Simulation du format
						format->swizzle[0] = GL_RED;
						format->swizzle[1] = GL_RED;
						format->swizzle[2] = GL_RED;
						format->swizzle[3] = GL_GREEN;
					}
					else
					{
						format->dataFormat = 0x190A; // GL_LUMINANCE_ALPHA
						format->dataType = GL_UNSIGNED_BYTE;
						format->internalFormat = 0x190A; // GL_LUMINANCE_ALPHA;
					}

					return true;
				}
				else
					return false;

			case PixelFormatType_R8:
				format->dataFormat = GL_RED;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_R8;
				return true;

			case PixelFormatType_R8I:
				format->dataFormat = GL_RED;
				format->dataType = GL_BYTE;
				format->internalFormat = GL_R8I;
				return true;

			case PixelFormatType_R8UI:
				format->dataFormat = GL_RED;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_R8UI;
				return true;

			case PixelFormatType_R16:
				format->dataFormat = GL_RED;
				format->dataType = GL_UNSIGNED_SHORT;
				format->internalFormat = GL_R16;
				return true;

			case PixelFormatType_R16F:
				format->dataFormat = GL_RED;
				format->dataType = GL_HALF_FLOAT;
				format->internalFormat = GL_R16F;
				return true;

			case PixelFormatType_R16I:
				format->dataFormat = GL_RED;
				format->dataType = GL_SHORT;
				format->internalFormat = GL_R16I;
				return true;

			case PixelFormatType_R16UI:
				format->dataFormat = GL_RED;
				format->dataType = GL_UNSIGNED_SHORT;
				format->internalFormat = GL_R16UI;
				return true;

			case PixelFormatType_R32F:
				format->dataFormat = GL_RED;
				format->dataType = GL_FLOAT;
				format->internalFormat = GL_R32F;
				return true;

			case PixelFormatType_R32I:
				format->dataFormat = GL_RED;
				format->dataType = GL_INT;
				format->internalFormat = GL_R32I;
				return true;

			case PixelFormatType_R32UI:
				format->dataFormat = GL_RED;
				format->dataType = GL_UNSIGNED_INT;
				format->internalFormat = GL_R32UI;
				return true;

			case PixelFormatType_RG8:
				format->dataFormat = GL_RG;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_RG8;
				return true;

			case PixelFormatType_RG8I:
				format->dataFormat = GL_RG;
				format->dataType = GL_BYTE;
				format->internalFormat = GL_RG8I;
				return true;

			case PixelFormatType_RG8UI:
				format->dataFormat = GL_RG;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_RG8UI;
				return true;

			case PixelFormatType_RG16:
				format->dataFormat = GL_RG;
				format->dataType = GL_UNSIGNED_SHORT;
				format->internalFormat = GL_RG16;
				return true;

			case PixelFormatType_RG16F:
				format->dataFormat = GL_RG;
				format->dataType = GL_HALF_FLOAT;
				format->internalFormat = GL_RG16F;
				return true;

			case PixelFormatType_RG16I:
				format->dataFormat = GL_RG;
				format->dataType = GL_SHORT;
				format->internalFormat = GL_RG16I;
				return true;

			case PixelFormatType_RG16UI:
				format->dataFormat = GL_RG;
				format->dataType = GL_UNSIGNED_SHORT;
				format->internalFormat = GL_RG16UI;
				return true;

			case PixelFormatType_RG32F:
				format->dataFormat = GL_RG;
				format->dataType = GL_FLOAT;
				format->internalFormat = GL_RG32F;
				return true;

			case PixelFormatType_RG32I:
				format->dataFormat = GL_RG;
				format->dataType = GL_INT;
				format->internalFormat = GL_RG32I;
				return true;

			case PixelFormatType_RG32UI:
				format->dataFormat = GL_RG;
				format->dataType = GL_UNSIGNED_INT;
				format->internalFormat = GL_RG32UI;
				return true;

			case PixelFormatType_RGB5A1:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_UNSIGNED_SHORT_5_5_5_1;
				format->internalFormat = GL_RGB5_A1;
				return true;

			case PixelFormatType_RGB8:
				format->dataFormat = GL_RGB;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_RGB8;
				return true;

			case PixelFormatType_RGB16F:
				format->dataFormat = GL_RGB;
				format->dataType = GL_HALF_FLOAT;
				format->internalFormat = GL_RGB16F;
				return true;

			case PixelFormatType_RGB16I:
				format->dataFormat = GL_RGB;
				format->dataType = GL_SHORT;
				format->internalFormat = GL_RGB16I;
				return true;

			case PixelFormatType_RGB16UI:
				format->dataFormat = GL_RGB;
				format->dataType = GL_UNSIGNED_SHORT;
				format->internalFormat = GL_RGB16UI;
				return true;

			case PixelFormatType_RGB32F:
				format->dataFormat = GL_RGB;
				format->dataType = GL_FLOAT;
				format->internalFormat = GL_RGB32F;
				return true;

			case PixelFormatType_RGB32I:
				format->dataFormat = GL_RGB;
				format->dataType = GL_INT;
				format->internalFormat = GL_RGB32I;
				return true;

			case PixelFormatType_RGB32UI:
				format->dataFormat = GL_RGB;
				format->dataType = GL_UNSIGNED_INT;
				format->internalFormat = GL_RGB32UI;
				return true;

			case PixelFormatType_RGBA4:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_UNSIGNED_SHORT_4_4_4_4;
				format->internalFormat = GL_RGBA4;
				return true;

			case PixelFormatType_RGBA8:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_RGBA8;
				return true;

			case PixelFormatType_RGBA16F:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_HALF_FLOAT;
				format->internalFormat = GL_RGBA16F;
				return true;

			case PixelFormatType_RGBA16I:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_SHORT;
				format->internalFormat = GL_RGBA16I;
				return true;

			case PixelFormatType_RGBA16UI:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_INT;
				format->internalFormat = GL_RGBA16UI;
				return true;

			case PixelFormatType_RGBA32F:
				format->dataFormat = GL_RGBA;
				format->dataType = GL_FLOAT;
				format->internalFormat = GL_RGBA32F;
				return true;

			case PixelFormatType_RGBA32I:
				format->dataFormat = GL_RGB;
				format->dataType = GL_INT;
				format->internalFormat = GL_RGB32I;
				return true;

			case PixelFormatType_RGBA32UI:
				format->dataFormat = GL_RGB;
				format->dataType = GL_UNSIGNED_INT;
				format->internalFormat = GL_RGB32UI;
				return true;

			case PixelFormatType_Depth16:
				format->dataFormat = GL_DEPTH_COMPONENT;
				format->dataType = GL_UNSIGNED_SHORT;
				format->internalFormat = GL_DEPTH_COMPONENT16;

				format->swizzle[0] = GL_RED;
				format->swizzle[1] = GL_RED;
				format->swizzle[2] = GL_RED;
				format->swizzle[3] = GL_ONE;
				return true;

			case PixelFormatType_Depth24:
				format->dataFormat = GL_DEPTH_COMPONENT;
				format->dataType = GL_UNSIGNED_INT;
				format->internalFormat = GL_DEPTH_COMPONENT24;

				format->swizzle[0] = GL_RED;
				format->swizzle[1] = GL_RED;
				format->swizzle[2] = GL_RED;
				format->swizzle[3] = GL_ONE;
				return true;

			case PixelFormatType_Depth24Stencil8:
				format->dataFormat = GL_DEPTH_STENCIL;
				format->dataType = GL_UNSIGNED_INT_24_8;
				format->internalFormat = GL_DEPTH24_STENCIL8;

				format->swizzle[0] = GL_RED;
				format->swizzle[1] = GL_RED;
				format->swizzle[2] = GL_RED;
				format->swizzle[3] = GL_GREEN;
				return true;

			case PixelFormatType_Depth32:
				format->dataFormat = GL_DEPTH_COMPONENT;
				format->dataType = GL_UNSIGNED_BYTE;
				format->internalFormat = GL_DEPTH_COMPONENT32;

				format->swizzle[0] = GL_RED;
				format->swizzle[1] = GL_RED;
				format->swizzle[2] = GL_RED;
				format->swizzle[3] = GL_ONE;
				return true;

			case PixelFormatType_Stencil1:
				if (type == FormatType_Texture) // Les formats de stencil ne sont pas supportés par les textures
					return false;
				else
				{
					format->dataFormat = GL_NONE;
					format->dataType = GL_NONE;
					format->internalFormat = GL_STENCIL_INDEX1;
					return true;
				}

			case PixelFormatType_Stencil4:
				if (type == FormatType_Texture)
					return false;
				else
				{
					format->dataFormat = GL_NONE;
					format->dataType = GL_NONE;
					format->internalFormat = GL_STENCIL_INDEX4;
					return true;
				}

			case PixelFormatType_Stencil8:
				if (type == FormatType_Texture)
					return false;
				else
				{
					format->dataFormat = GL_NONE;
					format->dataType = GL_NONE;
					format->internalFormat = GL_STENCIL_INDEX8;
					return true;
				}

			case PixelFormatType_Stencil16:
				if (type == FormatType_Texture)
					return false;
				else
				{
					format->dataFormat = GL_NONE;
					format->dataType = GL_NONE;
					format->internalFormat = GL_STENCIL_INDEX16;
					return true;
				}

			case PixelFormatType_Undefined:
				break;
		}

		NazaraError("Invalid pixel format");
		return false;
	}

	void OpenGL::Uninitialize()
	{
		if (s_initialized)
		{
			s_initialized = false;

			Context::Uninitialize();

			for (bool& ext : s_openGLextensions)
				ext = false;

			s_glslVersion = 0;
			s_openGLextensionSet.clear();
			s_openglVersion = 0;
			s_rendererName.Clear(false);
			s_vendorName.Clear(false);

			UnloadLibrary();
		}
	}

	void OpenGL::OnContextChanged(const Context* newContext)
	{
		s_contextStates = (newContext) ? &s_contexts[newContext] : nullptr;
		if (s_contextStates)
		{
			// On supprime les éventuelles ressources mortes-vivantes (Qui ne peuvent être libérées que dans notre contexte)
			for (std::pair<GarbageResourceType, GLuint>& pair : s_contextStates->garbage)
			{
				switch (pair.first)
				{
					case GarbageResourceType_FrameBuffer:
						glDeleteFramebuffers(1, &pair.second);
						break;

					case GarbageResourceType_VertexArray:
						glDeleteVertexArrays(1, &pair.second);
						break;
				}
			}
			s_contextStates->garbage.clear();
		}
	}

	void OpenGL::OnContextDestruction(const Context* context)
	{
		/*
		** Il serait possible d'activer le contexte avant sa destruction afin de libérer les éventuelles ressources mortes-vivantes,
		** mais un driver bien conçu va libérer ces ressources de lui-même.
		*/
		s_contexts.erase(context);
	}

	GLenum OpenGL::Attachment[] =
	{
		GL_COLOR_ATTACHMENT0,        // AttachmentPoint_Color
		GL_DEPTH_ATTACHMENT,         // AttachmentPoint_Depth
		GL_DEPTH_STENCIL_ATTACHMENT, // AttachmentPoint_DepthStencil
		GL_STENCIL_ATTACHMENT        // AttachmentPoint_Stencil
	};

	static_assert(AttachmentPoint_Max + 1 == 4, "Attachment array is incomplete");

	GLenum OpenGL::BlendFunc[] =
	{
		GL_DST_ALPHA,           // BlendFunc_DestAlpha
		GL_DST_COLOR,           // BlendFunc_DestColor
		GL_SRC_ALPHA,           // BlendFunc_SrcAlpha
		GL_SRC_COLOR,           // BlendFunc_SrcColor
		GL_ONE_MINUS_DST_ALPHA, // BlendFunc_InvDestAlpha
		GL_ONE_MINUS_DST_COLOR, // BlendFunc_InvDestColor
		GL_ONE_MINUS_SRC_ALPHA, // BlendFunc_InvSrcAlpha
		GL_ONE_MINUS_SRC_COLOR, // BlendFunc_InvSrcColor
		GL_ONE,                 // BlendFunc_One
		GL_ZERO                 // BlendFunc_Zero
	};

	static_assert(BlendFunc_Max + 1 == 10, "Blend func array is incomplete");

	GLenum OpenGL::BufferLock[] =
	{
		GL_WRITE_ONLY, // BufferAccess_DiscardAndWrite
		GL_READ_ONLY,  // BufferAccess_ReadOnly
		GL_READ_WRITE, // BufferAccess_ReadWrite
		GL_WRITE_ONLY  // BufferAccess_WriteOnly
	};

	static_assert(BufferAccess_Max + 1 == 4, "Buffer lock array is incomplete");

	GLenum OpenGL::BufferLockRange[] =
	{
		// http://www.opengl.org/discussion_boards/showthread.php/170118-VBOs-strangely-slow?p=1198118#post1198118
		GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT, // BufferAccess_DiscardAndWrite
		GL_MAP_READ_BIT,                                                               // BufferAccess_ReadOnly
		GL_MAP_READ_BIT | GL_MAP_WRITE_BIT,                                            // BufferAccess_ReadWrite
		GL_MAP_WRITE_BIT                                                               // BufferAccess_WriteOnly
	};

	static_assert(BufferAccess_Max + 1 == 4, "Buffer lock range array is incomplete");

	GLenum OpenGL::BufferTarget[] =
	{
		GL_ELEMENT_ARRAY_BUFFER, // BufferType_Index,
		GL_ARRAY_BUFFER,		 // BufferType_Vertex
	};

	static_assert(BufferType_Max + 1 == 2, "Buffer target array is incomplete");

	GLenum OpenGL::BufferTargetBinding[] =
	{
		GL_ELEMENT_ARRAY_BUFFER_BINDING, // BufferType_Index,
		GL_ARRAY_BUFFER_BINDING,		 // BufferType_Vertex
	};

	static_assert(BufferType_Max + 1 == 2, "Buffer target binding array is incomplete");

	GLenum OpenGL::ComponentType[] =
	{
		GL_UNSIGNED_BYTE, // ComponentType_Color
		GL_DOUBLE,        // ComponentType_Double1
		GL_DOUBLE,        // ComponentType_Double2
		GL_DOUBLE,        // ComponentType_Double3
		GL_DOUBLE,        // ComponentType_Double4
		GL_FLOAT,         // ComponentType_Float1
		GL_FLOAT,         // ComponentType_Float2
		GL_FLOAT,         // ComponentType_Float3
		GL_FLOAT,         // ComponentType_Float4
		GL_INT,           // ComponentType_Int1
		GL_INT,           // ComponentType_Int2
		GL_INT,           // ComponentType_Int3
		GL_INT,           // ComponentType_Int4
		GL_FLOAT          // ComponentType_Quaternion
	};

	static_assert(ComponentType_Max + 1 == 14, "Attribute type array is incomplete");

	GLenum OpenGL::CubemapFace[] =
	{
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, // CubemapFace_PositiveX
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // CubemapFace_NegativeX
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // CubemapFace_PositiveY
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // CubemapFace_NegativeY
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // CubemapFace_PositiveZ
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // CubemapFace_NegativeZ
	};

	static_assert(CubemapFace_Max + 1 == 6, "Cubemap face array is incomplete");

	GLenum OpenGL::FaceFilling[] =
	{
		GL_FILL,  // FaceFilling_Fill
		GL_LINE,  // FaceFilling_Line
		GL_POINT  // FaceFilling_Point
	};

	static_assert(FaceFilling_Max + 1 == 3, "Face filling array is incomplete");

	GLenum OpenGL::FaceSide[] =
	{
		GL_BACK,          // FaceSide_Back
		GL_FRONT,         // FaceSide_Front
		GL_FRONT_AND_BACK // FaceSide_FrontAndBack
	};

	static_assert(FaceSide_Max + 1 == 3, "Face side array is incomplete");

	GLenum OpenGL::PrimitiveMode[] =
	{
		GL_LINES,          // PrimitiveMode_LineList
		GL_LINE_STRIP,     // PrimitiveMode_LineStrip
		GL_POINTS,         // PrimitiveMode_PointList
		GL_TRIANGLES,      // PrimitiveMode_TriangleList
		GL_TRIANGLE_STRIP, // PrimitiveMode_TriangleStrip
		GL_TRIANGLE_FAN    // PrimitiveMode_TriangleFan
	};

	static_assert(PrimitiveMode_Max + 1 == 6, "Primitive mode array is incomplete");

	GLenum OpenGL::QueryCondition[] =
	{
		GL_QUERY_WAIT,              // GpuQueryCondition_NoWait
		GL_QUERY_BY_REGION_NO_WAIT, // GpuQueryCondition_Region_NoWait
		GL_QUERY_BY_REGION_WAIT,    // GpuQueryCondition_Region_Wait
		GL_QUERY_WAIT               // GpuQueryCondition_Wait
	};

	static_assert(GpuQueryCondition_Max + 1 == 4, "Query condition array is incomplete");

	GLenum OpenGL::QueryMode[] =
	{
		GL_ANY_SAMPLES_PASSED,                   // GpuQueryMode_AnySamplesPassed
		GL_ANY_SAMPLES_PASSED_CONSERVATIVE,      // GpuQueryMode_AnySamplesPassedConservative
		GL_PRIMITIVES_GENERATED,                 // GpuQueryMode_PrimitiveGenerated
		GL_SAMPLES_PASSED,                       // GpuQueryMode_SamplesPassed
		GL_TIME_ELAPSED,                         // GpuQueryMode_TimeElapsed
		GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN // GpuQueryMode_TransformFeedbackPrimitivesWritten
	};

	static_assert(GpuQueryMode_Max + 1 == 6, "Query mode array is incomplete");

	GLenum OpenGL::RendererComparison[] =
	{
		GL_ALWAYS,  // RendererComparison_Always
		GL_EQUAL,   // RendererComparison_Equal
		GL_GREATER, // RendererComparison_Greater
		GL_GEQUAL,  // RendererComparison_GreaterOrEqual
		GL_LESS,    // RendererComparison_Less
		GL_LEQUAL,  // RendererComparison_LessOrEqual
		GL_NEVER,   // RendererComparison_Never
		GL_NOTEQUAL // RendererComparison_NotEqual
	};

	static_assert(RendererComparison_Max + 1 == 8, "Renderer comparison array is incomplete");

	GLenum OpenGL::RendererParameter[] =
	{
		GL_BLEND,        // RendererParameter_Blend
		GL_NONE,         // RendererParameter_ColorWrite
		GL_DEPTH_TEST,   // RendererParameter_DepthBuffer
		GL_NONE,         // RendererParameter_DepthWrite
		GL_CULL_FACE,    // RendererParameter_FaceCulling
		GL_SCISSOR_TEST, // RendererParameter_ScissorTest
		GL_STENCIL_TEST  // RendererParameter_StencilTest
	};

	static_assert(RendererParameter_Max + 1 == 7, "Renderer parameter array is incomplete");

	GLenum OpenGL::SamplerWrapMode[] =
	{
		GL_CLAMP_TO_EDGE,   // TextureWrap_Clamp
		GL_MIRRORED_REPEAT, // SamplerWrap_MirroredRepeat
		GL_REPEAT           // TextureWrap_Repeat
	};

	static_assert(SamplerWrap_Max + 1 == 3, "Sampler wrap mode array is incomplete");

	GLenum OpenGL::ShaderStage[] =
	{
		GL_FRAGMENT_SHADER,	// ShaderStage_Fragment
		GL_GEOMETRY_SHADER,	// ShaderStage_Geometry
		GL_VERTEX_SHADER	// ShaderStage_Vertex
	};

	static_assert(ShaderStageType_Max + 1 == 3, "Shader stage array is incomplete");

	GLenum OpenGL::StencilOperation[] =
	{
		GL_DECR,      // StencilOperation_Decrement
		GL_DECR_WRAP, // StencilOperation_DecrementNoClamp
		GL_INCR,      // StencilOperation_Increment
		GL_INCR_WRAP, // StencilOperation_IncrementNoClamp
		GL_INVERT,    // StencilOperation_Invert
		GL_KEEP,      // StencilOperation_Keep
		GL_REPLACE,   // StencilOperation_Replace
		GL_ZERO       // StencilOperation_Zero
	};

	static_assert(StencilOperation_Max + 1 == 8, "Stencil operation array is incomplete");

	GLenum OpenGL::TextureTarget[] =
	{
		GL_TEXTURE_1D,       // ImageType_1D
		GL_TEXTURE_1D_ARRAY, // ImageType_1D_Array
		GL_TEXTURE_2D,       // ImageType_2D
		GL_TEXTURE_2D_ARRAY, // ImageType_2D_Array
		GL_TEXTURE_3D,       // ImageType_3D
		GL_TEXTURE_CUBE_MAP  // ImageType_Cubemap
	};

	static_assert(ImageType_Max + 1 == 6, "Texture target array is incomplete");

	GLenum OpenGL::TextureTargetBinding[] =
	{
		GL_TEXTURE_BINDING_1D,       // ImageType_1D
		GL_TEXTURE_BINDING_1D_ARRAY, // ImageType_1D_Array
		GL_TEXTURE_BINDING_2D,       // ImageType_2D
		GL_TEXTURE_BINDING_2D_ARRAY, // ImageType_2D_Array
		GL_TEXTURE_BINDING_3D,       // ImageType_3D
		GL_TEXTURE_BINDING_CUBE_MAP  // ImageType_Cubemap
	};

	static_assert(ImageType_Max + 1 == 6, "Texture target binding array is incomplete");

	GLenum OpenGL::TextureTargetProxy[] =
	{
		GL_PROXY_TEXTURE_1D,       // ImageType_1D
		GL_PROXY_TEXTURE_1D_ARRAY, // ImageType_1D_Array
		GL_PROXY_TEXTURE_2D,       // ImageType_2D
		GL_PROXY_TEXTURE_2D_ARRAY, // ImageType_2D_Array
		GL_PROXY_TEXTURE_3D,       // ImageType_3D
		GL_PROXY_TEXTURE_CUBE_MAP  // ImageType_Cubemap
	};

	static_assert(ImageType_Max + 1 == 6, "Texture target proxy array is incomplete");

	UInt8 OpenGL::VertexComponentIndex[] =
	{
		10, // VertexComponent_InstanceData0
		11, // VertexComponent_InstanceData1
		12, // VertexComponent_InstanceData2
		13, // VertexComponent_InstanceData3
		14, // VertexComponent_InstanceData4
		15, // VertexComponent_InstanceData5
		4,  // VertexComponent_Color
		2,  // VertexComponent_Normal
		0,  // VertexComponent_Position
		3,  // VertexComponent_Tangent
		1,  // VertexComponent_TexCoord
		5,  // VertexComponent_Userdata0
		6,  // VertexComponent_Userdata1
		7,  // VertexComponent_Userdata2
		8,  // VertexComponent_Userdata3
		9   // VertexComponent_Userdata4
	};

	static_assert(VertexComponent_Max + 1 == 16, "Attribute index array is incomplete");

PFNGLACTIVETEXTUREPROC            glActiveTexture            = nullptr;
PFNGLATTACHSHADERPROC             glAttachShader             = nullptr;
PFNGLBEGINCONDITIONALRENDERPROC   glBeginConditionalRender   = nullptr;
PFNGLBEGINQUERYPROC               glBeginQuery               = nullptr;
PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation       = nullptr;
PFNGLBINDBUFFERPROC               glBindBuffer               = nullptr;
PFNGLBINDFRAMEBUFFERPROC          glBindFramebuffer          = nullptr;
PFNGLBINDFRAGDATALOCATIONPROC     glBindFragDataLocation     = nullptr;
PFNGLBINDRENDERBUFFERPROC         glBindRenderbuffer         = nullptr;
PFNGLBINDSAMPLERPROC              glBindSampler              = nullptr;
PFNGLBINDTEXTUREPROC              glBindTexture              = nullptr;
PFNGLBINDVERTEXARRAYPROC          glBindVertexArray          = nullptr;
PFNGLBLENDFUNCPROC                glBlendFunc                = nullptr;
PFNGLBLENDFUNCSEPARATEPROC        glBlendFuncSeparate        = nullptr;
PFNGLBLITFRAMEBUFFERPROC          glBlitFramebuffer          = nullptr;
PFNGLBUFFERDATAPROC               glBufferData               = nullptr;
PFNGLBUFFERSUBDATAPROC            glBufferSubData            = nullptr;
PFNGLCLEARPROC                    glClear                    = nullptr;
PFNGLCLEARCOLORPROC               glClearColor               = nullptr;
PFNGLCLEARDEPTHPROC               glClearDepth               = nullptr;
PFNGLCLEARSTENCILPROC             glClearStencil             = nullptr;
PFNGLCREATEPROGRAMPROC            glCreateProgram            = nullptr;
PFNGLCREATESHADERPROC             glCreateShader             = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC   glCheckFramebufferStatus   = nullptr;
PFNGLCOLORMASKPROC                glColorMask                = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC  glCompressedTexSubImage1D  = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC  glCompressedTexSubImage2D  = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC  glCompressedTexSubImage3D  = nullptr;
PFNGLCULLFACEPROC                 glCullFace                 = nullptr;
PFNGLCOMPILESHADERPROC            glCompileShader            = nullptr;
PFNGLCOPYTEXSUBIMAGE2DPROC        glCopyTexSubImage2D        = nullptr;
PFNGLDEBUGMESSAGECALLBACKPROC     glDebugMessageCallback     = nullptr;
PFNGLDEBUGMESSAGECONTROLPROC      glDebugMessageControl      = nullptr;
PFNGLDEBUGMESSAGEINSERTPROC       glDebugMessageInsert       = nullptr;
PFNGLDELETEBUFFERSPROC            glDeleteBuffers            = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC       glDeleteFramebuffers       = nullptr;
PFNGLDELETEPROGRAMPROC            glDeleteProgram            = nullptr;
PFNGLDELETEQUERIESPROC            glDeleteQueries            = nullptr;
PFNGLDELETERENDERBUFFERSPROC      glDeleteRenderbuffers      = nullptr;
PFNGLDELETESAMPLERSPROC           glDeleteSamplers           = nullptr;
PFNGLDELETESHADERPROC             glDeleteShader             = nullptr;
PFNGLDELETETEXTURESPROC           glDeleteTextures           = nullptr;
PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays       = nullptr;
PFNGLDEPTHFUNCPROC                glDepthFunc                = nullptr;
PFNGLDEPTHMASKPROC                glDepthMask                = nullptr;
PFNGLDISABLEPROC                  glDisable                  = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLDRAWARRAYSPROC               glDrawArrays               = nullptr;
PFNGLDRAWARRAYSINSTANCEDPROC      glDrawArraysInstanced      = nullptr;
PFNGLDRAWBUFFERPROC               glDrawBuffer               = nullptr;
PFNGLDRAWBUFFERSPROC              glDrawBuffers              = nullptr;
PFNGLDRAWELEMENTSPROC             glDrawElements             = nullptr;
PFNGLDRAWELEMENTSINSTANCEDPROC    glDrawElementsInstanced    = nullptr;
PFNGLDRAWTEXTURENVPROC            glDrawTexture              = nullptr;
PFNGLENABLEPROC                   glEnable                   = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray  = nullptr;
PFNGLENDCONDITIONALRENDERPROC     glEndConditionalRender     = nullptr;
PFNGLENDQUERYPROC                 glEndQuery                 = nullptr;
PFNGLFLUSHPROC                    glFlush                    = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC  glFramebufferRenderbuffer  = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC       glFramebufferTexture       = nullptr;
PFNGLFRAMEBUFFERTEXTURE1DPROC     glFramebufferTexture1D     = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC     glFramebufferTexture2D     = nullptr;
PFNGLFRAMEBUFFERTEXTURE3DPROC     glFramebufferTexture3D     = nullptr;
PFNGLFRAMEBUFFERTEXTURELAYERPROC  glFramebufferTextureLayer  = nullptr;
PFNGLGENERATEMIPMAPPROC           glGenerateMipmap           = nullptr;
PFNGLGENBUFFERSPROC               glGenBuffers               = nullptr;
PFNGLGENFRAMEBUFFERSPROC          glGenFramebuffers          = nullptr;
PFNGLGENRENDERBUFFERSPROC         glGenRenderbuffers         = nullptr;
PFNGLGENQUERIESPROC               glGenQueries               = nullptr;
PFNGLGENSAMPLERSPROC              glGenSamplers              = nullptr;
PFNGLGENTEXTURESPROC              glGenTextures              = nullptr;
PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays          = nullptr;
PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform         = nullptr;
PFNGLGETBOOLEANVPROC              glGetBooleanv              = nullptr;
PFNGLGETBUFFERPARAMETERIVPROC     glGetBufferParameteriv     = nullptr;
PFNGLGETDEBUGMESSAGELOGPROC       glGetDebugMessageLog       = nullptr;
PFNGLGETERRORPROC                 glGetError                 = nullptr;
PFNGLGETFLOATVPROC                glGetFloatv                = nullptr;
PFNGLGETINTEGERVPROC              glGetIntegerv              = nullptr;
PFNGLGETPROGRAMBINARYPROC         glGetProgramBinary         = nullptr;
PFNGLGETPROGRAMIVPROC             glGetProgramiv             = nullptr;
PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog        = nullptr;
PFNGLGETQUERYIVPROC               glGetQueryiv               = nullptr;
PFNGLGETQUERYOBJECTIVPROC         glGetQueryObjectiv         = nullptr;
PFNGLGETQUERYOBJECTUIVPROC        glGetQueryObjectuiv        = nullptr;
PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog         = nullptr;
PFNGLGETSHADERIVPROC              glGetShaderiv              = nullptr;
PFNGLGETSHADERSOURCEPROC          glGetShaderSource          = nullptr;
PFNGLGETSTRINGPROC                glGetString                = nullptr;
PFNGLGETSTRINGIPROC               glGetStringi               = nullptr;
PFNGLGETTEXIMAGEPROC              glGetTexImage              = nullptr;
PFNGLGETTEXLEVELPARAMETERFVPROC   glGetTexLevelParameterfv   = nullptr;
PFNGLGETTEXLEVELPARAMETERIVPROC   glGetTexLevelParameteriv   = nullptr;
PFNGLGETTEXPARAMETERFVPROC        glGetTexParameterfv        = nullptr;
PFNGLGETTEXPARAMETERIVPROC        glGetTexParameteriv        = nullptr;
PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation       = nullptr;
PFNGLINVALIDATEBUFFERDATAPROC     glInvalidateBufferData     = nullptr;
PFNGLISENABLEDPROC                glIsEnabled                = nullptr;
PFNGLLINEWIDTHPROC                glLineWidth                = nullptr;
PFNGLLINKPROGRAMPROC              glLinkProgram              = nullptr;
PFNGLMAPBUFFERPROC                glMapBuffer                = nullptr;
PFNGLMAPBUFFERRANGEPROC           glMapBufferRange           = nullptr;
PFNGLPIXELSTOREIPROC              glPixelStorei              = nullptr;
PFNGLPOINTSIZEPROC                glPointSize                = nullptr;
PFNGLPOLYGONMODEPROC              glPolygonMode              = nullptr;
PFNGLPROGRAMBINARYPROC            glProgramBinary            = nullptr;
PFNGLPROGRAMPARAMETERIPROC        glProgramParameteri        = nullptr;
PFNGLPROGRAMUNIFORM1DPROC         glProgramUniform1d         = nullptr;
PFNGLPROGRAMUNIFORM1FPROC         glProgramUniform1f         = nullptr;
PFNGLPROGRAMUNIFORM1IPROC         glProgramUniform1i         = nullptr;
PFNGLPROGRAMUNIFORM1DVPROC        glProgramUniform1dv        = nullptr;
PFNGLPROGRAMUNIFORM1FVPROC        glProgramUniform1fv        = nullptr;
PFNGLPROGRAMUNIFORM1IVPROC        glProgramUniform1iv        = nullptr;
PFNGLPROGRAMUNIFORM2DVPROC        glProgramUniform2dv        = nullptr;
PFNGLPROGRAMUNIFORM2FVPROC        glProgramUniform2fv        = nullptr;
PFNGLPROGRAMUNIFORM2IVPROC        glProgramUniform2iv        = nullptr;
PFNGLPROGRAMUNIFORM3DVPROC        glProgramUniform3dv        = nullptr;
PFNGLPROGRAMUNIFORM3FVPROC        glProgramUniform3fv        = nullptr;
PFNGLPROGRAMUNIFORM3IVPROC        glProgramUniform3iv        = nullptr;
PFNGLPROGRAMUNIFORM4DVPROC        glProgramUniform4dv        = nullptr;
PFNGLPROGRAMUNIFORM4FVPROC        glProgramUniform4fv        = nullptr;
PFNGLPROGRAMUNIFORM4IVPROC        glProgramUniform4iv        = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC  glProgramUniformMatrix4dv  = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC  glProgramUniformMatrix4fv  = nullptr;
PFNGLREADPIXELSPROC               glReadPixels               = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC      glRenderbufferStorage      = nullptr;
PFNGLSAMPLERPARAMETERFPROC        glSamplerParameterf        = nullptr;
PFNGLSAMPLERPARAMETERIPROC        glSamplerParameteri        = nullptr;
PFNGLSCISSORPROC                  glScissor                  = nullptr;
PFNGLSHADERSOURCEPROC             glShaderSource             = nullptr;
PFNGLSTENCILFUNCPROC              glStencilFunc              = nullptr;
PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate      = nullptr;
PFNGLSTENCILOPPROC                glStencilOp                = nullptr;
PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate        = nullptr;
PFNGLTEXIMAGE1DPROC               glTexImage1D               = nullptr;
PFNGLTEXIMAGE2DPROC               glTexImage2D               = nullptr;
PFNGLTEXIMAGE3DPROC               glTexImage3D               = nullptr;
PFNGLTEXPARAMETERFPROC            glTexParameterf            = nullptr;
PFNGLTEXPARAMETERIPROC            glTexParameteri            = nullptr;
PFNGLTEXSTORAGE1DPROC             glTexStorage1D             = nullptr;
PFNGLTEXSTORAGE2DPROC             glTexStorage2D             = nullptr;
PFNGLTEXSTORAGE3DPROC             glTexStorage3D             = nullptr;
PFNGLTEXSUBIMAGE1DPROC            glTexSubImage1D            = nullptr;
PFNGLTEXSUBIMAGE2DPROC            glTexSubImage2D            = nullptr;
PFNGLTEXSUBIMAGE3DPROC            glTexSubImage3D            = nullptr;
PFNGLUNIFORM1DPROC                glUniform1d                = nullptr;
PFNGLUNIFORM1FPROC                glUniform1f                = nullptr;
PFNGLUNIFORM1IPROC                glUniform1i                = nullptr;
PFNGLUNIFORM1DVPROC               glUniform1dv               = nullptr;
PFNGLUNIFORM1FVPROC               glUniform1fv               = nullptr;
PFNGLUNIFORM1IVPROC               glUniform1iv               = nullptr;
PFNGLUNIFORM2DVPROC               glUniform2dv               = nullptr;
PFNGLUNIFORM2FVPROC               glUniform2fv               = nullptr;
PFNGLUNIFORM2IVPROC               glUniform2iv               = nullptr;
PFNGLUNIFORM3DVPROC               glUniform3dv               = nullptr;
PFNGLUNIFORM3FVPROC               glUniform3fv               = nullptr;
PFNGLUNIFORM3IVPROC               glUniform3iv               = nullptr;
PFNGLUNIFORM4DVPROC               glUniform4dv               = nullptr;
PFNGLUNIFORM4FVPROC               glUniform4fv               = nullptr;
PFNGLUNIFORM4IVPROC               glUniform4iv               = nullptr;
PFNGLUNIFORMMATRIX4DVPROC         glUniformMatrix4dv         = nullptr;
PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv         = nullptr;
PFNGLUNMAPBUFFERPROC              glUnmapBuffer              = nullptr;
PFNGLUSEPROGRAMPROC               glUseProgram               = nullptr;
PFNGLVALIDATEPROGRAMPROC          glValidateProgram          = nullptr;
PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f           = nullptr;
PFNGLVERTEXATTRIBDIVISORPROC      glVertexAttribDivisor      = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer      = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC     glVertexAttribIPointer     = nullptr;
PFNGLVERTEXATTRIBLPOINTERPROC     glVertexAttribLPointer     = nullptr;
PFNGLVIEWPORTPROC                 glViewport                 = nullptr;

#if defined(NAZARA_PLATFORM_WINDOWS)
PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormat       = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs    = nullptr;
PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB  = nullptr;
PFNWGLGETEXTENSIONSSTRINGEXTPROC  wglGetExtensionsStringEXT  = nullptr;
PFNWGLSWAPINTERVALEXTPROC         wglSwapInterval            = nullptr;
#elif defined(NAZARA_PLATFORM_GLX)
GLX::PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribs = nullptr;
GLX::PFNGLXSWAPINTERVALEXTPROC         glXSwapIntervalEXT      = nullptr;
GLX::PFNGLXSWAPINTERVALMESAPROC        NzglXSwapIntervalMESA   = nullptr;
GLX::PFNGLXSWAPINTERVALSGIPROC         glXSwapIntervalSGI      = nullptr;
#endif

}
