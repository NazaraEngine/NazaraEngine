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

if Config.PlatformSDL2 then
    table.insert(MODULE.Defines, "NAZARA_PLATFORM_SDL2")

    table.insert(MODULE.Files, "../src/Nazara/Renderer/SDL2/**.hpp")
    table.insert(MODULE.Files, "../src/Nazara/Renderer/SDL2/**.cpp")

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