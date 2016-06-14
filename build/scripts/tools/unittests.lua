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
	"../tests/Engine/**.cpp"
}

TOOL.Libraries = {
	"NazaraCore",
	"NazaraAudio",
	"NazaraLua",
	"NazaraNoise",
	"NazaraPhysics",
	"NazaraUtility",
	"NazaraRenderer",
	"NazaraGraphics"
}
