// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <cstring>
#include <set>
#include <sstream>
#include <stdexcept>
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

	std::set<NzString> s_openGLextensionSet;
	GLuint s_buffersBinding[nzBufferType_Max+1];
	GLuint s_currentProgram;
	GLuint s_texturesBinding[32]; // 32 est pour l'instant la plus haute limite (GL_TEXTURE31)
	NzRenderStates s_states; // Toujours synchronisé avec OpenGL
	const char* s_rendererName = nullptr;
	const char* s_vendorName = nullptr;
	bool s_initialized = false;
	bool s_openGLextensions[nzOpenGLExtension_Max+1] = {false};
	unsigned int s_openglVersion = 0;
	unsigned int s_textureUnit = 0;

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
	if (s_states.dstBlend != states.dstBlend || s_states.srcBlend != states.srcBlend)
	{
		glBlendFunc(BlendFunc[states.srcBlend], BlendFunc[states.dstBlend]);
		s_states.dstBlend = states.dstBlend;
		s_states.srcBlend = states.srcBlend;
	}

	if (s_states.depthFunc != states.depthFunc)
	{
		glDepthFunc(RendererComparison[states.depthFunc]);
		s_states.depthFunc = states.depthFunc;
	}

	if (s_states.faceCulling != states.faceCulling)
	{
		glCullFace(FaceCulling[states.faceCulling]);
		s_states.faceCulling = states.faceCulling;
	}

	if (s_states.faceFilling != states.faceFilling)
	{
		glPolygonMode(GL_FRONT_AND_BACK, FaceFilling[states.faceFilling]);
		s_states.faceFilling = states.faceFilling;
	}

	if (s_states.stencilCompare != states.stencilCompare || s_states.stencilMask != states.stencilMask || s_states.stencilReference != states.stencilReference)
	{
		glStencilFunc(RendererComparison[states.stencilCompare], states.stencilReference, states.stencilMask);
		s_states.stencilCompare = states.stencilCompare;
		s_states.stencilMask = states.stencilMask;
		s_states.stencilReference = states.stencilReference;
	}

	if (s_states.stencilFail != states.stencilFail || s_states.stencilPass != states.stencilPass || s_states.stencilZFail != states.stencilZFail)
	{
		glStencilOp(StencilOperation[states.stencilFail], StencilOperation[states.stencilZFail], StencilOperation[states.stencilPass]);
		s_states.stencilFail = states.stencilFail;
		s_states.stencilPass = states.stencilPass;
		s_states.stencilZFail = states.stencilZFail;
	}

	if (!NzNumberEquals(s_states.lineWidth, states.lineWidth, 0.001f))
	{
		glLineWidth(states.lineWidth);
		s_states.lineWidth = states.lineWidth;
	}

	if (!NzNumberEquals(s_states.pointSize, states.pointSize, 0.001f))
	{
		glPointSize(states.pointSize);
		s_states.pointSize = states.pointSize;
	}

	// Paramètres de rendu
	if (s_states.parameters[nzRendererParameter_Blend] != states.parameters[nzRendererParameter_Blend])
	{
		if (states.parameters[nzRendererParameter_Blend])
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		s_states.parameters[nzRendererParameter_Blend] = states.parameters[nzRendererParameter_Blend];
	}

	if (s_states.parameters[nzRendererParameter_ColorWrite] != states.parameters[nzRendererParameter_ColorWrite])
	{
		GLboolean param = (states.parameters[nzRendererParameter_ColorWrite]) ? GL_TRUE : GL_FALSE;
		glColorMask(param, param, param, param);

		s_states.parameters[nzRendererParameter_ColorWrite] = states.parameters[nzRendererParameter_ColorWrite];
	}

	if (s_states.parameters[nzRendererParameter_DepthBuffer] != states.parameters[nzRendererParameter_DepthBuffer])
	{
		if (states.parameters[nzRendererParameter_DepthBuffer])
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		s_states.parameters[nzRendererParameter_DepthBuffer] = states.parameters[nzRendererParameter_DepthBuffer];
	}

	if (s_states.parameters[nzRendererParameter_DepthWrite] != states.parameters[nzRendererParameter_DepthWrite])
	{
		glDepthMask((states.parameters[nzRendererParameter_DepthWrite]) ? GL_TRUE : GL_FALSE);
		s_states.parameters[nzRendererParameter_DepthWrite] = states.parameters[nzRendererParameter_DepthWrite];
	}

	if (s_states.parameters[nzRendererParameter_FaceCulling] != states.parameters[nzRendererParameter_FaceCulling])
	{
		if (states.parameters[nzRendererParameter_FaceCulling])
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);

		s_states.parameters[nzRendererParameter_FaceCulling] = states.parameters[nzRendererParameter_FaceCulling];
	}

	if (s_states.parameters[nzRendererParameter_ScissorTest] != states.parameters[nzRendererParameter_ScissorTest])
	{
		if (states.parameters[nzRendererParameter_ScissorTest])
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);

		s_states.parameters[nzRendererParameter_ScissorTest] = states.parameters[nzRendererParameter_ScissorTest];
	}

	if (s_states.parameters[nzRendererParameter_StencilTest] != states.parameters[nzRendererParameter_StencilTest])
	{
		if (states.parameters[nzRendererParameter_StencilTest])
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);

		s_states.parameters[nzRendererParameter_StencilTest] = states.parameters[nzRendererParameter_StencilTest];
	}
}

