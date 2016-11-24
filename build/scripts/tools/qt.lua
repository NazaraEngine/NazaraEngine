TOOL.Name = "Qt"

TOOL.Directory = "../Qt"
TOOL.Kind = "Library"
TOOL.TargetDirectory = "../Qt/lib"

TOOL.Defines = {
	"NDK_QT_BUILD"
}

TOOL.Includes = {
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
	"NazaraRenderer",
	"NazaraUtility"
}

function TOOL:Validate()
	local config = NazaraBuild:GetConfig()
	local include = config["Qt5IncludeDir"]
	local x86BinDir = config["Qt5BinDir_x86"]
	local x64BinDir = config["Qt5BinDir_x64"]
	local x86LibDir = config["Qt5LibDir_x86"]
	local x64LibDir = config["Qt5LibDir_x64"]
	if (not include) then
		return false, "Missing Qt includes directories in config.lua"
	end
	
	if (not x86LibDir and not x64LibDir) then
		return false, "Missing Qt library search directories in config.lua"
	end

	table.insert(self.Includes, include)
	
	if (x86BinDir) then
		table.insert(self.BinaryPaths.x86, x86BinDir)
	end

	if (x64BinDir) then
		table.insert(self.BinaryPaths.x64, x64BinDir)
	end

	if (x86LibDir) then
		table.insert(self.LibraryPaths.x86, x86LibDir)
	end

	if (x64LibDir) then
		table.insert(self.LibraryPaths.x64, x64LibDir)
	end

	return true
end
