solution "NazaraExamples"
 
-- Hack: loadfile doesn't change current directory, as does premake-overloaded dofile

loadfile("scripts/common.lua")()

local examples = os.matchdirs("../*")
for k,v in pairs(examples) do
	local dirName = v:match(".*/(.*)")
	print(dirName)
	if (dirName ~= "build" and dirName ~= "bin") then
		local f, err = loadfile(v)
		if (f) then
			f()
		else
			error("Unable to load example: " .. err)
		end
	end
end