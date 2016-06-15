TOOL.Name = "SDKServer"

TOOL.Directory = "../SDK"
TOOL.Kind = "Library"
TOOL.TargetDirectory = "../SDK/lib"

TOOL.Defines = {
	"NDK_BUILD",
	"NDK_SERVER"
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

-- Excludes client-only files
TOOL.FilesExcluded = {
	"../SDK/**/CameraComponent.*",
	"../SDK/**/Console.*",
	"../SDK/**/GraphicsComponent.*",
	"../SDK/**/LightComponent.*",
	"../SDK/**/ListenerComponent.*",
	"../SDK/**/ListenerSystem.*",
	"../SDK/**/RenderSystem.*",
	"../SDK/**/LuaBinding_Audio.*",
	"../SDK/**/LuaBinding_Graphics.*",
	"../SDK/**/LuaBinding_Renderer.*"
}


TOOL.Libraries = function()
    local libraries = {}
    for k,v in pairs(NazaraBuild.Modules) do
        if (not v.ClientOnly) then
            table.insert(libraries, "Nazara" .. v.Name)
        end
    end

    return libraries
end
