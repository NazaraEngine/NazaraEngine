TOOL.Name = "Editor"

TOOL.Directory = "../editor"
TOOL.EnableConsole = true
TOOL.Kind = "Application"

TOOL.Defines = {
}

TOOL.Includes = {
	"../extlibs/include/QtCore",
	"../extlibs/include/QtGui",
	"../extlibs/include/QtWidgets",
	"../editor/src",
	"../include"
}

TOOL.Files = {
	"../editor/src/**.hpp",
	"../editor/src/**.inl",
	"../editor/src/**.cpp"
}

TOOL.Libraries = {
	"Qt5Core",
	"Qt5Gui",
	"Qt5Widgets",
	"NazaraSDK"
}