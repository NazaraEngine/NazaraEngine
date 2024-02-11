EXAMPLE.Name = "MeshInfos"

EXAMPLE.EnableConsole = true

EXAMPLE.Files = {
	"main.cpp"
}

EXAMPLE.Libraries = {
	"NazaraCore",
	"NazaraPlatform"
}

if Config.PlatformSDL2 then
	table.insert(EXAMPLE.Defines, "NAZARA_PLATFORM_SDL2")
end
