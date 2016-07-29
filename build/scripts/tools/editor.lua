TOOL.Name = "Editor"

TOOL.Directory = "../editor"
TOOL.EnableConsole = true
TOOL.Kind = "Application"
TOOL.TargetDirectory = "../editor/bin"

TOOL.Defines = {
}

TOOL.Includes = {
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
	"NazaraQt",
	"NazaraSDK"
}

function TOOL:Validate()
	local qtDepTable = NazaraBuild:GetDependency(self, "NazaraQt")
	return qtDepTable.Validate(self)
end
