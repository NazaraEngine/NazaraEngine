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
	"../src"
}

TOOL.Files = {
	"../src/ShaderNode/**.hpp",
	"../src/ShaderNode/**.inl",
	"../src/ShaderNode/**.cpp"
}

TOOL.Libraries = {
	"NazaraCore%s(-s)%d(-d)",
	"NazaraShader%s(-s)%d(-d)",
	"NazaraUtility%s(-s)%d(-d)",
	"Qt5Core%d(d)",
	"Qt5Gui%d(d)",
	"Qt5Widgets%d(d)",
	"nodes%d(d)"
}

local function AppendValues(tab, value)
	if (type(value) == "table") then
		for _, v in pairs(value) do
			AppendValues(tab, v)
		end
	else
		table.insert(tab, value)
	end
end

function TOOL:ValidateLib(libName)
	local config = NazaraBuild:GetConfig()
	local includes = config[libName .. "IncludeDir"]
	local binDir32 = config[libName .. "BinDir_x86"]
	local binDir64 = config[libName .. "BinDir_x64"]
	local libDir32 = config[libName .. "LibDir_x86"]
	local libDir64 = config[libName .. "LibDir_x64"]
	if (not includes) then
		return false, "missing " .. libName .. " includes directories in config.lua"
	end
	
	if (not libDir32 and not libDir64) then
		return false, "missing " .. libName .. " library search directories in config.lua"
	end

	AppendValues(self.Includes, includes)

	if (binDir32) then
		AppendValues(self.BinaryPaths.x86, binDir32)
	end

	if (binDir64) then
		AppendValues(self.BinaryPaths.x64, binDir64)
	end

	if (libDir32) then
		AppendValues(self.LibraryPaths.x86, libDir32)
	end

	if (libDir64) then
		AppendValues(self.LibraryPaths.x64, libDir64)
	end

	return true
end

function TOOL:Validate()
	local success, err = self:ValidateLib("Qt5")
	if (not success) then
		return false, err
	end

	local success, err = self:ValidateLib("QtNodes")
	if (not success) then
		return false, err
	end

	return true
end