void NzOpenGL::BindBuffer(nzBufferType type, GLuint id)
{
	if (s_buffersBinding[type] != id)
	{
		glBindBuffer(BufferTarget[type], id);
		s_buffersBinding[type] = id;
	}
}

void NzOpenGL::BindProgram(GLuint id)
{
	if (s_currentProgram != id)
	{
		glUseProgram(id);
		s_currentProgram = id;
	}
}

void NzOpenGL::BindTexture(nzImageType type, GLuint id)
{
	if (s_texturesBinding[s_textureUnit] != id)
	{
		glBindTexture(TextureTarget[type], id);
		s_texturesBinding[s_textureUnit] = id;
	}
}

void NzOpenGL::BindTexture(unsigned int textureUnit, nzImageType type, GLuint id)
{
	if (s_texturesBinding[textureUnit] != id)
	{
		SetTextureUnit(textureUnit);

		glBindTexture(TextureTarget[type], id);
		s_texturesBinding[textureUnit] = id;
	}
}

void NzOpenGL::BindVertexArray(GLuint id)
{
	// Je ne pense pas que ça soit une bonne idée de le mettre en cache, c'est un objet "spécial"
	glBindVertexArray(id);

	// On invalide les bindings des buffers (Overridés par le VertexArray)
	std::memset(s_buffersBinding, 0, (nzBufferType_Max+1)*sizeof(GLuint));
}

void NzOpenGL::DeleteBuffer(nzBufferType type, GLuint id)
{
	glDeleteBuffers(1, &id);
	if (s_buffersBinding[type] == id)
		s_buffersBinding[type] = 0;
}

void NzOpenGL::DeleteProgram(GLuint id)
{
	glDeleteProgram(id);
	if (s_currentProgram == id)
		s_currentProgram = 0;
}

void NzOpenGL::DeleteTexture(GLuint id)
{
	glDeleteTextures(1, &id);

	for (GLuint& binding : s_texturesBinding)
	{
		if (binding == id)
			binding = 0;
	}
}

GLuint NzOpenGL::GetCurrentBuffer(nzBufferType type)
{
	return s_buffersBinding[type];
}

GLuint NzOpenGL::GetCurrentProgram()
{
	return s_currentProgram;
}

NzOpenGLFunc NzOpenGL::GetEntry(const NzString& entryPoint)
{
	return LoadEntry(entryPoint.GetConstBuffer(), false);
}

NzString NzOpenGL::GetRendererName()
{
	return s_rendererName;
}

