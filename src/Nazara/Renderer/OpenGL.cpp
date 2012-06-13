#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
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

	std::set<NzString> openGLextensionSet;
	bool openGLextensions[NzOpenGL::Count] = {false};
	unsigned int openGLversion = 0;

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
			openGLextensionSet.insert(*it);

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

			openGLextensionSet.insert(extension);
		}

		return true;
	}
}

unsigned int NzOpenGL::GetVersion()
{
	return openGLversion;
}

bool NzOpenGL::Initialize()
{
	if (!LoadLibrary())
	{
		NazaraError("Failed to load OpenGL library");
		return false;
	}

	// Le chargement des fonctions OpenGL nécessite un contexte OpenGL
	NzContextParameters parameters;
	parameters.majorVersion = 2;
	parameters.minorVersion = 0;
	parameters.shared = false;

	/*
		Note: Même le contexte de chargement nécessite quelques fonctions de base pour correctement s'initialiser
		Pour cette raison, deux contextes sont créés, le premier sert à récupérer les fonctions permetttant
		de créer le second avec les bons paramètres.s

		Non sérieusement si quelqu'un a une meilleure idée qu'il me le dise
	*/

	/****************************************Initialisation****************************************/

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
	glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(LoadEntry("glGetString"));
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

	openGLversion = major*100 + minor*10;

	parameters.debugMode = true; // Certaines extensions n'apparaissent qu'avec un contexte de debug (e.g. ARB_debug_output)
	parameters.majorVersion = NzContextParameters::defaultMajorVersion = openGLversion/100;
	parameters.minorVersion = NzContextParameters::defaultMinorVersion = (openGLversion%100)/10;

	// Destruction implicite du premier contexte
	if (!loadContext.Create(parameters))
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
		glFlush = reinterpret_cast<PFNGLFLUSHPROC>(LoadEntry("glFlush"));
		glEnable = reinterpret_cast<PFNGLENABLEPROC>(LoadEntry("glEnable"));
		glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(LoadEntry("glEnableVertexAttribArray"));
		glEndQuery = reinterpret_cast<PFNGLENDQUERYPROC>(LoadEntry("glEndQuery"));
		glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(LoadEntry("glGenBuffers"));
		glGenQueries = reinterpret_cast<PFNGLGENQUERIESPROC>(LoadEntry("glGenQueries"));
		glGenTextures = reinterpret_cast<PFNGLGENTEXTURESPROC>(LoadEntry("glGenTextures"));
		glGetBufferParameteriv = reinterpret_cast<PFNGLGETBUFFERPARAMETERIVPROC>(LoadEntry("glGetBufferParameteriv"));
		glGetError = reinterpret_cast<PFNGLGETERRORPROC>(LoadEntry("glGetError"));
		glGetQueryiv = reinterpret_cast<PFNGLGETQUERYIVPROC>(LoadEntry("glGetQueryiv"));
		glGetQueryObjectiv = reinterpret_cast<PFNGLGETQUERYOBJECTIVPROC>(LoadEntry("glGetQueryObjectiv"));
		glGetQueryObjectuiv = reinterpret_cast<PFNGLGETQUERYOBJECTUIVPROC>(LoadEntry("glGetQueryObjectuiv"));
		glGetIntegerv = reinterpret_cast<PFNGLGETINTEGERVPROC>(LoadEntry("glGetIntegerv"));
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
		glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(LoadEntry("glLinkProgram"));
		glMapBuffer = reinterpret_cast<PFNGLMAPBUFFERPROC>(LoadEntry("glMapBuffer"));
		glPolygonMode = reinterpret_cast<PFNGLPOLYGONMODEPROC>(LoadEntry("glPolygonMode"));
		glReadPixels = reinterpret_cast<PFNGLREADPIXELSPROC>(LoadEntry("glReadPixels"));
		glScissor = reinterpret_cast<PFNGLSCISSORPROC>(LoadEntry("glScissor"));
		glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(LoadEntry("glShaderSource"));
		glStencilFunc = reinterpret_cast<PFNGLSTENCILFUNCPROC>(LoadEntry("glStencilFunc"));
		glStencilOp = reinterpret_cast<PFNGLSTENCILOPPROC>(LoadEntry("glStencilOp"));
		glTexImage2D = reinterpret_cast<PFNGLTEXIMAGE2DPROC>(LoadEntry("glTexImage2D"));
		glTexParameterf = reinterpret_cast<PFNGLTEXPARAMETERFPROC>(LoadEntry("glTexParameterf"));
		glTexParameteri = reinterpret_cast<PFNGLTEXPARAMETERIPROC>(LoadEntry("glTexParameteri"));
		glTexSubImage2D = reinterpret_cast<PFNGLTEXSUBIMAGE2DPROC>(LoadEntry("glTexSubImage2D"));
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

	// Fonctions optionnelles
	glGetStringi = reinterpret_cast<PFNGLGETSTRINGIPROC>(LoadEntry("glGetStringi", false));
	glMapBufferRange = reinterpret_cast<PFNGLMAPBUFFERRANGEPROC>(LoadEntry("glMapBufferRange", false));

	#if defined(NAZARA_PLATFORM_WINDOWS)
	wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(LoadEntry("wglGetExtensionsStringARB", false));
	wglGetExtensionsStringEXT = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(LoadEntry("wglGetExtensionsStringEXT", false));
	wglSwapInterval = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(LoadEntry("wglSwapIntervalEXT", false));
	#elif defined(NAZARA_PLATFORM_LINUX)
	glXSwapInterval = reinterpret_cast<PFNGLXSWAPINTERVALSGIPROC>(LoadEntry("glXSwapIntervalSGI", false));
	#endif

	/****************************************Extensions****************************************/

	if (!glGetStringi || !LoadExtensions3())
	{
		if (openGLversion >= 300) // Dans le cas contraire c'est normal
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
			NazaraWarning("Failed to load windows' extension string");
	}
	#endif

	// AnisotropicFilter
	openGLextensions[NzOpenGL::AnisotropicFilter] = IsSupported("GL_EXT_texture_filter_anisotropic");

	// DebugOutput
	if (IsSupported("GL_ARB_debug_output"))
	{
		try
		{
			glDebugMessageControl = reinterpret_cast<PFNGLDEBUGMESSAGECONTROLARBPROC>(LoadEntry("glDebugMessageControlARB"));
			glDebugMessageInsert = reinterpret_cast<PFNGLDEBUGMESSAGEINSERTARBPROC>(LoadEntry("glDebugMessageInsertARB"));
			glDebugMessageCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKARBPROC>(LoadEntry("glDebugMessageCallbackARB"));
			glGetDebugMessageLog = reinterpret_cast<PFNGLGETDEBUGMESSAGELOGARBPROC>(LoadEntry("glGetDebugMessageLogARB"));

			openGLextensions[NzOpenGL::DebugOutput] = true;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to load GL_ARB_debug_output: " + NzString(e.what()));
		}
	}

	// FP64
	if (openGLversion >= 400 || IsSupported("GL_ARB_gpu_shader_fp64"))
	{
		try
		{
			glUniform1d = reinterpret_cast<PFNGLUNIFORM1DPROC>(LoadEntry("glUniform1d"));
			glUniform2dv = reinterpret_cast<PFNGLUNIFORM2DVPROC>(LoadEntry("glUniform2dv"));
			glUniform3dv = reinterpret_cast<PFNGLUNIFORM3DVPROC>(LoadEntry("glUniform3dv"));
			glUniform4dv = reinterpret_cast<PFNGLUNIFORM4DVPROC>(LoadEntry("glUniform4dv"));

			openGLextensions[NzOpenGL::FP64] = true;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to load ARB_gpu_shader_fp64: " + NzString(e.what()));
		}
	}

	// FrameBufferObject
	if (openGLversion >= 300 || IsSupported("GL_ARB_framebuffer_object"))
	{
		try
		{
			glBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(LoadEntry("glBindFramebuffer"));
			glBindRenderbuffer = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(LoadEntry("glBindRenderbuffer"));
			glCheckFramebufferStatus = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(LoadEntry("glCheckFramebufferStatus"));
			glDeleteFramebuffers = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(LoadEntry("glDeleteFramebuffers"));
			glDeleteRenderbuffers = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(LoadEntry("glDeleteRenderbuffers"));
			glFramebufferRenderbuffer = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(LoadEntry("glFramebufferRenderbuffer"));
			glFramebufferTexture2D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(LoadEntry("glFramebufferTexture2D"));
			glGenerateMipmap = reinterpret_cast<PFNGLGENERATEMIPMAPPROC>(LoadEntry("glGenerateMipmap"));
			glGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(LoadEntry("glGenFramebuffers"));
			glGenRenderbuffers = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(LoadEntry("glGenRenderbuffers"));
			glRenderbufferStorage = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>(LoadEntry("glRenderbufferStorage"));

			openGLextensions[NzOpenGL::FrameBufferObject] = true;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to load ARB_framebuffer_object: (" + NzString(e.what()) + ")");
		}
	}

	// SeparateShaderObjects
	if (openGLversion >= 400 || IsSupported("GL_ARB_gpu_shader_fp64"))
	{
		glProgramUniform1f = reinterpret_cast<PFNGLPROGRAMUNIFORM1FPROC>(LoadEntry("glProgramUniform1f"));
		glProgramUniform1i = reinterpret_cast<PFNGLPROGRAMUNIFORM1IPROC>(LoadEntry("glProgramUniform1i"));
		glProgramUniform2fv = reinterpret_cast<PFNGLPROGRAMUNIFORM2FVPROC>(LoadEntry("glProgramUniform2fv"));
		glProgramUniform3fv = reinterpret_cast<PFNGLPROGRAMUNIFORM3FVPROC>(LoadEntry("glProgramUniform3fv"));
		glProgramUniform4fv = reinterpret_cast<PFNGLPROGRAMUNIFORM4FVPROC>(LoadEntry("glProgramUniform4fv"));
		glProgramUniformMatrix4fv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4FVPROC>(LoadEntry("glProgramUniformMatrix4fv"));

		if (openGLextensions[NzOpenGL::FP64])
		{
			glProgramUniform1d = reinterpret_cast<PFNGLPROGRAMUNIFORM1DPROC>(LoadEntry("glProgramUniform1d"));
			glProgramUniform2dv = reinterpret_cast<PFNGLPROGRAMUNIFORM2DVPROC>(LoadEntry("glProgramUniform2dv"));
			glProgramUniform3dv = reinterpret_cast<PFNGLPROGRAMUNIFORM3DVPROC>(LoadEntry("glProgramUniform3dv"));
			glProgramUniform4dv = reinterpret_cast<PFNGLPROGRAMUNIFORM4DVPROC>(LoadEntry("glProgramUniform4dv"));
			glProgramUniformMatrix4dv = reinterpret_cast<PFNGLPROGRAMUNIFORMMATRIX4DVPROC>(LoadEntry("glProgramUniformMatrix4dv"));
		}

		openGLextensions[NzOpenGL::SeparateShaderObjects] = true;
	}

	// Texture3D
	try
	{
		glTexImage3D = reinterpret_cast<PFNGLTEXIMAGE3DPROC>(LoadEntry("glTexImage3D"));
		glTexSubImage3D = reinterpret_cast<PFNGLTEXSUBIMAGE3DPROC>(LoadEntry("glTexSubImage3D"));

		openGLextensions[NzOpenGL::Texture3D] = true;
	}
	catch (const std::exception& e)
	{
		if (openGLversion >= 120)
			NazaraWarning("Failed to load core texture 3D (" + NzString(e.what()) + ")");

		if (IsSupported("GL_EXT_texture3D"))
		{
			try
			{
				// Hacky: Normalement incompatible à cause du internalFormat, GLenum pour l'extension et GLint pour le noyau
				// Mais la taille du type étant la même (GLenum est un typedef équivalent à GLint) et Nazara n'utilisant pas
				// Ce qui cause l'incompatibilité (les paramètres 1,2,3,4), je prends cette liberté
				glTexImage3D = reinterpret_cast<PFNGLTEXIMAGE3DPROC>(LoadEntry("glTexImage3DEXT"));
				glTexSubImage3D = reinterpret_cast<PFNGLTEXSUBIMAGE3DEXTPROC>(LoadEntry("glTexSubImage3DEXT"));

				openGLextensions[NzOpenGL::Texture3D] = true;
			}
			catch (const std::exception& e)
			{
				NazaraError("Failed to load EXT_texture3D: " + NzString(e.what()));
			}
		}
	}

	// TextureCompression_s3tc
	openGLextensions[NzOpenGL::TextureCompression_s3tc] = IsSupported("GL_EXT_texture_compression_s3tc");

	// TextureStorage
	if (openGLversion >= 420 || IsSupported("GL_ARB_texture_storage"))
	{
		try
		{
			glTexStorage1D = reinterpret_cast<PFNGLTEXSTORAGE1DPROC>(LoadEntry("glTexStorage1D"));
			glTexStorage2D = reinterpret_cast<PFNGLTEXSTORAGE2DPROC>(LoadEntry("glTexStorage2D"));
			glTexStorage3D = reinterpret_cast<PFNGLTEXSTORAGE3DPROC>(LoadEntry("glTexStorage3D"));

			openGLextensions[NzOpenGL::TextureStorage] = true;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to load ARB_texture_storage: " + NzString(e.what()));
		}
	}

	// VertexArrayObject
	if (openGLversion >= 300 || IsSupported("GL_ARB_vertex_array_object"))
	{
		try
		{
			glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(LoadEntry("glBindVertexArray"));
			glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(LoadEntry("glDeleteVertexArrays"));
			glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(LoadEntry("glGenVertexArrays"));

			openGLextensions[NzOpenGL::VertexArrayObject] = true;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to load ARB_vertex_array_object: " + NzString(e.what()));
		}
	}

	/****************************************Contexte de référence****************************************/

	///FIXME: Utiliser le contexte de chargement comme référence ? (Vérifier mode debug)
	if (!NzContext::Initialize())
	{
		NazaraError("Failed to initialize contexts");
		Uninitialize();

		return false;
	}

	return true;
}

