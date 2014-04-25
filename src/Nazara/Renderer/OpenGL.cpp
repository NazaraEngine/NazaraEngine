// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <cstring>
#include <set>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	#ifdef NAZARA_PLATFORM_WINDOWS
	HMODULE openGLlibrary;
	#endif

	NzOpenGLFunc LoadEntry(const char* name, bool launchException = true)
	{
		#if defined(NAZARA_PLATFORM_WINDOWS)
		NzOpenGLFunc entry = reinterpret_cast<NzOpenGLFunc>(wglGetProcAddress(name));
		if (!entry) // wglGetProcAddress ne fonctionne pas sur les fonctions OpenGL <= 1.1
			entry = reinterpret_cast<NzOpenGLFunc>(GetProcAddress(openGLlibrary, name));
		#elif defined(NAZARA_PLATFORM_LINUX)
		NzOpenGLFunc entry = reinterpret_cast<NzOpenGLFunc>(glXGetProcAddress(name));
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
		GLuint buffersBinding[nzBufferType_Max+1] = {0};
		GLuint currentProgram = 0;
		GLuint samplers[32] = {0}; // 32 est pour l'instant la plus haute limite (GL_TEXTURE31)
		GLuint texturesBinding[32] = {0}; // 32 est pour l'instant la plus haute limite (GL_TEXTURE31)
		NzRecti currentScissorBox = NzRecti(0,0,0,0);
		NzRecti currentViewport = NzRecti(0,0,0,0);
		NzRenderStates renderStates; // Toujours synchronisé avec OpenGL
		const NzRenderTarget* currentTarget = nullptr;
		bool scissorBoxUpdated = true;
		bool viewportUpdated = true;
		unsigned int textureUnit = 0;
	};

	std::set<NzString> s_openGLextensionSet;
	std::unordered_map<const NzContext*, ContextStates> s_contexts;
	thread_local ContextStates* s_contextStates = nullptr;
	NzString s_rendererName;
	NzString s_vendorName;
	bool s_initialized = false;
	bool s_openGLextensions[nzOpenGLExtension_Max+1] = {false};
	unsigned int s_glslVersion = 0;
	unsigned int s_openglVersion = 0;

	bool LoadExtensionsString(const NzString& extensionString)
	{
		if (extensionString.IsEmpty())
		{
			NazaraError("Unable to get extension string");
			return false;
		}

		// On peut sûrement faire plus rapide mais comme ça ne se fait qu'une fois et que NzString implémente le COW...
		std::vector<NzString> ext;
		extensionString.Split(ext);

		for (std::vector<NzString>::iterator it = ext.begin(); it != ext.end(); ++it)
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
			NzString extension(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));
			if (extension.IsEmpty())
			{
				NazaraWarning("Unable to get extension #" + NzString::Number(i));
				continue;
			}

			s_openGLextensionSet.insert(extension);
		}

		return true;
	}
}

void NzOpenGL::ApplyStates(const NzRenderStates& states)
{
	#ifdef NAZARA_DEBUG
	if (!s_contextStates)
	{
		NazaraError("No context activated");
		return;
	}
	#endif

	NzRenderStates& currentRenderStates = s_contextStates->renderStates;

	// Les fonctions de blend n'a aucun intérêt sans blending
	if (states.parameters[nzRendererParameter_Blend])
	{
		if (currentRenderStates.dstBlend != states.dstBlend ||
		    currentRenderStates.srcBlend != states.srcBlend)
		{
			glBlendFunc(BlendFunc[states.srcBlend], BlendFunc[states.dstBlend]);
			currentRenderStates.dstBlend = states.dstBlend;
			currentRenderStates.srcBlend = states.srcBlend;
		}
	}

	if (states.parameters[nzRendererParameter_DepthBuffer])
	{
		// La comparaison de profondeur n'a aucun intérêt sans depth buffer
		if (currentRenderStates.depthFunc != states.depthFunc)
		{
			glDepthFunc(RendererComparison[states.depthFunc]);
			currentRenderStates.depthFunc = states.depthFunc;
		}

		// Le DepthWrite n'a aucune importance si le DepthBuffer est désactivé
		if (currentRenderStates.parameters[nzRendererParameter_DepthWrite] != states.parameters[nzRendererParameter_DepthWrite])
		{
			glDepthMask((states.parameters[nzRendererParameter_DepthWrite]) ? GL_TRUE : GL_FALSE);
			currentRenderStates.parameters[nzRendererParameter_DepthWrite] = states.parameters[nzRendererParameter_DepthWrite];
		}
	}

	// Inutile de changer le mode de face culling s'il n'est pas actif
	if (states.parameters[nzRendererParameter_FaceCulling])
	{
		if (currentRenderStates.faceCulling != states.faceCulling)
		{
			glCullFace(FaceSide[states.faceCulling]);
			currentRenderStates.faceCulling = states.faceCulling;
		}
	}

	if (currentRenderStates.faceFilling != states.faceFilling)
	{
		glPolygonMode(GL_FRONT_AND_BACK, FaceFilling[states.faceFilling]);
		currentRenderStates.faceFilling = states.faceFilling;
	}

	// Ici encore, ça ne sert à rien de se soucier des fonctions de stencil sans qu'il soit activé
	if (states.parameters[nzRendererParameter_StencilTest])
	{
		for (unsigned int i = 0; i < 2; ++i)
		{
			GLenum face = (i == 0) ? GL_BACK : GL_FRONT;
			const NzRenderStates::Face& srcStates = (i == 0) ? states.backFace : states.frontFace;
			NzRenderStates::Face& dstStates = (i == 0) ? currentRenderStates.backFace : currentRenderStates.frontFace;

			if (dstStates.stencilCompare != srcStates.stencilCompare ||
				dstStates.stencilMask != srcStates.stencilMask ||
				dstStates.stencilReference != srcStates.stencilReference)
			{
				glStencilFuncSeparate(face, RendererComparison[srcStates.stencilCompare], srcStates.stencilReference, srcStates.stencilMask);
				dstStates.stencilCompare = srcStates.stencilCompare;
				dstStates.stencilMask = srcStates.stencilMask;
				dstStates.stencilReference = srcStates.stencilReference;
			}

			if (dstStates.stencilFail != srcStates.stencilFail ||
				dstStates.stencilPass != srcStates.stencilPass ||
				dstStates.stencilZFail != srcStates.stencilZFail)
			{
				glStencilOpSeparate(face, StencilOperation[srcStates.stencilFail], StencilOperation[srcStates.stencilZFail], StencilOperation[srcStates.stencilPass]);
				dstStates.stencilFail = srcStates.stencilFail;
				dstStates.stencilPass = srcStates.stencilPass;
				dstStates.stencilZFail = srcStates.stencilZFail;
			}
		}
	}

	if (!NzNumberEquals(currentRenderStates.lineWidth, states.lineWidth, 0.001f))
	{
		glLineWidth(states.lineWidth);
		currentRenderStates.lineWidth = states.lineWidth;
	}

	if (!NzNumberEquals(currentRenderStates.pointSize, states.pointSize, 0.001f))
	{
		glPointSize(states.pointSize);
		currentRenderStates.pointSize = states.pointSize;
	}

	// Paramètres de rendu
	if (currentRenderStates.parameters[nzRendererParameter_Blend] != states.parameters[nzRendererParameter_Blend])
	{
		if (states.parameters[nzRendererParameter_Blend])
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		currentRenderStates.parameters[nzRendererParameter_Blend] = states.parameters[nzRendererParameter_Blend];
	}

	if (currentRenderStates.parameters[nzRendererParameter_ColorWrite] != states.parameters[nzRendererParameter_ColorWrite])
	{
		GLboolean param = (states.parameters[nzRendererParameter_ColorWrite]) ? GL_TRUE : GL_FALSE;
		glColorMask(param, param, param, param);

		currentRenderStates.parameters[nzRendererParameter_ColorWrite] = states.parameters[nzRendererParameter_ColorWrite];
	}

	if (currentRenderStates.parameters[nzRendererParameter_DepthBuffer] != states.parameters[nzRendererParameter_DepthBuffer])
	{
		if (states.parameters[nzRendererParameter_DepthBuffer])
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		currentRenderStates.parameters[nzRendererParameter_DepthBuffer] = states.parameters[nzRendererParameter_DepthBuffer];
	}

	if (currentRenderStates.parameters[nzRendererParameter_FaceCulling] != states.parameters[nzRendererParameter_FaceCulling])
	{
		if (states.parameters[nzRendererParameter_FaceCulling])
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);

		currentRenderStates.parameters[nzRendererParameter_FaceCulling] = states.parameters[nzRendererParameter_FaceCulling];
	}

	if (currentRenderStates.parameters[nzRendererParameter_ScissorTest] != states.parameters[nzRendererParameter_ScissorTest])
	{
		if (states.parameters[nzRendererParameter_ScissorTest])
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);

		currentRenderStates.parameters[nzRendererParameter_ScissorTest] = states.parameters[nzRendererParameter_ScissorTest];
	}

	if (currentRenderStates.parameters[nzRendererParameter_StencilTest] != states.parameters[nzRendererParameter_StencilTest])
	{
		if (states.parameters[nzRendererParameter_StencilTest])
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);

		currentRenderStates.parameters[nzRendererParameter_StencilTest] = states.parameters[nzRendererParameter_StencilTest];
	}
}

