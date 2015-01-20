local PosixOSes = {"bsd", "linux", "macosx", "solaris"}

NazaraBuild = {} -- L'équivalent d'un namespace en Lua est une table

function NazaraBuild:Execute()
	if (_ACTION == nil) then -- Si aucune action n'est spécifiée
		return -- Alors l'utilisateur voulait probablement savoir comment utiliser le programme, on ne fait rien
	end

	if (self.Actions[_ACTION] ~= nil) then
		self.Actions[_ACTION].Function()
	else
		solution "NazaraEngine"

		-- Configuration générale
		configurations 
		{
		--	"DebugStatic",
		--	"ReleaseStatic",
			"DebugDynamic",
			"ReleaseDynamic"
		}

		defines "NAZARA_BUILD"
		language "C++"
		location(_ACTION)

		includedirs
		{
			"../include",
			"../src/",
			"../extlibs/include"
		}

		libdirs "../lib"

		if (_OPTIONS["x64"]) then
			defines "NAZARA_PLATFORM_x64"
			libdirs "../extlibs/lib/x64"
		else
			libdirs "../extlibs/lib/x86"
		end

		targetdir "../lib"

		configuration "Debug*"
			defines "NAZARA_DEBUG"
			flags "Symbols"

		configuration "Release*"
			flags { "EnableSSE2", "Optimize", "OptimizeSpeed", "NoFramePointer", "NoRTTI" }

		configuration { "Release*", "codeblocks or codelite or gmake or xcode3 or xcode4" }
			buildoptions "-mfpmath=sse" -- Utilisation du SSE pour les calculs flottants
			buildoptions "-ftree-vectorize" -- Activation de la vectorisation du code

		configuration "*Static"
			defines "NAZARA_STATIC"
			kind "StaticLib"

		configuration "*Dynamic"
			kind "SharedLib"

		configuration "DebugStatic"
			targetsuffix "-s-d"

		configuration "ReleaseStatic"
			targetsuffix "-s"

		configuration "DebugDynamic"
			targetsuffix "-d"

		configuration "codeblocks or codelite or gmake or xcode3 or xcode4"
			buildoptions "-std=c++11"
		if (_OPTIONS["x64"]) then
			buildoptions "-m64"
		end

		configuration { "linux or bsd or macosx", "gmake" }
			buildoptions "-fvisibility=hidden"

		configuration { "linux or bsd or macosx", "gmake" }
			buildoptions "-fvisibility=hidden"

		configuration "vs*"
			defines "_CRT_SECURE_NO_WARNINGS"

		-- Spécification des modules
		if (_OPTIONS["united"]) then
			project "NazaraEngine"
		end

		for i=1, #self.Modules do
			local moduleTable = self.Modules[i]
			if (not _OPTIONS["united"]) then
				project("Nazara" .. moduleTable.Name)
			end

			configuration {}

			files(moduleTable.Files)
			excludes(moduleTable.FilesExclusion)

			defines(moduleTable.Defines)
			flags(moduleTable.Flags)
			links(moduleTable.Libraries)

			for k,v in pairs(moduleTable.ConfigurationLibraries) do
				configuration(k)
				links(v)
			end
		end
	end

	if (_OPTIONS["with-examples"]) then
		solution "NazaraExamples"
		-- Configuration générale
		configurations 
		{
		--	"DebugStatic",
		--	"ReleaseStatic",
			"DebugDynamic",
			"ReleaseDynamic"
		}

		language "C++"
		location("../examples/build/" .. _ACTION)

		debugdir "../examples/bin"
		includedirs "../include"
		libdirs "../lib"

		if (_OPTIONS["x64"]) then
			defines "NAZARA_PLATFORM_x64"
			libdirs "../extlibs/lib/x64"
		else
			libdirs "../extlibs/lib/x86"
		end

		targetdir "../examples/bin"

		configuration "Debug*"
			defines "NAZARA_DEBUG"
			flags "Symbols"

		configuration "Release*"
			flags { "EnableSSE2", "Optimize", "OptimizeSpeed", "NoFramePointer", "NoRTTI" }

		configuration { "Release*", "codeblocks or codelite or gmake or xcode3 or xcode4" }
			buildoptions "-mfpmath=sse" -- Utilisation du SSE pour les calculs flottants
			buildoptions "-ftree-vectorize" -- Activation de la vectorisation du code

		configuration "*Static"
			defines "NAZARA_STATIC"

		configuration "codeblocks or codelite or gmake or xcode3 or xcode4"
			buildoptions "-std=c++11"

		for i=1, #self.Examples do
			local exampleTable = self.Examples[i]
			project("Demo" .. exampleTable.Name)

			if (exampleTable.Console) then
				kind "ConsoleApp"
			else
				kind "Window"
			end
			
			files(exampleTable.Files)
			excludes(exampleTable.FilesExclusion)

			defines(exampleTable.Defines)
			flags(exampleTable.Flags)
			links(exampleTable.Libraries)

			for k,v in pairs(exampleTable.ConfigurationLibraries) do
				configuration(k)
				links(v)
			end
		end
	end

	if (_OPTIONS["with-extlibs"]) then
		solution "NazaraExtlibs"
		-- Configuration générale
		configurations 
		{
			"DebugStatic",
			"ReleaseStatic"
		}

		location("../extlibs/build/" .. _ACTION)
		includedirs "../extlibs/include"
		kind "StaticLib"

		if (_OPTIONS["x64"]) then
			libdirs "../extlibs/lib/x64"
			targetdir "../extlibs/lib/x64"
		else
			libdirs "../extlibs/lib/x86"
			targetdir "../extlibs/lib/x86"
		end

		configuration "Debug*"
			flags "Symbols"

		configuration "Release*"
			flags { "EnableSSE2", "Optimize", "OptimizeSpeed", "NoFramePointer", "NoRTTI" }

		configuration { "Release*", "codeblocks or codelite or gmake or xcode3 or xcode4" }
			buildoptions "-mfpmath=sse" -- Utilisation du SSE pour les calculs flottants
			buildoptions "-ftree-vectorize" -- Activation de la vectorisation du code

		configuration "DebugStatic"
			targetsuffix "-s-d"

		configuration "ReleaseStatic"
			targetsuffix "-s"

		configuration "codeblocks or codelite or gmake or xcode3 or xcode4"
			buildoptions "-std=c++11"

		for i=1, #self.ExtLibs do
			local libTable = self.ExtLibs[i]
			project(libTable.Name)
			
			language(libTable.Language)
			
			files(libTable.Files)
			excludes(libTable.FilesExclusion)

			defines(libTable.Defines)
			flags(libTable.Flags)
			links(libTable.Libraries)

			for k,v in pairs(libTable.ConfigurationLibraries) do
				configuration(k)
				links(v)
			end
		end
	end
