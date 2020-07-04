TOOL.Name = "ShaderNodes"

TOOL.ClientOnly = true
TOOL.EnableConsole = true
TOOL.Kind = "Application"
TOOL.TargetDirectory = "../bin"

TOOL.Defines = {
	"NODE_EDITOR_SHARED"
}

TOOL.Includes = {
	"../include",
	"../extlibs/include",
	"../src",
	[[C:\Projets\Libs\Qt\5.15.0\msvc2019_64\include]],
	[[C:\Projets\Libs\Qt\5.15.0\msvc2019_64\include\QtCore]],
	[[C:\Projets\Libs\Qt\5.15.0\msvc2019_64\include\QtGui]],
	[[C:\Projets\Libs\Qt\5.15.0\msvc2019_64\include\QtWidgets]],
	[[C:\Projets\Libs\nodeeditor\include]],
}

TOOL.Files = {
	"../src/ShaderNode/**.hpp",
	"../src/ShaderNode/**.inl",
	"../src/ShaderNode/**.cpp"
}

TOOL.Libraries = {
	"NazaraCore",
	"NazaraRenderer",
	"NazaraUtility",
	"Qt5Cored",
	"Qt5Guid",
	"Qt5Widgetsd",
	"nodes"
}

TOOL.LibraryPaths.x64 = {
	[[C:\Projets\Libs\Qt\5.15.0\msvc2019_64\lib]],
	[[C:\Projets\Libs\nodeeditor\build\lib\Debug]]
}