void NzOpenGL::BindBuffer(nzBufferType type, GLuint id)
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

void NzOpenGL::BindProgram(GLuint id)
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

void NzOpenGL::BindSampler(GLuint unit, GLuint id)
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

void NzOpenGL::BindScissorBox(const NzRecti& scissorBox)
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

void NzOpenGL::BindTexture(nzImageType type, GLuint id)
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

void NzOpenGL::BindTexture(unsigned int textureUnit, nzImageType type, GLuint id)
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

void NzOpenGL::BindTextureUnit(unsigned int textureUnit)
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

void NzOpenGL::BindViewport(const NzRecti& viewport)
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

void NzOpenGL::DeleteBuffer(nzBufferType type, GLuint id)
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

void NzOpenGL::DeleteFrameBuffer(const NzContext* context, GLuint id)
{
	// Si le contexte est actif, ne nous privons pas
	if (NzContext::GetCurrent() == context)
		glDeleteFramebuffers(1, &id);
	else
		s_contexts[context].garbage.emplace_back(GarbageResourceType_FrameBuffer, id);
}

void NzOpenGL::DeleteProgram(GLuint id)
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

void NzOpenGL::DeleteSampler(GLuint id)
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

void NzOpenGL::DeleteTexture(GLuint id)
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

void NzOpenGL::DeleteVertexArray(const NzContext* context, GLuint id)
{
	// Si le contexte est actif, ne nous privons pas
	if (NzContext::GetCurrent() == context)
		glDeleteFramebuffers(1, &id);
	else
		s_contexts[context].garbage.emplace_back(GarbageResourceType_VertexArray, id);
}

GLuint NzOpenGL::GetCurrentBuffer(nzBufferType type)
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

GLuint NzOpenGL::GetCurrentProgram()
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

NzRecti NzOpenGL::GetCurrentScissorBox()
{
	#ifdef NAZARA_DEBUG
	if (!s_contextStates)
	{
		NazaraError("No context activated");
		return NzRecti();
	}
	#endif

	return s_contextStates->currentScissorBox;
}

const NzRenderTarget* NzOpenGL::GetCurrentTarget()
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

GLuint NzOpenGL::GetCurrentTexture()
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

GLuint NzOpenGL::GetCurrentTexture(unsigned int textureUnit)
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

unsigned int NzOpenGL::GetCurrentTextureUnit()
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

NzRecti NzOpenGL::GetCurrentViewport()
{
	#ifdef NAZARA_DEBUG
	if (!s_contextStates)
	{
		NazaraError("No context activated");
		return NzRecti();
	}
	#endif

	return s_contextStates->currentViewport;
}

NzOpenGLFunc NzOpenGL::GetEntry(const NzString& entryPoint)
{
	return LoadEntry(entryPoint.GetConstBuffer(), false);
}

unsigned int NzOpenGL::GetGLSLVersion()
{
	return s_glslVersion;
}

NzString NzOpenGL::GetRendererName()
{
	return s_rendererName;
}

NzString NzOpenGL::GetVendorName()
{
	return s_vendorName;
}

unsigned int NzOpenGL::GetVersion()
{
	return s_openglVersion;
}

