MODULE.Name = "Renderer"

MODULE.ClientOnly = true

MODULE.Defines = {
	"NAZARA_RENDERER_OPENGL"
}

MODULE.Libraries = {
	"NazaraCore",
	"NazaraUtility"
}

MODULE.OsFiles.Windows = {
	"../src/Nazara/Renderer/Win32/**.hpp",
	"../src/Nazara/Renderer/Win32/**.cpp"
}

MODULE.OsFiles.Posix = {
	"../src/Nazara/Renderer/GLX/**.hpp",
	"../src/Nazara/Renderer/GLX/**.cpp"
}

MODULE.OsLibraries.Windows = {
	"gdi32",
	"opengl32",
	"winmm"
}

MODULE.OsLibraries.Posix = {
	"GL",
	"X11"
}