end

function NazaraBuild:Initialize()
	-- Commençons par les options
	newoption {
		trigger     = "x64",
		description = "Setup build project for x64 arch"
	}

	newoption {
		trigger     = "united",
		description = "Builds all the modules as one united library"
	}

	newoption {
		trigger     = "with-extlibs",
		description = "Builds the extern libraries"
	}

	newoption {
		trigger     = "with-examples",
		description = "Builds the examples"
	}

	-- Puis par les bibliothèques externes
	self.ExtLibs = {}
	local extlibs = os.matchfiles("../extlibs/build/*.lua")
	for k,v in pairs(extlibs) do
		local f, err = loadfile(v)
		if (f) then
			LIBRARY = {}
			LIBRARY.ConfigurationLibraries = {}
			LIBRARY.ConfigurationLibraries.DebugStatic = {}
			LIBRARY.ConfigurationLibraries.ReleaseStatic = {}
			LIBRARY.ConfigurationLibraries.DebugDynamic = {}
			LIBRARY.ConfigurationLibraries.ReleaseDynamic = {}
			LIBRARY.Defines = {}
			LIBRARY.Files = {}
			LIBRARY.FilesExclusion = {}
			LIBRARY.Flags = {}
			LIBRARY.Libraries = {}
			LIBRARY.OsFiles = {}
			LIBRARY.OsLibraries = {}

			f()

			local succeed, err = self:RegisterExternLibrary(LIBRARY)
			if (not succeed) then
				print("Unable to register extern library: " .. err)
			end
		else
			print("Unable to load extern library file: " .. err)
		end
	end
	LIBRARY = nil

	-- Ensuite nous allons parcourir tous les scripts de modules
	self.Modules = {}
	local modules = os.matchfiles("scripts/module/*.lua")
	for k,v in pairs(modules) do
		local moduleName = v:match(".*/(.*).lua"):lower()

		if (moduleName ~= "core") then -- exclure le noyau n'aurait aucun sens
			newoption {
				trigger     = "exclude-" .. moduleName,
				description = "Exclude the " .. moduleName .. " module from the build system"
			}
		end

		if (not _OPTIONS["exclude-" .. moduleName]) then
			local f, err = loadfile(v)
			if (f) then
				MODULE = {}
				MODULE.ConfigurationLibraries = {}
				MODULE.ConfigurationLibraries.DebugStatic = {}
				MODULE.ConfigurationLibraries.ReleaseStatic = {}
				MODULE.ConfigurationLibraries.DebugDynamic = {}
				MODULE.ConfigurationLibraries.ReleaseDynamic = {}
				MODULE.Defines = {}
				MODULE.ExtLibs = {}
				MODULE.Files = {}
				MODULE.FilesExclusion = {}
				MODULE.Flags = {}
				MODULE.Libraries = {}
				MODULE.OsFiles = {}
				MODULE.OsLibraries = {}

				f()

				local succeed, err = self:RegisterModule(MODULE)
				if (not succeed) then
					print("Unable to register module: " .. err)
				end
			else
				print("Unable to load module file: " .. err)
			end
		end
	end
	MODULE = nil
	table.sort(self.Modules, function (a, b) return a.Name < b.Name end)

	-- Et ensuite les scripts d'actions possibles
	self.Actions = {}
	modules = os.matchfiles("scripts/actions/*.lua")
	for k,v in pairs(modules) do
		local f, err = loadfile(v)
		if (f) then
			ACTION = {}

			f()

			local succeed, err = self:RegisterAction(ACTION)
			if (not succeed) then
				print("Unable to register action: " .. err)
			end
		else
			print("Unable to load action file: " .. err)
		end
	end
	ACTION = nil
	
	self.Examples = {}
	local examples = os.matchdirs("../examples/*")
	for k,v in pairs(examples) do
		local dirName = v:match(".*/(.*)")
		if (dirName ~= "bin" and dirName ~= "build") then
			local f, err = loadfile(v .. "/build.lua")
			if (f) then
				EXAMPLE = {}
				EXAMPLE.ConfigurationLibraries = {}
				EXAMPLE.ConfigurationLibraries.DebugStatic = {}
				EXAMPLE.ConfigurationLibraries.ReleaseStatic = {}
				EXAMPLE.ConfigurationLibraries.DebugDynamic = {}
				EXAMPLE.ConfigurationLibraries.ReleaseDynamic = {}
				EXAMPLE.Defines = {}
				EXAMPLE.Directory = dirName
				EXAMPLE.Files = {}
				EXAMPLE.FilesExclusion = {}
				EXAMPLE.Flags = {}
				EXAMPLE.Libraries = {}
				EXAMPLE.OsFiles = {}
				EXAMPLE.OsLibraries = {}

				f()

				local succeed, err = self:RegisterExample(EXAMPLE)
				if (not succeed) then
					print("Unable to register example: " .. err)
				end
			else
				print("Unable to load example file: " .. err)
			end
		end
	end
	EXAMPLE = nil
