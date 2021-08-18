EXAMPLE.Name = "Particles"

EXAMPLE.EnableConsole = true

EXAMPLE.Files = {
	"*.hpp",
	"*.inl",
	"*.cpp"
}

EXAMPLE.Libraries = {
	"NazaraSDK"
}

if Config.PlatformSDL2 then
	table.insert(EXAMPLE.Defines, "NAZARA_PLATFORM_SDL2")
end
