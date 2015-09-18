TOOL.Name = "UnitTests"

TOOL.Directory = "../tests"
TOOL.Kind = "ConsoleApp"

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
