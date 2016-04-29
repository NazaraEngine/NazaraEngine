TOOL.Name = "Assimp"

TOOL.Directory = "../SDK/lib"
TOOL.Kind = "Plugin"

TOOL.Includes = {
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
