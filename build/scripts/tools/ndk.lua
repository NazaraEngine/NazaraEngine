TOOL.Name = "SDK"

TOOL.Directory = "../SDK"
TOOL.Kind = "Library"
TOOL.TargetDirectory = "../lib"

TOOL.Defines = {
	"NDK_BUILD"
}

TOOL.Includes = {
	"../SDK/include",
	"../SDK/src"
}

TOOL.Files = {
	"../SDK/include/NDK/**.hpp",
	"../SDK/include/NDK/**.inl",
	"../SDK/src/NDK/**.hpp",
	"../SDK/src/NDK/**.inl",
	"../SDK/src/NDK/**.cpp"
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