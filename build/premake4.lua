solution "Nazara"
 
-- Hack: loadfile doesn't change current directory, as does premake-overloaded dofile

loadfile("scripts/common.lua")()

local modules

modules = os.matchfiles("scripts/module/*.lua")
for k,v in pairs(modules) do
	local f, err = loadfile(v)
	if (f) then
		f()
	else
		error("Unable to load module: " .. err)
	end
end

modules = os.matchfiles("scripts/actions/*.lua")
for k,v in pairs(modules) do
	local f, err = loadfile(v)
	if (f) then
		f()
	else
		print("Unable to load action: " .. err)
	end
end