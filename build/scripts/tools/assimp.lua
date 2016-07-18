TOOL.Name = "Assimp"

TOOL.Directory = "../plugins/Assimp"
TOOL.Kind = "Plugin"
TOOL.TargetDirectory = "../SDK/lib"

TOOL.Includes = {
	"../extlibs/include",
	"../include",
    "../plugins/Assimp"
}

TOOL.Files = {
	"../plugins/Assimp/**.hpp",
	"../plugins/Assimp/**.inl",
	"../plugins/Assimp/**.cpp"
}

TOOL.Libraries = {
	"NazaraCore",
	"NazaraUtility",
	"assimp"
}
