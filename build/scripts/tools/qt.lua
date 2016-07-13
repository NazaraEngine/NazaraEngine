TOOL.Name = "Qt"

TOOL.Directory = "../Qt"
TOOL.Kind = "Library"
TOOL.TargetDirectory = "../Qt/lib"

TOOL.Defines = {
	"NDK_QT_BUILD"
}

TOOL.Includes = {
	"../extlibs/include",
	"../extlibs/include/QtCore",
	"../extlibs/include/QtGui",
	"../extlibs/include/QtWidgets",
    "../include",
	"../Qt/include",
	"../Qt/src"
}

TOOL.Files = {
	"../Qt/include/**.hpp",
	"../Qt/include/**.inl",
	"../Qt/src/**.hpp",
	"../Qt/src/**.inl",
	"../Qt/src/**.cpp"
}

TOOL.Libraries = {
	"Qt5Core",
	"Qt5Gui",
	"Qt5Widgets",
	"NazaraRenderer"
}