unsigned int NzOpenGL::GetTextureUnit()
{
	return s_textureUnit;
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

	// Le chargement des fonctions OpenGL nécessite un contexte OpenGL
	NzContextParameters parameters;
	parameters.majorVersion = 2;
	parameters.minorVersion = 0;
	parameters.shared = false;

	/*
		Note: Même le contexte de chargement nécessite quelques fonctions de base pour correctement s'initialiser
		Pour cette raison, deux contextes sont créés, le premier sert à récupérer les fonctions permetttant
		de créer le second avec les bons paramètres.

		Non sérieusement si vous avez une meilleure idée contactez-moi
	*/

	/****************************************Chargement OpenGL****************************************/

	NzContext loadContext;
	if (!loadContext.Create(parameters))
	{
		NazaraError("Failed to create load context");
		Uninitialize();

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

	// Récupération de la version d'OpenGL
	// Ce code se base sur le fait que la carte graphique renverra un contexte de compatibilité avec la plus haute version supportée
	// Ce qui semble vrai au moins chez ATI/AMD et NVidia, mais si quelqu'un à une meilleure idée ...
	glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(LoadEntry("glGetString", false));
	if (!glGetString)
	{
		NazaraError("Unable to load OpenGL: failed to load glGetString");
		Uninitialize();

		return false;
	}

	const GLubyte* version = glGetString(GL_VERSION);
	if (!version)
	{
		NazaraError("Unable to retrieve OpenGL version");
		Uninitialize();

		return false;
	}

	unsigned int major = version[0] - '0';
	unsigned int minor = version[2] - '0';

	if (major == 0 || major > 9)
	{
		NazaraError("Unable to retrieve OpenGL major version");
		return false;
	}

	if (minor > 9)
	{
		NazaraWarning("Unable to retrieve OpenGL minor version (using 0)");
		minor = 0;
	}

	s_openglVersion = major*100 + minor*10;
	if (s_openglVersion < 200)
	{
		NazaraError("OpenGL version is too low, please upgrade your drivers or your video card");
		Uninitialize();

		return false;
	}

	parameters.debugMode = true; // Certaines extensions n'apparaissent qu'avec un contexte de debug (e.g. ARB_debug_output)
	parameters.majorVersion = NzContextParameters::defaultMajorVersion = major;
	parameters.minorVersion = NzContextParameters::defaultMinorVersion = minor;

	if (!loadContext.Create(parameters)) // Destruction implicite du premier contexte
	{
		NazaraError("Failed to create load context");
		Uninitialize();

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
		glStencilOp = reinterpret_cast<PFNGLSTENCILOPPROC>(LoadEntry("glStencilOp"));
		glTexImage2D = reinterpret_cast<PFNGLTEXIMAGE2DPROC>(LoadEntry("glTexImage2D"));
		glTexImage3D = reinterpret_cast<PFNGLTEXIMAGE3DPROC>(LoadEntry("glTexImage3D"));
		glTexParameterf = reinterpret_cast<PFNGLTEXPARAMETERFPROC>(LoadEntry("glTexParameterf"));
		glTexParameteri = reinterpret_cast<PFNGLTEXPARAMETERIPROC>(LoadEntry("glTexParameteri"));
		glTexSubImage2D = reinterpret_cast<PFNGLTEXSUBIMAGE2DPROC>(LoadEntry("glTexSubImage2D"));
		glTexSubImage3D = reinterpret_cast<PFNGLTEXSUBIMAGE3DPROC>(LoadEntry("glTexSubImage3D"));
		glUniform1f = reinterpret_cast<PFNGLUNIFORM1FPROC>(LoadEntry("glUniform1f"));
		glUniform1i = reinterpret_cast<PFNGLUNIFORM1IPROC>(LoadEntry("glUniform1i"));
		glUniform2fv = reinterpret_cast<PFNGLUNIFORM2FVPROC>(LoadEntry("glUniform2fv"));
		glUniform3fv = reinterpret_cast<PFNGLUNIFORM3FVPROC>(LoadEntry("glUniform3fv"));
		glUniform4fv = reinterpret_cast<PFNGLUNIFORM4FVPROC>(LoadEntry("glUniform4fv"));
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
		Uninitialize();

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
	glMapBufferRange = reinterpret_cast<PFNGLMAPBUFFERRANGEPROC>(LoadEntry("glMapBufferRange", false));
	glInvalidateBufferData = reinterpret_cast<PFNGLINVALIDATEBUFFERDATAPROC>(LoadEntry("glInvalidateBufferData", false));

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
			NazaraWarning("Failed to load GL_ARB_draw_instanced: " + NzString(e.what()));
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
			NazaraWarning("Failed to load GL_ARB_instanced_arrays: " + NzString(e.what()));
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
			glProgramUniform2fv = reinterpret_cast<PFNGLPROGRAMUNIFORM2FVPROC>(LoadEntry("glProgramUniform2fv"));
			glProgramUniform3fv = reinterpret_cast<PFNGLPROGRAMUNIFORM3FVPROC>(LoadEntry("glProgramUniform3fv"));
			glProgramUniform4fv = reinterpret_cast<PFNGLPROGRAMUNIFORM4FVPROC>(LoadEntry("glProgramUniform4fv"));
			glProgramUniformMatrix4fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4FVPROC>(LoadEntry("glProgramUniformMatrix4fv"));

			// Si ARB_gpu_shader_fp64 est supporté, alors cette extension donne également accès aux fonctions utilisant des double
			if (s_openGLextensions[nzOpenGLExtension_FP64])
			{
				glProgramUniform1d = reinterpret_cast<PFNGLPROGRAMUNIFORM1DPROC>(LoadEntry("glProgramUniform1d"));
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

	/****************************************Initialisation****************************************/

	///FIXME: Utiliser le contexte de chargement comme référence ? (Vérifier mode debug)
	if (!NzContext::Initialize())
	{
		NazaraError("Failed to initialize contexts");
		Uninitialize();

		return false;
	}

	std::memset(s_buffersBinding, 0, (nzBufferType_Max+1)*sizeof(GLuint));
	std::memset(s_texturesBinding, 0, 32*sizeof(GLuint));

	s_currentProgram = 0;
	s_rendererName = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	s_textureUnit = 0;
	s_vendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

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

void NzOpenGL::SetTextureUnit(unsigned int textureUnit)
{
	if (s_textureUnit != textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		s_textureUnit = textureUnit;
	}
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

		case nzPixelFormat_Depth16:
			format->dataFormat = GL_DEPTH_COMPONENT;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_DEPTH_COMPONENT16;
			return true;

		case nzPixelFormat_Depth24:
			format->dataFormat = GL_DEPTH_COMPONENT;
			format->dataType = GL_UNSIGNED_BYTE;
			format->internalFormat = GL_DEPTH_COMPONENT24;
			return true;

		case nzPixelFormat_Depth24Stencil8:
			format->dataFormat = GL_DEPTH_STENCIL;
			format->dataType = GL_UNSIGNED_BYTE;
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
		NzContext::Uninitialize();

		for (bool& ext : s_openGLextensions)
			ext = false;

		s_initialized = false;
		s_openGLextensionSet.clear();
		s_openglVersion = 0;
		s_rendererName = nullptr;
		s_vendorName = nullptr;

		UnloadLibrary();
	}
}

GLenum NzOpenGL::Attachment[nzAttachmentPoint_Max+1] =
{
	GL_COLOR_ATTACHMENT0,        // nzAttachmentPoint_Color
	GL_DEPTH_ATTACHMENT,         // nzAttachmentPoint_Depth
	GL_DEPTH_STENCIL_ATTACHMENT, // nzAttachmentPoint_DepthStencil
	GL_STENCIL_ATTACHMENT        // nzAttachmentPoint_Stencil
};

nzUInt8 NzOpenGL::AttributeIndex[nzElementUsage_Max+1] =
{
	2, // nzElementUsage_Diffuse
	1, // nzElementUsage_Normal
	0, // nzElementUsage_Position
	3, // nzElementUsage_Tangent

	4  // nzElementUsage_TexCoord (Doit être le dernier de la liste car extensible)
};

GLenum NzOpenGL::BlendFunc[nzBlendFunc_Max+1] =
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

GLenum NzOpenGL::BufferLock[nzBufferAccess_Max+1] =
{
	GL_WRITE_ONLY, // nzBufferAccess_DiscardAndWrite
	GL_READ_ONLY,  // nzBufferAccess_ReadOnly
	GL_READ_WRITE, // nzBufferAccess_ReadWrite
	GL_WRITE_ONLY  // nzBufferAccess_WriteOnly
};

GLenum NzOpenGL::BufferLockRange[nzBufferAccess_Max+1] =
{
	GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_WRITE_BIT, // nzBufferAccess_DiscardAndWrite
	GL_MAP_READ_BIT,                                 // nzBufferAccess_ReadOnly
	GL_MAP_READ_BIT | GL_MAP_WRITE_BIT,              // nzBufferAccess_ReadWrite
	GL_MAP_WRITE_BIT                                 // nzBufferAccess_WriteOnly
};

GLenum NzOpenGL::BufferTarget[nzBufferType_Max+1] =
{
	GL_ELEMENT_ARRAY_BUFFER, // nzBufferType_Index,
	GL_ARRAY_BUFFER,		 // nzBufferType_Vertex
};

GLenum NzOpenGL::BufferTargetBinding[nzBufferType_Max+1] =
{
	GL_ELEMENT_ARRAY_BUFFER_BINDING, // nzBufferType_Index,
	GL_ARRAY_BUFFER_BINDING,		 // nzBufferType_Vertex
};

GLenum NzOpenGL::BufferUsage[nzBufferUsage_Max+1] =
{
	// J'ai choisi DYNAMIC à la place de STREAM car DYNAMIC semble plus adapté au profil "une mise à jour pour quelques rendus"
	// Ce qui est je pense le scénario qui arrivera le plus souvent (Prévoir une option pour permettre d'utiliser le STREAM_DRAW ?)
	// Source: http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=160839
	GL_DYNAMIC_DRAW, // nzBufferUsage_Dynamic
	GL_STATIC_DRAW   // nzBufferUsage_Static
};

GLenum NzOpenGL::CubemapFace[6] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X, // nzCubemapFace_PositiveX
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // nzCubemapFace_NegativeX
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // nzCubemapFace_PositiveY (Inversion pour les standards OpenGL)
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // nzCubemapFace_NegativeY (Inversion pour les standards OpenGL)
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // nzCubemapFace_PositiveZ
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // nzCubemapFace_NegativeZ
};

GLenum NzOpenGL::ElementType[nzElementType_Max+1] =
{
	GL_UNSIGNED_BYTE, // nzElementType_Color
	GL_DOUBLE,        // nzElementType_Double1
	GL_DOUBLE,        // nzElementType_Double2
	GL_DOUBLE,        // nzElementType_Double3
	GL_DOUBLE,        // nzElementType_Double4
	GL_FLOAT,         // nzElementType_Float1
	GL_FLOAT,         // nzElementType_Float2
	GL_FLOAT,         // nzElementType_Float3
	GL_FLOAT          // nzElementType_Float4
};

GLenum NzOpenGL::FaceCulling[nzFaceCulling_Max+1] =
{
	GL_BACK,          // nzFaceCulling_Back
	GL_FRONT,	      // nzFaceCulling_Front
	GL_FRONT_AND_BACK // nzFaceCulling_FrontAndBack
};

GLenum NzOpenGL::FaceFilling[nzFaceFilling_Max+1] =
{
	GL_POINT, // nzFaceFilling_Point
	GL_LINE,  // nzFaceFilling_Line
	GL_FILL   // nzFaceFilling_Fill
};

GLenum NzOpenGL::PrimitiveMode[nzPrimitiveMode_Max+1] =
{
	GL_LINES,          // nzPrimitiveMode_LineList
	GL_LINE_STRIP,     // nzPrimitiveMode_LineStrip
	GL_POINTS,         // nzPrimitiveMode_PointList
	GL_TRIANGLES,      // nzPrimitiveMode_TriangleList
	GL_TRIANGLE_STRIP, // nzPrimitiveMode_TriangleStrip
	GL_TRIANGLE_FAN    // nzPrimitiveMode_TriangleFan
};

GLenum NzOpenGL::RendererComparison[nzRendererComparison_Max+1] =
{
	GL_ALWAYS,  // nzRendererComparison_Always
	GL_EQUAL,   // nzRendererComparison_Equal
	GL_GREATER, // nzRendererComparison_Greater
	GL_GEQUAL,  // nzRendererComparison_GreaterOrEqual
	GL_LESS,    // nzRendererComparison_Less
	GL_LEQUAL,  // nzRendererComparison_LessOrEqual
	GL_NEVER    // nzRendererComparison_Never
};

GLenum NzOpenGL::RendererParameter[nzRendererParameter_Max+1] =
{
	GL_BLEND,        // nzRendererParameter_Blend
	GL_NONE,         // nzRendererParameter_ColorWrite
	GL_DEPTH_TEST,   // nzRendererParameter_DepthBuffer
	GL_NONE,         // nzRendererParameter_DepthWrite
	GL_CULL_FACE,    // nzRendererParameter_FaceCulling
	GL_SCISSOR_TEST, // nzRendererParameter_ScissorTest
	GL_STENCIL_TEST  // nzRendererParameter_StencilTest
};

GLenum NzOpenGL::SamplerWrapMode[nzSamplerWrap_Max+1] =
{
	GL_CLAMP_TO_EDGE,   // nzTextureWrap_Clamp
	GL_MIRRORED_REPEAT, // nzSamplerWrap_MirroredRepeat
	GL_REPEAT           // nzTextureWrap_Repeat
};

GLenum NzOpenGL::ShaderType[nzShaderType_Max+1] =
{
	GL_FRAGMENT_SHADER,	// nzShaderType_Fragment
	GL_GEOMETRY_SHADER,	// nzShaderType_Geometry
	GL_VERTEX_SHADER	// nzShaderType_Vertex
};

GLenum NzOpenGL::StencilOperation[nzStencilOperation_Max+1] =
{
	GL_DECR,      // nzStencilOperation_Decrement
	GL_DECR_WRAP, // nzStencilOperation_DecrementToSaturation
	GL_INCR,      // nzStencilOperation_Increment
	GL_INCR_WRAP, // nzStencilOperation_IncrementToSaturation
	GL_INVERT,    // nzStencilOperation_Invert
	GL_KEEP,      // nzStencilOperation_Keep
	GL_REPLACE,   // nzStencilOperation_Replace
	GL_ZERO       // nzStencilOperation_Zero
};

GLenum NzOpenGL::TextureTarget[nzImageType_Max+1] =
{
	GL_TEXTURE_1D,       // nzImageType_1D
	GL_TEXTURE_1D_ARRAY, // nzImageType_1D_Array
	GL_TEXTURE_2D,       // nzImageType_2D
	GL_TEXTURE_2D_ARRAY, // nzImageType_2D_Array
	GL_TEXTURE_3D,       // nzImageType_3D
	GL_TEXTURE_CUBE_MAP  // nzImageType_Cubemap
};

GLenum NzOpenGL::TextureTargetBinding[nzImageType_Max+1] =
{
	GL_TEXTURE_BINDING_1D,       // nzImageType_1D
	GL_TEXTURE_BINDING_1D_ARRAY, // nzImageType_1D_Array
	GL_TEXTURE_BINDING_2D,       // nzImageType_2D
	GL_TEXTURE_BINDING_2D_ARRAY, // nzImageType_2D_Array
	GL_TEXTURE_BINDING_3D,       // nzImageType_3D
	GL_TEXTURE_BINDING_CUBE_MAP  // nzImageType_Cubemap
};

GLenum NzOpenGL::TextureTargetProxy[nzImageType_Max+1] =
{
	GL_PROXY_TEXTURE_1D,       // nzImageType_1D
	GL_PROXY_TEXTURE_1D_ARRAY, // nzImageType_1D_Array
	GL_PROXY_TEXTURE_2D,       // nzImageType_2D
	GL_PROXY_TEXTURE_2D_ARRAY, // nzImageType_2D_Array
	GL_PROXY_TEXTURE_3D,       // nzImageType_3D
	GL_PROXY_TEXTURE_CUBE_MAP  // nzImageType_Cubemap
};

PFNGLACTIVETEXTUREPROC            glActiveTexture            = nullptr;
PFNGLATTACHSHADERPROC             glAttachShader             = nullptr;
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
PFNGLGETBOOLEANVPROC              glGetBooleanv              = nullptr;
PFNGLGETBUFFERPARAMETERIVPROC     glGetBufferParameteriv     = nullptr;
PFNGLGETDEBUGMESSAGELOGPROC       glGetDebugMessageLog       = nullptr;
PFNGLGETERRORPROC                 glGetError                 = nullptr;
PFNGLGETFLOATVPROC                glGetFloatv                = nullptr;
PFNGLGETINTEGERVPROC              glGetIntegerv              = nullptr;
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
PFNGLPROGRAMUNIFORM1DPROC         glProgramUniform1d         = nullptr;
PFNGLPROGRAMUNIFORM1FPROC         glProgramUniform1f         = nullptr;
PFNGLPROGRAMUNIFORM1IPROC         glProgramUniform1i         = nullptr;
PFNGLPROGRAMUNIFORM2DVPROC        glProgramUniform2dv        = nullptr;
PFNGLPROGRAMUNIFORM2FVPROC        glProgramUniform2fv        = nullptr;
PFNGLPROGRAMUNIFORM3DVPROC        glProgramUniform3dv        = nullptr;
PFNGLPROGRAMUNIFORM3FVPROC        glProgramUniform3fv        = nullptr;
PFNGLPROGRAMUNIFORM4DVPROC        glProgramUniform4dv        = nullptr;
PFNGLPROGRAMUNIFORM4FVPROC        glProgramUniform4fv        = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC  glProgramUniformMatrix4dv  = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC  glProgramUniformMatrix4fv  = nullptr;
PFNGLREADPIXELSPROC               glReadPixels               = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC      glRenderbufferStorage      = nullptr;
PFNGLSAMPLERPARAMETERFPROC        glSamplerParameterf        = nullptr;
PFNGLSAMPLERPARAMETERIPROC        glSamplerParameteri        = nullptr;
PFNGLSCISSORPROC                  glScissor                  = nullptr;
PFNGLSHADERSOURCEPROC             glShaderSource             = nullptr;
PFNGLSTENCILFUNCPROC              glStencilFunc              = nullptr;
PFNGLSTENCILOPPROC                glStencilOp                = nullptr;
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
PFNGLUNIFORM2DVPROC               glUniform2dv               = nullptr;
PFNGLUNIFORM2FVPROC               glUniform2fv               = nullptr;
PFNGLUNIFORM3DVPROC               glUniform3dv               = nullptr;
PFNGLUNIFORM3FVPROC               glUniform3fv               = nullptr;
PFNGLUNIFORM4DVPROC               glUniform4dv               = nullptr;
PFNGLUNIFORM4FVPROC               glUniform4fv               = nullptr;
PFNGLUNIFORMMATRIX4DVPROC         glUniformMatrix4dv         = nullptr;
PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv         = nullptr;
PFNGLUNMAPBUFFERPROC              glUnmapBuffer              = nullptr;
PFNGLUSEPROGRAMPROC               glUseProgram               = nullptr;
PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f           = nullptr;
PFNGLVERTEXATTRIBDIVISORPROC      glVertexAttribDivisor      = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer      = nullptr;
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