bool NzOpenGL::Initialize()
{
	if (s_initialized)
		return true;

	if (!LoadLibrary())
	{
		NazaraError("Failed to load OpenGL library");
		return false;
	}

	s_initialized = true;

	// En cas d'erreur, on libèrera OpenGL
	NzCallOnExit onExit(NzOpenGL::Uninitialize);

	// Le chargement des fonctions OpenGL nécessite un contexte OpenGL
	NzContextParameters parameters;
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

	NzContext loadContext;
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
	#elif defined(NAZARA_PLATFORM_LINUX)
	glXCreateContextAttribs = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(LoadEntry("glXCreateContextAttribsARB", false));
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

	if (major <= 0 || major > 9)
	{
		NazaraError("Unable to retrieve OpenGL major version");
		return false;
	}

	if (minor > 9) // 0 est une valeur correcte ici (ex: OpenGL 3.0)
	{
		NazaraWarning("Unable to retrieve OpenGL minor version (assuming 0)");
		minor = 0;
	}

	s_openglVersion = major*100 + minor*10; // Donnera 330 pour OpenGL 3.3, 410 pour OpenGL 4.1, bien plus facile à comparer

	NazaraDebug("OpenGL version: " + NzString::Number(major) + '.' + NzString::Number(minor));

	// Le moteur ne fonctionnera pas avec OpenGL 1.x, autant s'arrêter là si c'est le cas
	if (s_openglVersion < 200)
	{
		NazaraError("OpenGL " + NzString::Number(major) + '.' + NzString::Number(minor) + " detected (2.0 required). Please upgrade your drivers or your video card");
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

	s_glslVersion = major*100 + minor*10; // GLSL 3.3 => 330

	// Possible uniquement dans le cas où le GLSL vient d'une extension d'OpenGL 1
	// Ce qui est rejeté il y a un moment déjà, mais on doit s'attendre à tout de la part d'un driver...
	// (Exemple: Un driver OpenGL 2 mais ne supportant que le GLSL 100)
	if (s_glslVersion < 110)
	{
		NazaraError("GLSL version is too low, please upgrade your drivers or your video card");
		return false;
	}

	parameters.debugMode = true; // Certaines extensions n'apparaissent qu'avec un contexte de debug (e.g. ARB_debug_output)
	parameters.majorVersion = NzContextParameters::defaultMajorVersion = major;
	parameters.minorVersion = NzContextParameters::defaultMinorVersion = minor;

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
		glBeginQuery = reinterpret_cast<PFNGLBEGINQUERYPROC>(LoadEntry("glBeginQuery"));
		glBindAttribLocation = reinterpret_cast<PFNGLBINDATTRIBLOCATIONPROC>(LoadEntry("glBindAttribLocation"));
		glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(LoadEntry("glBindBuffer"));
		glBindTexture = reinterpret_cast<PFNGLBINDTEXTUREPROC>(LoadEntry("glBindTexture"));
		glBlendFunc = reinterpret_cast<PFNGLBLENDFUNCPROC>(LoadEntry("glBlendFunc"));
		glBlendFuncSeparate = reinterpret_cast<PFNGLBLENDFUNCSEPARATEPROC>(LoadEntry("glBlendFuncSeparate"));
		glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(LoadEntry("glBufferData"));
		glBufferSubData = reinterpret_cast<PFNGLBUFFERSUBDATAPROC>(LoadEntry("glBufferSubData"));
		glClear = reinterpret_cast<PFNGLCLEARPROC>(LoadEntry("glClear"));
		glClearColor = reinterpret_cast<PFNGLCLEARCOLORPROC>(LoadEntry("glClearColor"));
		glClearDepth = reinterpret_cast<PFNGLCLEARDEPTHPROC>(LoadEntry("glClearDepth"));
		glClearStencil = reinterpret_cast<PFNGLCLEARSTENCILPROC>(LoadEntry("glClearStencil"));
		glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(LoadEntry("glCreateProgram"));
		glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(LoadEntry("glCreateShader"));
		glColorMask = reinterpret_cast<PFNGLCOLORMASKPROC>(LoadEntry("glColorMask"));
		glCullFace = reinterpret_cast<PFNGLCULLFACEPROC>(LoadEntry("glCullFace"));
		glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(LoadEntry("glCompileShader"));
		glCopyTexSubImage2D = reinterpret_cast<PFNGLCOPYTEXSUBIMAGE2DPROC>(LoadEntry("glCopyTexSubImage2D"));
		glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(LoadEntry("glDeleteBuffers"));
		glDeleteQueries = reinterpret_cast<PFNGLDELETEQUERIESPROC>(LoadEntry("glDeleteQueries"));
		glDeleteProgram = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(LoadEntry("glDeleteProgram"));
		glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(LoadEntry("glDeleteShader"));
		glDeleteTextures = reinterpret_cast<PFNGLDELETETEXTURESPROC>(LoadEntry("glDeleteTextures"));
		glDepthFunc = reinterpret_cast<PFNGLDEPTHFUNCPROC>(LoadEntry("glDepthFunc"));
		glDepthMask = reinterpret_cast<PFNGLDEPTHMASKPROC>(LoadEntry("glDepthMask"));
		glDisable = reinterpret_cast<PFNGLDISABLEPROC>(LoadEntry("glDisable"));
		glDisableVertexAttribArray = reinterpret_cast<PFNGLDISABLEVERTEXATTRIBARRAYPROC>(LoadEntry("glDisableVertexAttribArray"));
		glDrawArrays = reinterpret_cast<PFNGLDRAWARRAYSPROC>(LoadEntry("glDrawArrays"));
		glDrawBuffer = reinterpret_cast<PFNGLDRAWBUFFERPROC>(LoadEntry("glDrawBuffer"));
		glDrawBuffers = reinterpret_cast<PFNGLDRAWBUFFERSPROC>(LoadEntry("glDrawBuffers"));
		glDrawElements = reinterpret_cast<PFNGLDRAWELEMENTSPROC>(LoadEntry("glDrawElements"));
		glEnable = reinterpret_cast<PFNGLENABLEPROC>(LoadEntry("glEnable"));
		glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(LoadEntry("glEnableVertexAttribArray"));
		glEndQuery = reinterpret_cast<PFNGLENDQUERYPROC>(LoadEntry("glEndQuery"));
		glFlush = reinterpret_cast<PFNGLFLUSHPROC>(LoadEntry("glFlush"));
		glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(LoadEntry("glGenBuffers"));
		glGenQueries = reinterpret_cast<PFNGLGENQUERIESPROC>(LoadEntry("glGenQueries"));
		glGenTextures = reinterpret_cast<PFNGLGENTEXTURESPROC>(LoadEntry("glGenTextures"));
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
		glPixelStorei = reinterpret_cast<PFNGLPIXELSTOREIPROC>(LoadEntry("glPixelStorei"));
		glPointSize = reinterpret_cast<PFNGLPOINTSIZEPROC>(LoadEntry("glPointSize"));
		glPolygonMode = reinterpret_cast<PFNGLPOLYGONMODEPROC>(LoadEntry("glPolygonMode"));
		glReadPixels = reinterpret_cast<PFNGLREADPIXELSPROC>(LoadEntry("glReadPixels"));
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
		glVertexAttrib4f = reinterpret_cast<PFNGLVERTEXATTRIB4FPROC>(LoadEntry("glVertexAttrib4f"));
		glVertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(LoadEntry("glVertexAttribPointer"));
		glViewport = reinterpret_cast<PFNGLVIEWPORTPROC>(LoadEntry("glViewport"));
	}
	catch (const std::exception& e)
	{
		NazaraError("Unable to load OpenGL: " + NzString(e.what()));
		return false;
	}

	/****************************************Extensions****************************************/

	// Fonctions optionnelles
	glBindFragDataLocation = reinterpret_cast<PFNGLBINDFRAGDATALOCATIONPROC>(LoadEntry("glBindFragDataLocation", false));
	if (!glBindFragDataLocation)
		glBindFragDataLocation = reinterpret_cast<PFNGLBINDFRAGDATALOCATIONEXTPROC>(LoadEntry("glBindFragDataLocationEXT", false));

	glDrawTexture = reinterpret_cast<PFNGLDRAWTEXTURENVPROC>(LoadEntry("glDrawTextureNV", false));
	glFramebufferTexture = reinterpret_cast<PFNGLFRAMEBUFFERTEXTUREPROC>(LoadEntry("glFramebufferTexture", false));
	glGetStringi = reinterpret_cast<PFNGLGETSTRINGIPROC>(LoadEntry("glGetStringi", false));
	glInvalidateBufferData = reinterpret_cast<PFNGLINVALIDATEBUFFERDATAPROC>(LoadEntry("glInvalidateBufferData", false));
	glMapBufferRange = reinterpret_cast<PFNGLMAPBUFFERRANGEPROC>(LoadEntry("glMapBufferRange", false));
	glVertexAttribIPointer = reinterpret_cast<PFNGLVERTEXATTRIBIPOINTERPROC>(LoadEntry("glVertexAttribIPointer", false));
	glVertexAttribLPointer = reinterpret_cast<PFNGLVERTEXATTRIBLPOINTERPROC>(LoadEntry("glVertexAttribLPointer", false));

	#if defined(NAZARA_PLATFORM_WINDOWS)
	wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(LoadEntry("wglGetExtensionsStringARB", false));
	wglGetExtensionsStringEXT = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(LoadEntry("wglGetExtensionsStringEXT", false));
	wglSwapInterval = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(LoadEntry("wglSwapIntervalEXT", false));
	#elif defined(NAZARA_PLATFORM_LINUX)
	glXSwapInterval = reinterpret_cast<PFNGLXSWAPINTERVALSGIPROC>(LoadEntry("glXSwapIntervalSGI", false));
	#endif

	if (!glGetStringi || !LoadExtensions3())
	{
		if (s_openglVersion >= 300) // Dans le cas contraire c'est normal
			NazaraWarning("Failed to load OpenGL 3 extension system, switching to OpenGL 2 extension system...");

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
	s_openGLextensions[nzOpenGLExtension_AnisotropicFilter] = IsSupported("GL_EXT_texture_filter_anisotropic");

	// ConditionalRender
	if (s_openglVersion >= 300)
	{
		try
		{
			glBeginConditionalRender = reinterpret_cast<PFNGLBEGINCONDITIONALRENDERPROC>(LoadEntry("glBeginConditionalRender"));
			glEndConditionalRender = reinterpret_cast<PFNGLENDCONDITIONALRENDERPROC>(LoadEntry("glEndConditionalRender"));

			s_openGLextensions[nzOpenGLExtension_ConditionalRender] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load Conditional Render: " + NzString(e.what()));
		}
	}

	if (!s_openGLextensions[nzOpenGLExtension_ConditionalRender] && IsSupported("GL_NV_conditional_render"))
	{
		try
		{
			glBeginConditionalRender = reinterpret_cast<PFNGLBEGINCONDITIONALRENDERPROC>(LoadEntry("glBeginConditionalRenderNV"));
			glEndConditionalRender = reinterpret_cast<PFNGLENDCONDITIONALRENDERPROC>(LoadEntry("glEndConditionalRenderNV"));

			s_openGLextensions[nzOpenGLExtension_ConditionalRender] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load GL_NV_conditional_render: " + NzString(e.what()));
		}
	}

	// DebugOutput
	if (s_openglVersion >= 430 || IsSupported("GL_KHR_debug"))
	{
		try
		{
			glDebugMessageCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(LoadEntry("glDebugMessageCallback"));
			glDebugMessageControl = reinterpret_cast<PFNGLDEBUGMESSAGECONTROLPROC>(LoadEntry("glDebugMessageControl"));
			glDebugMessageInsert = reinterpret_cast<PFNGLDEBUGMESSAGEINSERTPROC>(LoadEntry("glDebugMessageInsert"));
			glGetDebugMessageLog = reinterpret_cast<PFNGLGETDEBUGMESSAGELOGPROC>(LoadEntry("glGetDebugMessageLog"));

			s_openGLextensions[nzOpenGLExtension_DebugOutput] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load GL_KHR_debug: " + NzString(e.what()));
		}
	}

	if (!s_openGLextensions[nzOpenGLExtension_DebugOutput] && IsSupported("GL_ARB_debug_output"))
	{
		try
		{
			glDebugMessageCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKARBPROC>(LoadEntry("glDebugMessageCallbackARB"));
			glDebugMessageControl = reinterpret_cast<PFNGLDEBUGMESSAGECONTROLARBPROC>(LoadEntry("glDebugMessageControlARB"));
			glDebugMessageInsert = reinterpret_cast<PFNGLDEBUGMESSAGEINSERTARBPROC>(LoadEntry("glDebugMessageInsertARB"));
			glGetDebugMessageLog = reinterpret_cast<PFNGLGETDEBUGMESSAGELOGARBPROC>(LoadEntry("glGetDebugMessageLogARB"));

			s_openGLextensions[nzOpenGLExtension_DebugOutput] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load GL_ARB_debug_output: " + NzString(e.what()));
		}
	}

	// DrawInstanced
	if (s_openglVersion >= 330)
	{
		try
		{
			glDrawArraysInstanced = reinterpret_cast<PFNGLDRAWARRAYSINSTANCEDPROC>(LoadEntry("glDrawArraysInstanced"));
			glDrawElementsInstanced = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDPROC>(LoadEntry("glDrawElementsInstanced"));

			s_openGLextensions[nzOpenGLExtension_DrawInstanced] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load Draw Instanced: " + NzString(e.what()));
		}
	}

	if (!s_openGLextensions[nzOpenGLExtension_DrawInstanced] && IsSupported("GL_ARB_draw_instanced"))
	{
		try
		{
			glDrawArraysInstanced = reinterpret_cast<PFNGLDRAWARRAYSINSTANCEDARBPROC>(LoadEntry("glDrawArraysInstancedARB"));
			glDrawElementsInstanced = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDARBPROC>(LoadEntry("glDrawElementsInstancedARB"));

			s_openGLextensions[nzOpenGLExtension_DrawInstanced] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load GL_ARB_draw_instanced: " + NzString(e.what()));
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

			s_openGLextensions[nzOpenGLExtension_FP64] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load ARB_gpu_shader_fp64: " + NzString(e.what()));
		}
	}

	// FrameBufferObject
	if (s_openglVersion >= 300 || IsSupported("GL_ARB_framebuffer_object"))
	{
		try
		{
			glBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(LoadEntry("glBindFramebuffer"));
			glBindRenderbuffer = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(LoadEntry("glBindRenderbuffer"));
			glBlitFramebuffer = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(LoadEntry("glBlitFramebuffer"));
			glCheckFramebufferStatus = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(LoadEntry("glCheckFramebufferStatus"));
			glDeleteFramebuffers = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(LoadEntry("glDeleteFramebuffers"));
			glDeleteRenderbuffers = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(LoadEntry("glDeleteRenderbuffers"));
			glFramebufferRenderbuffer = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(LoadEntry("glFramebufferRenderbuffer"));
			glFramebufferTexture1D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE1DPROC>(LoadEntry("glFramebufferTexture1D"));
			glFramebufferTexture2D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(LoadEntry("glFramebufferTexture2D"));
			glFramebufferTexture3D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE3DPROC>(LoadEntry("glFramebufferTexture3D"));
			glFramebufferTextureLayer = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURELAYERPROC>(LoadEntry("glFramebufferTextureLayer"));
			glGenerateMipmap = reinterpret_cast<PFNGLGENERATEMIPMAPPROC>(LoadEntry("glGenerateMipmap"));
			glGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(LoadEntry("glGenFramebuffers"));
			glGenRenderbuffers = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(LoadEntry("glGenRenderbuffers"));
			glRenderbufferStorage = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(LoadEntry("glRenderbufferStorage"));

			s_openGLextensions[nzOpenGLExtension_FrameBufferObject] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load ARB_framebuffer_object: (" + NzString(e.what()) + ")");
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

			s_openGLextensions[nzOpenGLExtension_GetProgramBinary] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load ARB_get_program_binary: (" + NzString(e.what()) + ")");
		}
	}

	// InstancedArray
	if (s_openglVersion >= 330)
	{
		try
		{
			glVertexAttribDivisor = reinterpret_cast<PFNGLVERTEXATTRIBDIVISORPROC>(LoadEntry("glVertexAttribDivisor"));

			s_openGLextensions[nzOpenGLExtension_InstancedArray] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load Instanced Array: " + NzString(e.what()));
		}
	}

	if (!s_openGLextensions[nzOpenGLExtension_InstancedArray] && IsSupported("GL_ARB_instanced_arrays"))
	{
		try
		{
			glVertexAttribDivisor = reinterpret_cast<PFNGLVERTEXATTRIBDIVISORARBPROC>(LoadEntry("glVertexAttribDivisorARB"));

			s_openGLextensions[nzOpenGLExtension_InstancedArray] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load GL_ARB_instanced_arrays: " + NzString(e.what()));
		}
	}

	// PixelBufferObject
	s_openGLextensions[nzOpenGLExtension_PixelBufferObject] = (s_openglVersion >= 210 || IsSupported("GL_ARB_pixel_buffer_object"));

	// SamplerObjects
	if (s_openglVersion >= 330 || IsSupported("GL_ARB_sampler_objects"))
	{
		try
		{
			glBindSampler = reinterpret_cast<PFNGLBINDSAMPLERPROC>(LoadEntry("glBindSampler"));
			glDeleteSamplers = reinterpret_cast<PFNGLDELETESAMPLERSPROC>(LoadEntry("glDeleteSamplers"));
			glGenSamplers = reinterpret_cast<PFNGLGENSAMPLERSPROC>(LoadEntry("glGenSamplers"));
			glSamplerParameterf = reinterpret_cast<PFNGLSAMPLERPARAMETERFPROC>(LoadEntry("glSamplerParameterf"));
			glSamplerParameteri = reinterpret_cast<PFNGLSAMPLERPARAMETERIPROC>(LoadEntry("glSamplerParameteri"));

			s_openGLextensions[nzOpenGLExtension_SamplerObjects] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load ARB_sampler_objects: (" + NzString(e.what()) + ")");
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
			if (s_openGLextensions[nzOpenGLExtension_FP64])
			{
				glProgramUniform1d = reinterpret_cast<PFNGLPROGRAMUNIFORM1DPROC>(LoadEntry("glProgramUniform1d"));
				glProgramUniform1dv = reinterpret_cast<PFNGLPROGRAMUNIFORM2DVPROC>(LoadEntry("glProgramUniform1dv"));
				glProgramUniform2dv = reinterpret_cast<PFNGLPROGRAMUNIFORM2DVPROC>(LoadEntry("glProgramUniform2dv"));
				glProgramUniform3dv = reinterpret_cast<PFNGLPROGRAMUNIFORM3DVPROC>(LoadEntry("glProgramUniform3dv"));
				glProgramUniform4dv = reinterpret_cast<PFNGLPROGRAMUNIFORM4DVPROC>(LoadEntry("glProgramUniform4dv"));
				glProgramUniformMatrix4dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4DVPROC>(LoadEntry("glProgramUniformMatrix4dv"));
			}

			s_openGLextensions[nzOpenGLExtension_SeparateShaderObjects] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load ARB_separate_shader_objects: (" + NzString(e.what()) + ")");
		}
	}

	// Shader_ImageLoadStore
	s_openGLextensions[nzOpenGLExtension_Shader_ImageLoadStore] = (s_openglVersion >= 420 || IsSupported("GL_ARB_shader_image_load_store"));

	// TextureArray
	s_openGLextensions[nzOpenGLExtension_TextureArray] = (s_openglVersion >= 300 || IsSupported("GL_EXT_texture_array"));

	// TextureCompression_s3tc
	s_openGLextensions[nzOpenGLExtension_TextureCompression_s3tc] = IsSupported("GL_EXT_texture_compression_s3tc");

	// TextureStorage
	if (s_openglVersion >= 420 || IsSupported("GL_ARB_texture_storage"))
	{
		try
		{
			glTexStorage1D = reinterpret_cast<PFNGLTEXSTORAGE1DPROC>(LoadEntry("glTexStorage1D"));
			glTexStorage2D = reinterpret_cast<PFNGLTEXSTORAGE2DPROC>(LoadEntry("glTexStorage2D"));
			glTexStorage3D = reinterpret_cast<PFNGLTEXSTORAGE3DPROC>(LoadEntry("glTexStorage3D"));

			s_openGLextensions[nzOpenGLExtension_TextureStorage] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load ARB_texture_storage: " + NzString(e.what()));
		}
	}

	// VertexArrayObject
	if (s_openglVersion >= 300 || IsSupported("GL_ARB_vertex_array_object"))
	{
		try
		{
			glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(LoadEntry("glBindVertexArray"));
			glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(LoadEntry("glDeleteVertexArrays"));
			glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(LoadEntry("glGenVertexArrays"));

			s_openGLextensions[nzOpenGLExtension_VertexArrayObjects] = true;
		}
		catch (const std::exception& e)
		{
			NazaraWarning("Failed to load ARB_vertex_array_object: " + NzString(e.what()));
		}
	}

	// Fonctions de substitut
	if (!glGenerateMipmap)
		glGenerateMipmap = reinterpret_cast<PFNGLGENERATEMIPMAPEXTPROC>(LoadEntry("glGenerateMipmapEXT", false));

	/******************************Initialisation*****************************/

	s_contextStates = nullptr;
	s_rendererName = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	s_vendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

	// On initialise les vrais contextes OpenGL
	if (!NzContext::Initialize())
	{
		NazaraError("Failed to initialize contexts");
		return false;
	}

	// Le contexte OpenGL n'est plus assuré à partir d'ici
	onExit.Reset();

	return true;
}

bool NzOpenGL::IsInitialized()
{
	return s_initialized;
}

bool NzOpenGL::IsSupported(nzOpenGLExtension extension)
{
	return s_openGLextensions[extension];
}

bool NzOpenGL::IsSupported(const NzString& string)
{
	return s_openGLextensionSet.find(string) != s_openGLextensionSet.end();
}

void NzOpenGL::SetBuffer(nzBufferType type, GLuint id)
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

void NzOpenGL::SetScissorBox(const NzRecti& scissorBox)
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

void NzOpenGL::SetProgram(GLuint id)
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

void NzOpenGL::SetTarget(const NzRenderTarget* renderTarget)
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
			const NzRecti& scissorBox = s_contextStates->currentViewport;

			unsigned int height = s_contextStates->currentTarget->GetHeight();
			glScissor(scissorBox.x, height - scissorBox.height - scissorBox.y, scissorBox.width, scissorBox.height);

			s_contextStates->scissorBoxUpdated = true;
		}

		if (!s_contextStates->viewportUpdated)
		{
			const NzRecti& viewport = s_contextStates->currentViewport;

			unsigned int height = s_contextStates->currentTarget->GetHeight();
			glViewport(viewport.x, height - viewport.height - viewport.y, viewport.width, viewport.height);

			s_contextStates->viewportUpdated = true;
		}
	}
}

