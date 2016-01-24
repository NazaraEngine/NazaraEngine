-- Premake5 features
function floatingpoint(value)
    if (value == "Fast") then
        flags("FloatFast")
    elseif (value == "Strict") then
        flags("FloatStrict")
    end
end

function optimize(value)
    if (value == "Size") then
        flags({"Optimize", "OptimizeSize"})
    elseif (value == "Speed") then
        flags({"Optimize", "OptimizeSpeed"})
    elseif (value == "Full") then
        flags({"Optimize", "OptimizeSize", "OptimizeSpeed"})
    end
end

function nativewchar(value)
    if (value == "On") then
        flags("NativeWChar")
    elseif (value == "Off") then
        flags("NoNativeWChar")
    end
end

function rtti(value)
    if (value == "Off") then
        flags("NoRTTI")
    end
end

function vectorextensions(level)
    if (level == "SSE") then
        flags("EnableSSE")
    elseif (level == "SSE2") then
        flags("EnableSSE")
    end
end

function warnings(value)
    if (value == "Off") then
        flags("NoWarnings")
    elseif (value == "Extra") then
        flags("ExtraWarnings")
    end
end

workspace = solution

-- Hack: loadfile doesn't change current directory, as does premake-overloaded dofile
loadfile("scripts/common.lua")()

NazaraBuild:Initialize()
NazaraBuild:Execute()