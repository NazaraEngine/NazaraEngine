MODULE.Name = "Platform"

MODULE.ClientOnly = true

MODULE.Libraries = {
	"NazaraCore",
	"NazaraUtility"
}

if (Config.PlatformSDL2) then
	table.insert(MODULE.Defines, "NAZARA_PLATFORM_SDL2")

	table.insert(MODULE.Files, "../src/Nazara/Platform/SDL2/**.hpp")
	table.insert(MODULE.Files, "../src/Nazara/Platform/SDL2/**.cpp")

	table.insert(MODULE.Libraries, "SDL2")

	assert(Config.PlatformSDL2_Path)
	MODULE.Includes = Config.PlatformSDL2_Path .. "/include"
	MODULE.LibraryPaths.x64 = {
		Config.PlatformSDL2_Path .. "/lib/x64/",
		Config.PlatformSDL2_Path .. "/bin/x64/"
	}
	MODULE.LibraryPaths.x86 = {
		Config.PlatformSDL2_Path .. "/lib/x86/",
		Config.PlatformSDL2_Path .. "/bin/x86/"
	}

	MODULE.FilesExcluded = {
		"../src/Nazara/Platform/Win32/**",
		"../src/Nazara/Platform/X11/**"
	}
else
	MODULE.OsFiles.Windows = {
		"../src/Nazara/Platform/Win32/**.hpp",
		"../src/Nazara/Platform/Win32/**.cpp"
	}

	MODULE.OsFiles.Posix = {
		"../src/Nazara/Platform/X11/**.hpp",
		"../src/Nazara/Platform/X11/**.cpp"
	}

	MODULE.OsLibraries.Windows = {
		"gdi32"
	}

	MODULE.OsLibraries.Posix = {
		"X11",
		"xcb",
		"xcb-cursor",
		"xcb-ewmh",
		"xcb-icccm",
		"xcb-keysyms",
		"xcb-randr"
	}

end
