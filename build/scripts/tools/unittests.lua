TOOL.Name = "UnitTests"

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
	"../tests/main.cpp",
	"../tests/Engine/**.hpp",
	"../tests/Engine/**.cpp",
	"../tests/SDK/**.hpp",
	"../tests/SDK/**.cpp"
}

TOOL.Libraries = {
	"NazaraNetwork",
	"NazaraSDK"
}
