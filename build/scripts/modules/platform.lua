MODULE.Name = "Platform"

MODULE.ClientOnly = true

MODULE.Defines = {
	"NAZARA_PLATFORM_SDL2"
}

MODULE.Libraries = {
	"NazaraCore",
	"NazaraUtility",
	"SDL2"
}

MODULE.Files = {
	"../src/Nazara/Platform/SDL2/**.hpp",
	"../src/Nazara/Platform/SDL2/**.cpp"
}
