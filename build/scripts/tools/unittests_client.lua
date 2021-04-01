TOOL.Name = "ClientUnitTests"

TOOL.Category = "Test"
TOOL.Directory = "../tests"
TOOL.EnableConsole = true
TOOL.Kind = "Application"
TOOL.TargetDirectory = TOOL.Directory

TOOL.Defines = {
}

TOOL.Includes = {
	"../include"
}

TOOL.Files = {
	"../tests/main_client.cpp",
	"../tests/Engine/Audio/**",
	"../tests/Engine/Graphics/**",
	"../tests/Engine/Platform/**",
	"../tests/SDK/NDK/Application.cpp",
	"../tests/SDK/NDK/Systems/ListenerSystem.cpp",
	"../tests/SDK/NDK/Systems/RenderSystem.cpp"
}

TOOL.Libraries = {
	"NazaraNetwork",
	"NazaraClientSDK"
}