bool NzOpenGL::IsSupported(Extension extension)
{
	return openGLextensions[extension];
}

bool NzOpenGL::IsSupported(const NzString& string)
{
	return openGLextensionSet.find(string) != openGLextensionSet.end();
}

void NzOpenGL::Uninitialize()
{
	NzContext::Uninitialize();

	for (bool& ext : openGLextensions)
		ext = false;

	openGLextensionSet.clear();
	openGLversion = 0;

	UnloadLibrary();
}

PFNGLACTIVETEXTUREPROC			  glActiveTexture			 = nullptr;
PFNGLATTACHSHADERPROC			  glAttachShader			 = nullptr;
PFNGLBEGINQUERYPROC				  glBeginQuery				 = nullptr;
PFNGLBINDATTRIBLOCATIONPROC		  glBindAttribLocation		 = nullptr;
PFNGLBINDBUFFERPROC				  glBindBuffer				 = nullptr;
PFNGLBINDFRAMEBUFFERPROC		  glBindFramebuffer			 = nullptr;
PFNGLBINDRENDERBUFFERPROC		  glBindRenderbuffer		 = nullptr;
PFNGLBINDTEXTUREPROC			  glBindTexture				 = nullptr;
PFNGLBINDVERTEXARRAYPROC		  glBindVertexArray			 = nullptr;
PFNGLBLENDFUNCPROC				  glBlendFunc				 = nullptr;
PFNGLBUFFERDATAPROC				  glBufferData				 = nullptr;
PFNGLBUFFERSUBDATAPROC			  glBufferSubData			 = nullptr;
PFNGLCLEARPROC					  glClear					 = nullptr;
PFNGLCLEARCOLORPROC				  glClearColor				 = nullptr;
PFNGLCLEARDEPTHPROC				  glClearDepth				 = nullptr;
PFNGLCLEARSTENCILPROC			  glClearStencil			 = nullptr;
PFNGLCREATEPROGRAMPROC			  glCreateProgram			 = nullptr;
PFNGLCREATESHADERPROC			  glCreateShader			 = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC	  glCheckFramebufferStatus	 = nullptr;
PFNGLCOLORMASKPROC				  glColorMask				 = nullptr;
PFNGLCULLFACEPROC				  glCullFace				 = nullptr;
PFNGLCOMPILESHADERPROC			  glCompileShader			 = nullptr;
PFNGLCOPYTEXSUBIMAGE2DPROC		  glCopyTexSubImage2D		 = nullptr;
PFNGLDEBUGMESSAGECONTROLARBPROC	  glDebugMessageControl		 = nullptr;
PFNGLDEBUGMESSAGEINSERTARBPROC	  glDebugMessageInsert		 = nullptr;
PFNGLDEBUGMESSAGECALLBACKARBPROC  glDebugMessageCallback	 = nullptr;
PFNGLDELETEBUFFERSPROC			  glDeleteBuffers			 = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC		  glDeleteFramebuffers		 = nullptr;
PFNGLDELETEPROGRAMPROC			  glDeleteProgram			 = nullptr;
PFNGLDELETEQUERIESPROC			  glDeleteQueries			 = nullptr;
PFNGLDELETERENDERBUFFERSPROC	  glDeleteRenderbuffers		 = nullptr;
PFNGLDELETESHADERPROC			  glDeleteShader			 = nullptr;
PFNGLDELETETEXTURESPROC			  glDeleteTextures			 = nullptr;
PFNGLDELETEVERTEXARRAYSPROC		  glDeleteVertexArrays		 = nullptr;
PFNGLDEPTHFUNCPROC				  glDepthFunc				 = nullptr;
PFNGLDEPTHMASKPROC				  glDepthMask				 = nullptr;
PFNGLDISABLEPROC 				  glDisable					 = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLDRAWARRAYSPROC				  glDrawArrays				 = nullptr;
PFNGLDRAWBUFFERPROC				  glDrawBuffer				 = nullptr;
PFNGLDRAWBUFFERSPROC			  glDrawBuffers				 = nullptr;
PFNGLDRAWELEMENTSPROC			  glDrawElements			 = nullptr;
PFNGLENDQUERYPROC				  glEndQuery				 = nullptr;
PFNGLFLUSHPROC					  glFlush					 = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC  glFramebufferRenderbuffer	 = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC	  glFramebufferTexture2D	 = nullptr;
PFNGLENABLEPROC					  glEnable					 = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray	 = nullptr;
PFNGLGENERATEMIPMAPPROC			  glGenerateMipmap			 = nullptr;
PFNGLGENBUFFERSPROC				  glGenBuffers				 = nullptr;
PFNGLGENFRAMEBUFFERSPROC		  glGenFramebuffers			 = nullptr;
PFNGLGENRENDERBUFFERSPROC		  glGenRenderbuffers		 = nullptr;
PFNGLGENQUERIESPROC				  glGenQueries				 = nullptr;
PFNGLGENTEXTURESPROC			  glGenTextures				 = nullptr;
PFNGLGENVERTEXARRAYSPROC		  glGenVertexArrays			 = nullptr;
PFNGLGETBUFFERPARAMETERIVPROC	  glGetBufferParameteriv	 = nullptr;
PFNGLGETDEBUGMESSAGELOGARBPROC	  glGetDebugMessageLog		 = nullptr;
PFNGLGETERRORPROC				  glGetError				 = nullptr;
PFNGLGETINTEGERVPROC			  glGetIntegerv				 = nullptr;
PFNGLGETPROGRAMIVPROC			  glGetProgramiv			 = nullptr;
PFNGLGETPROGRAMINFOLOGPROC		  glGetProgramInfoLog		 = nullptr;
PFNGLGETQUERYIVPROC				  glGetQueryiv				 = nullptr;
PFNGLGETQUERYOBJECTIVPROC		  glGetQueryObjectiv		 = nullptr;
PFNGLGETQUERYOBJECTUIVPROC		  glGetQueryObjectuiv		 = nullptr;
PFNGLGETSHADERINFOLOGPROC		  glGetShaderInfoLog		 = nullptr;
PFNGLGETSHADERIVPROC			  glGetShaderiv				 = nullptr;
PFNGLGETSHADERSOURCEPROC		  glGetShaderSource			 = nullptr;
PFNGLGETSTRINGPROC				  glGetString				 = nullptr;
PFNGLGETSTRINGIPROC				  glGetStringi				 = nullptr;
PFNGLGETTEXIMAGEPROC			  glGetTexImage				 = nullptr;
PFNGLGETTEXLEVELPARAMETERFVPROC	  glGetTexLevelParameterfv	 = nullptr;
PFNGLGETTEXLEVELPARAMETERIVPROC	  glGetTexLevelParameteriv	 = nullptr;
PFNGLGETTEXPARAMETERFVPROC		  glGetTexParameterfv		 = nullptr;
PFNGLGETTEXPARAMETERIVPROC		  glGetTexParameteriv		 = nullptr;
PFNGLGETUNIFORMLOCATIONPROC		  glGetUniformLocation		 = nullptr;
PFNGLLINKPROGRAMPROC			  glLinkProgram				 = nullptr;
PFNGLMAPBUFFERPROC				  glMapBuffer				 = nullptr;
PFNGLMAPBUFFERRANGEPROC			  glMapBufferRange			 = nullptr;
PFNGLPOLYGONMODEPROC			  glPolygonMode				 = nullptr;
PFNGLPROGRAMUNIFORM1DPROC		  glProgramUniform1d		 = nullptr;
PFNGLPROGRAMUNIFORM1FPROC		  glProgramUniform1f		 = nullptr;
PFNGLPROGRAMUNIFORM1IPROC		  glProgramUniform1i		 = nullptr;
PFNGLPROGRAMUNIFORM2DVPROC		  glProgramUniform2dv		 = nullptr;
PFNGLPROGRAMUNIFORM2FVPROC		  glProgramUniform2fv		 = nullptr;
PFNGLPROGRAMUNIFORM3DVPROC		  glProgramUniform3dv		 = nullptr;
PFNGLPROGRAMUNIFORM3FVPROC		  glProgramUniform3fv		 = nullptr;
PFNGLPROGRAMUNIFORM4DVPROC		  glProgramUniform4dv		 = nullptr;
PFNGLPROGRAMUNIFORM4FVPROC		  glProgramUniform4fv		 = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC  glProgramUniformMatrix4dv	 = nullptr;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC  glProgramUniformMatrix4fv	 = nullptr;
PFNGLREADPIXELSPROC				  glReadPixels				 = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC	  glRenderbufferStorage		 = nullptr;
PFNGLSCISSORPROC				  glScissor					 = nullptr;
PFNGLSHADERSOURCEPROC			  glShaderSource			 = nullptr;
PFNGLSTENCILFUNCPROC			  glStencilFunc				 = nullptr;
PFNGLSTENCILOPPROC				  glStencilOp				 = nullptr;
PFNGLTEXIMAGE1DPROC				  glTexImage1D				 = nullptr;
PFNGLTEXIMAGE2DPROC				  glTexImage2D				 = nullptr;
PFNGLTEXIMAGE3DPROC				  glTexImage3D				 = nullptr;
PFNGLTEXPARAMETERFPROC			  glTexParameterf			 = nullptr;
PFNGLTEXPARAMETERIPROC			  glTexParameteri			 = nullptr;
PFNGLTEXSTORAGE1DPROC			  glTexStorage1D			 = nullptr;
PFNGLTEXSTORAGE2DPROC			  glTexStorage2D			 = nullptr;
PFNGLTEXSTORAGE3DPROC			  glTexStorage3D			 = nullptr;
PFNGLTEXSUBIMAGE1DPROC			  glTexSubImage1D			 = nullptr;
PFNGLTEXSUBIMAGE2DPROC			  glTexSubImage2D			 = nullptr;
PFNGLTEXSUBIMAGE3DPROC			  glTexSubImage3D			 = nullptr;
PFNGLUNIFORM1DPROC				  glUniform1d				 = nullptr;
PFNGLUNIFORM1FPROC				  glUniform1f				 = nullptr;
PFNGLUNIFORM1IPROC				  glUniform1i				 = nullptr;
PFNGLUNIFORM2DVPROC				  glUniform2dv				 = nullptr;
PFNGLUNIFORM2FVPROC				  glUniform2fv				 = nullptr;
PFNGLUNIFORM3DVPROC				  glUniform3dv				 = nullptr;
PFNGLUNIFORM3FVPROC				  glUniform3fv				 = nullptr;
PFNGLUNIFORM4DVPROC				  glUniform4dv				 = nullptr;
PFNGLUNIFORM4FVPROC				  glUniform4fv				 = nullptr;
PFNGLUNIFORMMATRIX4DVPROC		  glUniformMatrix4dv		 = nullptr;
PFNGLUNIFORMMATRIX4FVPROC		  glUniformMatrix4fv		 = nullptr;
PFNGLUNMAPBUFFERPROC			  glUnmapBuffer				 = nullptr;
PFNGLUSEPROGRAMPROC				  glUseProgram				 = nullptr;
PFNGLVERTEXATTRIB4FPROC			  glVertexAttrib4f			 = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC	  glVertexAttribPointer		 = nullptr;
PFNGLVIEWPORTPROC				  glViewport				 = nullptr;
#if defined(NAZARA_PLATFORM_WINDOWS)
PFNWGLCHOOSEPIXELFORMATARBPROC	  wglChoosePixelFormat		 = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs	 = nullptr;
PFNWGLGETEXTENSIONSSTRINGARBPROC  wglGetExtensionsStringARB	 = nullptr;
PFNWGLGETEXTENSIONSSTRINGEXTPROC  wglGetExtensionsStringEXT	 = nullptr;
PFNWGLSWAPINTERVALEXTPROC		  wglSwapInterval			 = nullptr;
#elif defined(NAZARA_PLATFORM_LINUX)
PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribs	 = nullptr;
PFNGLXSWAPINTERVALSGIPROC		  glXSwapInterval			 = nullptr;
#endif
