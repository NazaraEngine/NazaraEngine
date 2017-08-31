TOOL.Name = "UnitTestsServer"

TOOL.Directory = "../tests"
TOOL.EnableConsole = true
TOOL.Kind = "Application"
TOOL.TargetDirectory = TOOL.Directory

TOOL.Defines = {
	"NDK_SERVER"
}

TOOL.Includes = {
	"../include"
}

TOOL.Files = {
	"../tests/main.cpp",
	"../tests/Engine/**.hpp",
	"../tests/Engine/**.cpp",
	"../tests/SDK/**.hpp",
	"../tests/SDK/**.cpp"
}

-- Excludes client-only files
TOOL.FilesExcluded = {
	"../tests/Engine/Audio/**",
	"../tests/Engine/Graphics/**",
	"../tests/Engine/Platform/**",
	"../tests/SDK/NDK/Application.cpp",
	"../tests/SDK/NDK/Systems/ListenerSystem.cpp",
	"../tests/SDK/NDK/Systems/RenderSystem.cpp"
}

TOOL.Libraries = {
	"NazaraNetwork",
	"NazaraSDKServer"
}
