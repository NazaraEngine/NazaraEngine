TOOL.Name = "Assimp"

TOOL.Directory = "../plugins/Assimp"
TOOL.Kind = "Plugin"

TOOL.Includes = {
	"../include",
    "../plugins/Assimp"
}

TOOL.ExtIncludes = {
	"../thirdparty/include"
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
