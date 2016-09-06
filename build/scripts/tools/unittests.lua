TOOL.Name = "UnitTests"

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
	"../tests/Engine/**.cpp",
	"../tests/SDK/**.cpp"
}

TOOL.Libraries = {
	"NazaraSDK"
}
