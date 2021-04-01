EXAMPLE.Name = "Tut00_EmptyProject"

EXAMPLE.EnableConsole = true

EXAMPLE.Files = {
	"main.cpp"
}

EXAMPLE.Libraries = {
    "NazaraAudio",
    "NazaraClientSDK",
    "NazaraCore",
    "NazaraGraphics",
    "NazaraLua",
    "NazaraNetwork",
    "NazaraNoise",
    "NazaraPhysics2D",
    "NazaraPhysics3D",
    "NazaraPlatform",
    "NazaraRenderer",
    "NazaraUtility",
    "NazaraSDK"
}

if Config.PlatformSDL2 then
	table.insert(EXAMPLE.Defines, "NAZARA_PLATFORM_SDL2")
end
