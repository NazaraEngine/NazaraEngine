newoption {
	trigger     = "x64",
	description = "Setup build project for x64 arch"
}

newoption {
	trigger     = "one-library",
	description = "Builds all the modules as one library"
}

newoption {
	trigger     = "with-examples",
	description = "Builds the examples"
}

solution "NazaraEngine"
-- Hack: loadfile doesn't change current directory, as does premake-overloaded dofile
loadfile("scripts/common.lua")()

local modules

if (_OPTIONS["one-library"]) then
	project "NazaraEngine"
end

modules = os.matchfiles("scripts/module/*.lua")
for k,v in pairs(modules) do
	local moduleName = v:match(".*/(.*).lua")

	if (moduleName ~= "core") then
		newoption {
			trigger     = "exclude-" .. moduleName,
			description = "Exclude the " .. moduleName .. " module from build system"
		}
	end

	if (not _OPTIONS["exclude-" .. moduleName]) then
		local f, err = loadfile(v)
		if (f) then
			f()
		else
			error("Unable to load module: " .. err)
		end
	end
end

if (_OPTIONS["one-library"]) then
	configuration "Debug*"
		targetname "Nazarad"

	configuration "Release*"
		targetname "Nazara"
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

if (_OPTIONS["with-examples"]) then
	solution "NazaraExamples"
	loadfile("scripts/common_examples.lua")()

	local examples = os.matchdirs("../examples/*")
	for k,v in pairs(examples) do
		local dirName = v:match(".*/(.*)")
		if (dirName ~= "bin" and
		    dirName ~= "build") then
			project(dirName)
			dofile(v .. "/build.lua")
		end
	end
end
