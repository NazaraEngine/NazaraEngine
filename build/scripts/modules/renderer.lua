MODULE.Name = "Renderer"

MODULE.ClientOnly = true

MODULE.Libraries = {
	"NazaraCore",
	"NazaraShader",
	"NazaraUtility",
	"NazaraPlatform"
}

MODULE.OsFiles.Windows = {
    "../src/Nazara/Renderer/Win32/**.hpp",
    "../src/Nazara/Renderer/Win32/**.cpp"
}

MODULE.OsFiles.Posix = {
    "../src/Nazara/Renderer/GLX/**.hpp",
    "../src/Nazara/Renderer/GLX/**.cpp"
}