end

function NazaraBuild:RegisterAction(actionTable)
	if (actionTable.Name == nil or type(actionTable.Name) ~= "string") then
		return false, "Action name is invalid"
	end

	if (string.len(actionTable.Name) == 0) then
		return false, "Action name is empty"
	end

	if (actionTable.Description == nil or type(actionTable.Description) ~= "string") then
		return false, "Action description is invalid"
	end

	if (string.len(actionTable.Description) == 0) then
		return false, "Action description is empty"
	end

	if (self.Actions[actionTable.name] ~= nil) then
		return false, "Action name \"" .. actionTable.name .. " is already registred"
	end

	if (actionTable.Function == nil or type(actionTable.Function) ~= "function") then
		return false, "Action function is invalid"
	end

	local actionTrigger = string.lower(actionTable.Name)

	self.Actions[actionTrigger] = actionTable

	newaction
	{
		trigger     = actionTrigger,
		description = actionTable.Description,
		execute     = actionTable.Function
	}

	return true
end

function NazaraBuild:RegisterExample(exampleTable)
	if (exampleTable.Name == nil or type(exampleTable.Name) ~= "string") then
		return false, "Example name is invalid"
	end

	if (string.len(exampleTable.Name) == 0) then
		return false, "Example name is empty"
	end

	if (exampleTable.Files == nil or type(exampleTable.Files) ~= "table") then
		return false, "Example files table is invalid"
	end

	if (#exampleTable.Files == 0) then
		return false, "Example files table is empty"
	end

	local files = {}
	for k, file in ipairs(exampleTable.Files) do
		table.insert(files, "../examples/" .. exampleTable.Directory .. "/" .. file)
	end
	exampleTable.Files = files
	
	local libraries = {}
	for k, library in ipairs(exampleTable.Libraries) do
		if (string.sub(library, 1, 6) == "Nazara") then
			if (_OPTIONS["united"]) then
				table.insert(exampleTable.ConfigurationLibraries.DebugStatic, "NazaraEngine-s-d")
				table.insert(exampleTable.ConfigurationLibraries.ReleaseStatic, "NazaraEngine-s")
				table.insert(exampleTable.ConfigurationLibraries.DebugDynamic, "NazaraEngine-d")
				table.insert(exampleTable.ConfigurationLibraries.ReleaseDynamic, "NazaraEngine")
			else
				table.insert(exampleTable.ConfigurationLibraries.DebugStatic, library .. "-s-d")
				table.insert(exampleTable.ConfigurationLibraries.ReleaseStatic, library .. "-s")
				table.insert(exampleTable.ConfigurationLibraries.DebugDynamic, library .. "-d")
				table.insert(exampleTable.ConfigurationLibraries.ReleaseDynamic, library)
			end
		else
			table.insert(libraries, library)
		end
	end
	exampleTable.Libraries = libraries

	for platform, fileTable in ipairs(exampleTable.OsFiles) do
		platform = string.lower(platform)
		if (platform == "posix") then
			for k,v in ipairs(PosixOSes) do
				if (os.is(v)) then
					platform = v
					break
				end
			end
		end

		if (os.is(platform)) then
			for k,v in ipairs(fileTable) do
				table.insert(exampleTable.Files, v)
			end
		else
			for k,v in ipairs(fileTable) do
				table.insert(exampleTable.FilesExclusion, v)
			end
		end
	end
	exampleTable.OsFiles = nil

	for platform, libraryTable in ipairs(exampleTable.OsLibraries) do
		platform = string.lower(platform)
		if (platform == "posix") then
			for k,v in ipairs(PosixOSes) do
				if (os.is(v)) then
					platform = v
					break
				end
			end
		end

		if (os.is(platform)) then
			for k,v in ipairs(libraryTable) do
				table.insert(exampleTable.Libraries, v)
			end
		end
	end
	exampleTable.OsLibraries = nil

	table.insert(self.Examples, exampleTable)
	return true
end

function NazaraBuild:RegisterExternLibrary(libTable)
	if (libTable.Name == nil or type(libTable.Name) ~= "string") then
		return false, "Module name is invalid"
	end

	if (string.len(libTable.Name) == 0) then
		return false, "Module name is empty"
	end

	if (libTable.Files == nil or type(libTable.Files) ~= "table") then
		return false, "Module files table is invalid"
	end

	if (#libTable.Files == 0) then
		return false, "Module files table is empty"
	end

	libTable.Libraries = libraries

	for platform, fileTable in pairs(libTable.OsFiles) do
		platform = string.lower(platform)
		if (platform == "posix") then
			for k,v in pairs(PosixOSes) do
				if (os.is(v)) then
					platform = v
					break
				end
			end
		end

		if (os.is(platform)) then
			for k,v in pairs(fileTable) do
				table.insert(libTable.Files, v)
			end
		else
			for k,v in pairs(fileTable) do
				table.insert(libTable.FilesExclusion, v)
			end
		end
	end
	libTable.OsFiles = nil

	for platform, libraryTable in pairs(libTable.OsLibraries) do
		platform = string.lower(platform)
		if (platform == "posix") then
			for k,v in pairs(PosixOSes) do
				if (os.is(v)) then
					platform = v
					break
				end
			end
		end

		if (os.is(platform)) then
			for k,v in pairs(libraryTable) do
				table.insert(libTable.Libraries, v)
			end
		end
	end
	libTable.OsLibraries = nil

	table.insert(self.ExtLibs, libTable)
	return true
end

function NazaraBuild:RegisterModule(moduleTable)
	if (moduleTable.Name == nil or type(moduleTable.Name) ~= "string") then
		return false, "Module name is invalid"
	end

	if (string.len(moduleTable.Name) == 0) then
		return false, "Module name is empty"
	end

	table.insert(moduleTable.Files, "../include/Nazara/" .. moduleTable.Name .. "/**.hpp")
	table.insert(moduleTable.Files, "../include/Nazara/" .. moduleTable.Name .. "/**.inl")
	table.insert(moduleTable.Files, "../src/Nazara/" .. moduleTable.Name .. "/**.hpp")
	table.insert(moduleTable.Files, "../src/Nazara/" .. moduleTable.Name .. "/**.cpp")
	table.insert(moduleTable.FilesExclusion, "../src/Nazara/" .. moduleTable.Name .. "/Debug/NewOverload.cpp")

	local libraries = {}
	for k, library in pairs(moduleTable.Libraries) do
		if (string.sub(library, 1, 6) == "Nazara") then
			if (not _OPTIONS["united"]) then
				table.insert(moduleTable.ConfigurationLibraries.DebugStatic, library .. "-s-d")
				table.insert(moduleTable.ConfigurationLibraries.ReleaseStatic, library .. "-s")
				table.insert(moduleTable.ConfigurationLibraries.DebugDynamic, library .. "-d")
				table.insert(moduleTable.ConfigurationLibraries.ReleaseDynamic, library)
			end
		else
			local found = false
			for k,extlibTable in pairs(self.ExtLibs) do
				if (library == extlibTable.Name) then
					table.insert(moduleTable.ConfigurationLibraries.DebugStatic, library .. "-s-d")
					table.insert(moduleTable.ConfigurationLibraries.ReleaseStatic, library .. "-s")
					table.insert(moduleTable.ConfigurationLibraries.DebugDynamic, library .. "-s-d")
					table.insert(moduleTable.ConfigurationLibraries.ReleaseDynamic, library .. "-s")
					found = true
					break
				end
			end

			if (not found) then
				table.insert(libraries, library)
			end
		end
	end
	moduleTable.Libraries = libraries

	for platform, fileTable in pairs(moduleTable.OsFiles) do
		platform = string.lower(platform)
		if (platform == "posix") then
			for k,v in pairs(PosixOSes) do
				if (os.is(v)) then
					platform = v
					break
				end
			end
		end

		if (os.is(platform)) then
			for k,v in pairs(fileTable) do
				table.insert(moduleTable.Files, v)
			end
		else
			for k,v in pairs(fileTable) do
				table.insert(moduleTable.FilesExclusion, v)
			end
		end
	end
	moduleTable.OsFiles = nil

	for platform, libraryTable in pairs(moduleTable.OsLibraries) do
		platform = string.lower(platform)
		if (platform == "posix") then
			for k,v in pairs(PosixOSes) do
				if (os.is(v)) then
					platform = v
					break
				end
			end
		end

		if (os.is(platform)) then
			for k,library in pairs(libraryTable) do
				table.insert(moduleTable.Libraries, library)
			end
		end
	end
	moduleTable.OsLibraries = nil

	table.insert(self.Modules, moduleTable)
	return true
end
