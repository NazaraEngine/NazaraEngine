MODULE.Name = "Renderer"

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
	"../src/Nazara/Renderer/Posix/**.hpp",
	"../src/Nazara/Renderer/Posix/**.cpp"
}

MODULE.OsLibraries.Windows = {
	"gdi32",
	"opengl32",
	"winmm"
}