void NzOpenGL::SetTexture(GLuint id)
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

void NzOpenGL::SetTexture(unsigned int textureUnit, GLuint id)
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

void NzOpenGL::SetTextureUnit(unsigned int textureUnit)
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

void NzOpenGL::SetViewport(const NzRecti& viewport)
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

bool NzOpenGL::TranslateFormat(nzPixelFormat pixelFormat, Format* format, FormatType type)
{
	switch (pixelFormat)
	{
		case nzPixelFormat_BGR8:
			format->dataFormat = GL_BGR;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_RGB8;
			return true;

		case nzPixelFormat_BGRA8:
			format->dataFormat = GL_BGRA;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_RGBA8;
			return true;

		case nzPixelFormat_DXT1:
			format->dataFormat = GL_RGB;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			return true;

		case nzPixelFormat_DXT3:
			format->dataFormat = GL_RGBA;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			return true;

		case nzPixelFormat_DXT5:
			format->dataFormat = GL_RGBA;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			return true;

		case nzPixelFormat_L8:
		case nzPixelFormat_LA8:
			return false;

		case nzPixelFormat_R8:
			format->dataFormat = GL_RED;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_R8;
			return true;

		case nzPixelFormat_R8I:
			format->dataFormat = GL_RED;
			format->dataType = GL_BYTE;
			format->internalFormat = GL_R8I;
			return true;

		case nzPixelFormat_R8UI:
			format->dataFormat = GL_RED;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_R8UI;
			return true;

		case nzPixelFormat_R16:
			format->dataFormat = GL_RED;
			format->dataType = GL_UNSIGNED_SHORT;
			format->internalFormat = GL_R16;
			return true;

		case nzPixelFormat_R16F:
			format->dataFormat = GL_RED;
			format->dataType = GL_HALF_FLOAT;
			format->internalFormat = GL_R16F;
			return true;

		case nzPixelFormat_R16I:
			format->dataFormat = GL_RED;
			format->dataType = GL_SHORT;
			format->internalFormat = GL_R16I;
			return true;

		case nzPixelFormat_R16UI:
			format->dataFormat = GL_RED;
			format->dataType = GL_UNSIGNED_SHORT;
			format->internalFormat = GL_R16UI;
			return true;

		case nzPixelFormat_R32F:
			format->dataFormat = GL_RED;
			format->dataType = GL_FLOAT;
			format->internalFormat = GL_R32F;
			return true;

		case nzPixelFormat_R32I:
			format->dataFormat = GL_RED;
			format->dataType = GL_INT;
			format->internalFormat = GL_R32I;
			return true;

		case nzPixelFormat_R32UI:
			format->dataFormat = GL_RED;
			format->dataType = GL_UNSIGNED_INT;
			format->internalFormat = GL_R32UI;
			return true;

		case nzPixelFormat_RG8:
			format->dataFormat = GL_RG;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_RG8;
			return true;

		case nzPixelFormat_RG8I:
			format->dataFormat = GL_RG;
			format->dataType = GL_BYTE;
			format->internalFormat = GL_RG8I;
			return true;

		case nzPixelFormat_RG8UI:
			format->dataFormat = GL_RG;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_RG8UI;
			return true;

		case nzPixelFormat_RG16:
			format->dataFormat = GL_RG;
			format->dataType = GL_UNSIGNED_SHORT;
			format->internalFormat = GL_RG16;
			return true;

		case nzPixelFormat_RG16F:
			format->dataFormat = GL_RG;
			format->dataType = GL_HALF_FLOAT;
			format->internalFormat = GL_RG16F;
			return true;

		case nzPixelFormat_RG16I:
			format->dataFormat = GL_RG;
			format->dataType = GL_SHORT;
			format->internalFormat = GL_RG16I;
			return true;

		case nzPixelFormat_RG16UI:
			format->dataFormat = GL_RG;
			format->dataType = GL_UNSIGNED_SHORT;
			format->internalFormat = GL_RG16UI;
			return true;

		case nzPixelFormat_RG32F:
			format->dataFormat = GL_RG;
			format->dataType = GL_FLOAT;
			format->internalFormat = GL_RG32F;
			return true;

		case nzPixelFormat_RG32I:
			format->dataFormat = GL_RG;
			format->dataType = GL_INT;
			format->internalFormat = GL_RG32I;
			return true;

		case nzPixelFormat_RG32UI:
			format->dataFormat = GL_RG;
			format->dataType = GL_UNSIGNED_INT;
			format->internalFormat = GL_RG32UI;
			return true;

		case nzPixelFormat_RGB5A1:
			format->dataFormat = GL_RGBA;
			format->dataType = GL_UNSIGNED_SHORT_5_5_5_1;
			format->internalFormat = GL_RGB5_A1;
			return true;

		case nzPixelFormat_RGB8:
			format->dataFormat = GL_RGB;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_RGB8;
			return true;

		case nzPixelFormat_RGB16F:
			format->dataFormat = GL_RGB;
			format->dataType = GL_HALF_FLOAT;
			format->internalFormat = GL_RGB16F;
			return true;

		case nzPixelFormat_RGB16I:
			format->dataFormat = GL_RGB;
			format->dataType = GL_SHORT;
			format->internalFormat = GL_RGB16I;
			return true;

		case nzPixelFormat_RGB16UI:
			format->dataFormat = GL_RGB;
			format->dataType = GL_UNSIGNED_SHORT;
			format->internalFormat = GL_RGB16UI;
			return true;

		case nzPixelFormat_RGB32F:
			format->dataFormat = GL_RGB;
			format->dataType = GL_FLOAT;
			format->internalFormat = GL_RGB32F;
			return true;

		case nzPixelFormat_RGB32I:
			format->dataFormat = GL_RGB;
			format->dataType = GL_INT;
			format->internalFormat = GL_RGB32I;
			return true;

		case nzPixelFormat_RGB32UI:
			format->dataFormat = GL_RGB;
			format->dataType = GL_UNSIGNED_INT;
			format->internalFormat = GL_RGB32UI;
			return true;

		case nzPixelFormat_RGBA4:
			format->dataFormat = GL_RGBA;
			format->dataType = GL_UNSIGNED_SHORT_4_4_4_4;
			format->internalFormat = GL_RGBA4;
			return true;

		case nzPixelFormat_RGBA8:
			format->dataFormat = GL_RGBA;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_RGBA8;
			return true;

		case nzPixelFormat_RGBA16F:
			format->dataFormat = GL_RGBA;
			format->dataType = GL_HALF_FLOAT;
			format->internalFormat = GL_RGBA16F;
			return true;

		case nzPixelFormat_RGBA16I:
			format->dataFormat = GL_RGBA;
			format->dataType = GL_SHORT;
			format->internalFormat = GL_RGBA16I;
			return true;

		case nzPixelFormat_RGBA16UI:
			format->dataFormat = GL_RGBA;
			format->dataType = GL_INT;
			format->internalFormat = GL_RGBA16UI;
			return true;

		case nzPixelFormat_RGBA32F:
			format->dataFormat = GL_RGBA;
			format->dataType = GL_FLOAT;
			format->internalFormat = GL_RGBA32F;
			return true;

		case nzPixelFormat_RGBA32I:
			format->dataFormat = GL_RGB;
			format->dataType = GL_INT;
			format->internalFormat = GL_RGB32I;
			return true;

		case nzPixelFormat_RGBA32UI:
			format->dataFormat = GL_RGB;
			format->dataType = GL_UNSIGNED_INT;
			format->internalFormat = GL_RGB32UI;
			return true;

		case nzPixelFormat_Depth16:
			format->dataFormat = GL_DEPTH_COMPONENT;
			format->dataType = GL_UNSIGNED_SHORT;
			format->internalFormat = GL_DEPTH_COMPONENT16;
			return true;

		case nzPixelFormat_Depth24:
			format->dataFormat = GL_DEPTH_COMPONENT;
			format->dataType = GL_UNSIGNED_INT;
			format->internalFormat = GL_DEPTH_COMPONENT24;
			return true;

		case nzPixelFormat_Depth24Stencil8:
			format->dataFormat = GL_DEPTH_STENCIL;
			format->dataType = GL_UNSIGNED_INT_24_8;
			format->internalFormat = GL_DEPTH24_STENCIL8;
			return true;

		case nzPixelFormat_Depth32:
			format->dataFormat = GL_DEPTH_COMPONENT;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_DEPTH_COMPONENT32;
			return true;

		case nzPixelFormat_Stencil1:
			if (type == FormatType_Texture) // Les formats de stencil ne sont pas supportés par les textures
				return false;
			else
			{
				format->dataFormat = GL_NONE;
				format->dataType = GL_NONE;
				format->internalFormat = GL_STENCIL_INDEX1;
				return true;
			}

		case nzPixelFormat_Stencil4:
			if (type == FormatType_Texture)
				return false;
			else
			{
				format->dataFormat = GL_NONE;
				format->dataType = GL_NONE;
				format->internalFormat = GL_STENCIL_INDEX4;
				return true;
			}

		case nzPixelFormat_Stencil8:
			if (type == FormatType_Texture)
				return false;
			else
			{
				format->dataFormat = GL_NONE;
				format->dataType = GL_NONE;
				format->internalFormat = GL_STENCIL_INDEX8;
				return true;
			}

		case nzPixelFormat_Stencil16:
			if (type == FormatType_Texture)
				return false;
			else
			{
				format->dataFormat = GL_NONE;
				format->dataType = GL_NONE;
				format->internalFormat = GL_STENCIL_INDEX16;
				return true;
			}

		case nzPixelFormat_Undefined:
			break;
	}

	NazaraError("Invalid pixel format");
	return false;
}

