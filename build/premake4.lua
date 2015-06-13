-- Hack: loadfile doesn't change current directory, as does premake-overloaded dofile
loadfile("scripts/common.lua")()

NazaraBuild:Initialize()
NazaraBuild:Execute()