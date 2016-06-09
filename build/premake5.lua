PremakeVersion = 50

-- Hack: loadfile doesn't change current directory, as does premake-overloaded dofile
local func, err = loadfile("scripts/common.lua")
if (not func) then
	error("Failed to compile common.lua: " .. err)
end

func()

NazaraBuild:Initialize()
NazaraBuild:Execute()