void NzOpenGL::Uninitialize()
{
	if (s_initialized)
	{
		s_initialized = false;

		NzContext::Uninitialize();

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

void NzOpenGL::OnContextChanged(const NzContext* newContext)
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

void NzOpenGL::OnContextDestruction(const NzContext* context)
{
	/*
	** Il serait possible d'activer le contexte avant sa destruction afin de libérer les éventuelles ressources mortes-vivantes,
	** mais un driver bien conçu va libérer ces ressources de lui-même.
	*/
	s_contexts.erase(context);
}

GLenum NzOpenGL::Attachment[] =
{
	GL_COLOR_ATTACHMENT0,        // nzAttachmentPoint_Color
	GL_DEPTH_ATTACHMENT,         // nzAttachmentPoint_Depth
	GL_DEPTH_STENCIL_ATTACHMENT, // nzAttachmentPoint_DepthStencil
	GL_STENCIL_ATTACHMENT        // nzAttachmentPoint_Stencil
};

static_assert(nzAttachmentPoint_Max+1 == 4, "Attachment array is incomplete");

nzUInt8 NzOpenGL::AttributeIndex[] =
{
	10, // nzAttributeUsage_InstanceData0
	11, // nzAttributeUsage_InstanceData1
	12, // nzAttributeUsage_InstanceData2
	13, // nzAttributeUsage_InstanceData3
	14, // nzAttributeUsage_InstanceData4
	15, // nzAttributeUsage_InstanceData5
	2,  // nzAttributeUsage_Normal
	0,  // nzAttributeUsage_Position
	3,  // nzAttributeUsage_Tangent
	1,  // nzAttributeUsage_TexCoord
	4,  // nzAttributeUsage_Userdata0
	5,  // nzAttributeUsage_Userdata1
	6,  // nzAttributeUsage_Userdata2
	7,  // nzAttributeUsage_Userdata3
	8,  // nzAttributeUsage_Userdata4
	9   // nzAttributeUsage_Userdata5
};

static_assert(nzAttributeUsage_Max+1 == 16, "Attribute index array is incomplete");

GLenum NzOpenGL::AttributeType[] =
{
	GL_UNSIGNED_BYTE, // nzAttributeType_Color
	GL_DOUBLE,        // nzAttributeType_Double1
	GL_DOUBLE,        // nzAttributeType_Double2
	GL_DOUBLE,        // nzAttributeType_Double3
	GL_DOUBLE,        // nzAttributeType_Double4
	GL_FLOAT,         // nzAttributeType_Float1
	GL_FLOAT,         // nzAttributeType_Float2
	GL_FLOAT,         // nzAttributeType_Float3
	GL_FLOAT,         // nzAttributeType_Float4
	GL_INT,           // nzAttributeType_Int1
	GL_INT,           // nzAttributeType_Int2
	GL_INT,           // nzAttributeType_Int3
	GL_INT            // nzAttributeType_Int4
};

static_assert(nzAttributeType_Max+1 == 13, "Attribute type array is incomplete");

GLenum NzOpenGL::BlendFunc[] =
{
	GL_DST_ALPHA,           // nzBlendFunc_DestAlpha
	GL_DST_COLOR,           // nzBlendFunc_DestColor
	GL_SRC_ALPHA,           // nzBlendFunc_SrcAlpha
	GL_SRC_COLOR,           // nzBlendFunc_SrcColor
	GL_ONE_MINUS_DST_ALPHA, // nzBlendFunc_InvDestAlpha
	GL_ONE_MINUS_DST_COLOR, // nzBlendFunc_InvDestColor
	GL_ONE_MINUS_SRC_ALPHA, // nzBlendFunc_InvSrcAlpha
	GL_ONE_MINUS_SRC_COLOR, // nzBlendFunc_InvSrcColor
	GL_ONE,                 // nzBlendFunc_One
	GL_ZERO                 // nzBlendFunc_Zero
};

static_assert(nzBlendFunc_Max+1 == 10, "Blend func array is incomplete");

GLenum NzOpenGL::BufferLock[] =
{
	GL_WRITE_ONLY, // nzBufferAccess_DiscardAndWrite
	GL_READ_ONLY,  // nzBufferAccess_ReadOnly
	GL_READ_WRITE, // nzBufferAccess_ReadWrite
	GL_WRITE_ONLY  // nzBufferAccess_WriteOnly
};

static_assert(nzBufferAccess_Max+1 == 4, "Buffer lock array is incomplete");

GLenum NzOpenGL::BufferLockRange[] =
{
	// http://www.opengl.org/discussion_boards/showthread.php/170118-VBOs-strangely-slow?p=1198118#post1198118
	GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT, // nzBufferAccess_DiscardAndWrite
	GL_MAP_READ_BIT,                                                               // nzBufferAccess_ReadOnly
	GL_MAP_READ_BIT | GL_MAP_WRITE_BIT,                                            // nzBufferAccess_ReadWrite
	GL_MAP_WRITE_BIT                                                               // nzBufferAccess_WriteOnly
};

static_assert(nzBufferAccess_Max+1 == 4, "Buffer lock range array is incomplete");

GLenum NzOpenGL::BufferTarget[] =
{
	GL_ELEMENT_ARRAY_BUFFER, // nzBufferType_Index,
	GL_ARRAY_BUFFER,		 // nzBufferType_Vertex
};

static_assert(nzBufferType_Max+1 == 2, "Buffer target array is incomplete");

GLenum NzOpenGL::BufferTargetBinding[] =
{
	GL_ELEMENT_ARRAY_BUFFER_BINDING, // nzBufferType_Index,
	GL_ARRAY_BUFFER_BINDING,		 // nzBufferType_Vertex
};

static_assert(nzBufferType_Max+1 == 2, "Buffer target binding array is incomplete");

GLenum NzOpenGL::BufferUsage[] =
{
	// D'après la documentation, GL_STREAM_DRAW semble être plus adapté à notre cas (ratio modification/rendu 1:2-3)
	// Source: http://www.opengl.org/sdk/docs/man/html/glBufferData.xhtml
	GL_STREAM_DRAW, // nzBufferUsage_Dynamic
	GL_STATIC_DRAW  // nzBufferUsage_Static
};

static_assert(nzBufferUsage_Max+1 == 2, "Buffer usage array is incomplete");

GLenum NzOpenGL::CubemapFace[] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X, // nzCubemapFace_PositiveX
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // nzCubemapFace_NegativeX
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // nzCubemapFace_PositiveY (Inversion pour les standards OpenGL)
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // nzCubemapFace_NegativeY (Inversion pour les standards OpenGL)
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // nzCubemapFace_PositiveZ
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // nzCubemapFace_NegativeZ
};

static_assert(nzCubemapFace_Max+1 == 6, "Cubemap face array is incomplete");

GLenum NzOpenGL::FaceFilling[] =
{
	GL_POINT, // nzFaceFilling_Point
	GL_LINE,  // nzFaceFilling_Line
	GL_FILL   // nzFaceFilling_Fill
};

static_assert(nzFaceFilling_Max+1 == 3, "Face filling array is incomplete");

GLenum NzOpenGL::FaceSide[] =
{
	GL_BACK,          // nzFaceSide_Back
	GL_FRONT,         // nzFaceSide_Front
	GL_FRONT_AND_BACK // nzFaceSide_FrontAndBack
};

static_assert(nzFaceSide_Max+1 == 3, "Face side array is incomplete");

GLenum NzOpenGL::PrimitiveMode[] =
{
	GL_LINES,          // nzPrimitiveMode_LineList
	GL_LINE_STRIP,     // nzPrimitiveMode_LineStrip
	GL_POINTS,         // nzPrimitiveMode_PointList
	GL_TRIANGLES,      // nzPrimitiveMode_TriangleList
	GL_TRIANGLE_STRIP, // nzPrimitiveMode_TriangleStrip
	GL_TRIANGLE_FAN    // nzPrimitiveMode_TriangleFan
};

static_assert(nzPrimitiveMode_Max+1 == 6, "Primitive mode array is incomplete");

GLenum NzOpenGL::QueryCondition[] =
{
	GL_QUERY_WAIT,              // nzGpuQueryCondition_NoWait
	GL_QUERY_BY_REGION_NO_WAIT, // nzGpuQueryCondition_Region_NoWait
	GL_QUERY_BY_REGION_WAIT,    // nzGpuQueryCondition_Region_Wait
	GL_QUERY_WAIT               // nzGpuQueryCondition_Wait
};

static_assert(nzGpuQueryCondition_Max+1 == 4, "Query condition array is incomplete");

GLenum NzOpenGL::QueryMode[] =
{
	GL_ANY_SAMPLES_PASSED,                   // nzGpuQueryMode_AnySamplesPassed
	GL_ANY_SAMPLES_PASSED_CONSERVATIVE,      // nzGpuQueryMode_AnySamplesPassedConservative
	GL_PRIMITIVES_GENERATED,                 // nzGpuQueryMode_PrimitiveGenerated
	GL_SAMPLES_PASSED,                       // nzGpuQueryMode_SamplesPassed
	GL_TIME_ELAPSED,                         // nzGpuQueryMode_TimeElapsed
	GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN // nzGpuQueryMode_TransformFeedbackPrimitivesWritten
};

static_assert(nzGpuQueryMode_Max+1 == 6, "Query mode array is incomplete");

GLenum NzOpenGL::RendererComparison[] =
{
	GL_ALWAYS,  // nzRendererComparison_Always
	GL_EQUAL,   // nzRendererComparison_Equal
	GL_GREATER, // nzRendererComparison_Greater
	GL_GEQUAL,  // nzRendererComparison_GreaterOrEqual
	GL_LESS,    // nzRendererComparison_Less
	GL_LEQUAL,  // nzRendererComparison_LessOrEqual
	GL_NEVER,   // nzRendererComparison_Never
	GL_NOTEQUAL // nzRendererComparison_NotEqual
};

static_assert(nzRendererComparison_Max+1 == 8, "Renderer comparison array is incomplete");

GLenum NzOpenGL::RendererParameter[] =
{
	GL_BLEND,        // nzRendererParameter_Blend
	GL_NONE,         // nzRendererParameter_ColorWrite
	GL_DEPTH_TEST,   // nzRendererParameter_DepthBuffer
	GL_NONE,         // nzRendererParameter_DepthWrite
	GL_CULL_FACE,    // nzRendererParameter_FaceCulling
	GL_SCISSOR_TEST, // nzRendererParameter_ScissorTest
	GL_STENCIL_TEST  // nzRendererParameter_StencilTest
};

static_assert(nzRendererParameter_Max+1 == 7, "Renderer parameter array is incomplete");

GLenum NzOpenGL::SamplerWrapMode[] =
{
	GL_CLAMP_TO_EDGE,   // nzTextureWrap_Clamp
	GL_MIRRORED_REPEAT, // nzSamplerWrap_MirroredRepeat
	GL_REPEAT           // nzTextureWrap_Repeat
};

static_assert(nzSamplerWrap_Max+1 == 3, "Sampler wrap mode array is incomplete");

GLenum NzOpenGL::ShaderStage[] =
{
	GL_FRAGMENT_SHADER,	// nzShaderStage_Fragment
	GL_GEOMETRY_SHADER,	// nzShaderStage_Geometry
	GL_VERTEX_SHADER	// nzShaderStage_Vertex
};

static_assert(nzShaderStage_Max+1 == 3, "Shader stage array is incomplete");

GLenum NzOpenGL::StencilOperation[] =
{
	GL_DECR,      // nzStencilOperation_Decrement
	GL_DECR_WRAP, // nzStencilOperation_DecrementNoClamp
	GL_INCR,      // nzStencilOperation_Increment
	GL_INCR_WRAP, // nzStencilOperation_IncrementNoClamp
	GL_INVERT,    // nzStencilOperation_Invert
	GL_KEEP,      // nzStencilOperation_Keep
	GL_REPLACE,   // nzStencilOperation_Replace
	GL_ZERO       // nzStencilOperation_Zero
};

static_assert(nzStencilOperation_Max+1 == 8, "Stencil operation array is incomplete");

GLenum NzOpenGL::TextureTarget[] =
{
	GL_TEXTURE_1D,       // nzImageType_1D
	GL_TEXTURE_1D_ARRAY, // nzImageType_1D_Array
	GL_TEXTURE_2D,       // nzImageType_2D
	GL_TEXTURE_2D_ARRAY, // nzImageType_2D_Array
	GL_TEXTURE_3D,       // nzImageType_3D
	GL_TEXTURE_CUBE_MAP  // nzImageType_Cubemap
};

static_assert(nzImageType_Max+1 == 6, "Texture target array is incomplete");

GLenum NzOpenGL::TextureTargetBinding[] =
{
	GL_TEXTURE_BINDING_1D,       // nzImageType_1D
	GL_TEXTURE_BINDING_1D_ARRAY, // nzImageType_1D_Array
	GL_TEXTURE_BINDING_2D,       // nzImageType_2D
	GL_TEXTURE_BINDING_2D_ARRAY, // nzImageType_2D_Array
	GL_TEXTURE_BINDING_3D,       // nzImageType_3D
	GL_TEXTURE_BINDING_CUBE_MAP  // nzImageType_Cubemap
};

static_assert(nzImageType_Max+1 == 6, "Texture target binding array is incomplete");

GLenum NzOpenGL::TextureTargetProxy[] =
{
	GL_PROXY_TEXTURE_1D,       // nzImageType_1D
	GL_PROXY_TEXTURE_1D_ARRAY, // nzImageType_1D_Array
	GL_PROXY_TEXTURE_2D,       // nzImageType_2D
	GL_PROXY_TEXTURE_2D_ARRAY, // nzImageType_2D_Array
	GL_PROXY_TEXTURE_3D,       // nzImageType_3D
	GL_PROXY_TEXTURE_CUBE_MAP  // nzImageType_Cubemap
};

static_assert(nzImageType_Max+1 == 6, "Texture target proxy array is incomplete");

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
#elif defined(NAZARA_PLATFORM_LINUX)
PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribs    = nullptr;
PFNGLXSWAPINTERVALSGIPROC         glXSwapInterval            = nullptr;
#endif
