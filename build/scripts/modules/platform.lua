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

MODULE.OsDefines.Windows = {
	"SDL_VIDEO_DRIVER_WINDOWS=1"
}

MODULE.OsDefines.Posix = {
	"SDL_VIDEO_DRIVER_X11=1",
	"SDL_VIDEO_DRIVER_WAYLAND=1",
}
