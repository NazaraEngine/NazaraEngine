EXAMPLE.Name = "HardwareInfo"

EXAMPLE.EnableConsole = true

EXAMPLE.Defines = {
	"NAZARA_RENDERER_OPENGL"
}

EXAMPLE.Files = {
	"main.cpp"
}

EXAMPLE.Libraries = {
	"NazaraCore",
	"NazaraPlatform",
	"NazaraRenderer",
	"NazaraUtility"
}

if (Config.PlatformSDL2) then
	table.insert(EXAMPLE.Defines, "NAZARA_PLATFORM_SDL2")

    assert(Config.PlatformSDL2_Path)
    EXAMPLE.Includes = Config.PlatformSDL2_Path .. "/include"
end
