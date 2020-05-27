MODULE.Name = "Renderer"

MODULE.ClientOnly = true

MODULE.Defines = {
	"NAZARA_RENDERER_OPENGL"
}

MODULE.Libraries = {
	"NazaraCore",
	"NazaraUtility",
	"NazaraPlatform"
}

if (Config.PlatformSDL2) then
    table.insert(MODULE.Defines, "NAZARA_PLATFORM_SDL2")

    table.insert(MODULE.Files, "../src/Nazara/Renderer/SDL2/**.hpp")
    table.insert(MODULE.Files, "../src/Nazara/Renderer/SDL2/**.cpp")

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
        "../src/Nazara/Renderer/Win32/**",
        "../src/Nazara/Renderer/GLX/**.cpp"
    }
else
    MODULE.OsFiles.Windows = {
        "../src/Nazara/Renderer/Win32/**.hpp",
        "../src/Nazara/Renderer/Win32/**.cpp"
    }

    MODULE.OsFiles.Posix = {
        "../src/Nazara/Renderer/GLX/**.hpp",
        "../src/Nazara/Renderer/GLX/**.cpp"
    }

    MODULE.OsLibraries.Windows = {
        "gdi32",
        "opengl32",
        "winmm"
    }

    MODULE.OsLibraries.Posix = {
        "GL",
        "X11"
    }
end
