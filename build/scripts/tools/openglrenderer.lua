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
	"NazaraRenderer",
	"NazaraUtility"
}

TOOL.OsFiles.Windows = {
	"../src/Nazara/OpenGLRenderer/Win32/**.hpp",
	"../src/Nazara/OpenGLRenderer/Win32/**.cpp"
}
