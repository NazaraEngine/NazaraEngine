TOOL.Name = "OpenGLRenderer"

TOOL.ClientOnly = true

TOOL.Kind = "Library"
TOOL.TargetDirectory = "../lib"

TOOL.Defines = {
	"NAZARA_BUILD",
	"NAZARA_OPENGLRENDERER_BUILD"
}

TOOL.Includes = {
	"../include",
	"../src/",
	"../extlibs/include"
}

TOOL.Files = {
	"../include/Nazara/OpenGLRenderer/**.hpp",
	"../include/Nazara/OpenGLRenderer/**.inl",
	"../src/Nazara/OpenGLRenderer/**.hpp",
	"../src/Nazara/OpenGLRenderer/**.inl",
	"../src/Nazara/OpenGLRenderer/**.cpp"
}

TOOL.Libraries = {
	"NazaraCore",
	"NazaraPlatform",
	"NazaraRenderer",
	"NazaraShader",
	"NazaraUtility"
}

TOOL.OsFiles.Windows = {
	"../include/Nazara/OpenGLRenderer/Wrapper/Win32/**.hpp",
	"../include/Nazara/OpenGLRenderer/Wrapper/Win32/**.inl",
	"../include/Nazara/OpenGLRenderer/Wrapper/WGL/**.hpp",
	"../include/Nazara/OpenGLRenderer/Wrapper/WGL/**.inl",
	"../src/Nazara/OpenGLRenderer/Wrapper/Win32/**.hpp",
	"../src/Nazara/OpenGLRenderer/Wrapper/Win32/**.inl",
	"../src/Nazara/OpenGLRenderer/Wrapper/Win32/**.cpp",
	"../src/Nazara/OpenGLRenderer/Wrapper/WGL/**.hpp",
	"../src/Nazara/OpenGLRenderer/Wrapper/WGL/**.inl",
	"../src/Nazara/OpenGLRenderer/Wrapper/WGL/**.cpp"
}

TOOL.OsFiles.Linux = {
	"../include/Nazara/OpenGLRenderer/Wrapper/Linux/**.hpp",
	"../include/Nazara/OpenGLRenderer/Wrapper/Linux/**.inl",
	"../src/Nazara/OpenGLRenderer/Wrapper/Linux/**.hpp",
	"../src/Nazara/OpenGLRenderer/Wrapper/Linux/**.inl",
	"../src/Nazara/OpenGLRenderer/Wrapper/Linux/**.cpp"
}
