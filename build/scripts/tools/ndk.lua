TOOL.Name = "SDK"

TOOL.Directory = "../SDK"
TOOL.Kind = "Library"
TOOL.TargetDirectory = "../lib"

TOOL.Defines = {
	"NDK_BUILD"
}

TOOL.Includes = {
	"../include",
	"../src"
}

TOOL.Files = {
	"../include/NazaraSDK/**.hpp",
	"../include/NazaraSDK/**.inl",
	"../src/NazaraSDK/**.hpp",
	"../src/NazaraSDK/**.inl",
	"../src/NazaraSDK/**.cpp"
}

TOOL.Libraries = function()
    local libraries = {}
    for k,v in pairs(NazaraBuild.Modules) do
        table.insert(libraries, "Nazara" .. v.Name)
    end

	-- Keep libraries in the same order to prevent useless premake regeneration
	table.sort(libraries)

    return libraries
end