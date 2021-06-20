EXAMPLE.Name = "Tut01_HelloWorld"

EXAMPLE.EnableConsole = true

EXAMPLE.Files = {
	"main.cpp"
}

EXAMPLE.Libraries = {
	"NazaraSDK"
}

if Config.PlatformSDL2 then
	table.insert(EXAMPLE.Defines, "NAZARA_PLATFORM_SDL2")
end
