MODULE.Name = "Renderer"

MODULE.ClientOnly = true

MODULE.Defines = {
	"NAZARA_RENDERER_OPENGL"
}

MODULE.Libraries = {
	"NazaraCore",
	"NazaraUtility",
	"NazaraPlatform",
    "SDL2"
}

MODULE.Files = {
    "../src/Nazara/Renderer/SDL2/**.hpp",
    "../src/Nazara/Renderer/SDL2/**.cpp"
}
