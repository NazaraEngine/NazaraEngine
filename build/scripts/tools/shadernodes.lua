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
	[[E:\Qt\5.14.1\msvc2017_64\include]],
	[[E:\Qt\5.14.1\msvc2017_64\include\QtCore]],
	[[E:\Qt\5.14.1\msvc2017_64\include\QtGui]],
	[[E:\Qt\5.14.1\msvc2017_64\include\QtWidgets]],
	[[C:\Users\Lynix\Documents\GitHub\nodeeditor\include]],
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
	[[E:\Qt\5.14.1\msvc2017_64\lib]],
	[[C:\Users\Lynix\Documents\GitHub\nodeeditor\build\lib\Debug]]
}
