NazaraBuild = {} -- L'équivalent d'un namespace en Lua est une table

function NazaraBuild:Execute()
	if (_ACTION == nil) then -- Si aucune action n'est spécifiée
		return -- Alors l'utilisateur voulait probablement savoir comment utiliser le programme, on ne fait rien
	end

	local platformData
	if (os.is64bit()) then
		platformData = {"x64", "x32"}
	else
		platformData = {"x32", "x64"}
	end
	
	if (self.Actions[_ACTION] == nil) then
		local makeLibDir = os.is("windows") and "mingw" or "gmake"
	
        if (_OPTIONS["with-extlibs"]) then
			workspace("NazaraExtlibs")
			platforms(platformData)

			-- Configuration générale
			configurations({
				"DebugStatic",
				"ReleaseStatic"
			})

			includedirs("../extlibs/include")
			libdirs("../extlibs/lib/common")
			location(_ACTION)
			kind("StaticLib")

			configuration("x32")
				libdirs("../extlibs/lib/common/x86")

			configuration("x64")
				libdirs("../extlibs/lib/common/x64")

			configuration({"codeblocks or codelite or gmake", "x32"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x86")
				targetdir("../extlibs/lib/" .. makeLibDir .. "/x86")

			configuration({"codeblocks or codelite or gmake", "x64"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x64")
				targetdir("../extlibs/lib/" .. makeLibDir .. "/x64")

			configuration("vs*")
				buildoptions("/MP")

			configuration({"vs*", "x32"})
				libdirs("../extlibs/lib/msvc/x86")
				targetdir("../extlibs/lib/msvc/x86")

			configuration({"vs*", "x64"})
				libdirs("../extlibs/lib/msvc/x64")
				targetdir("../extlibs/lib/msvc/x64")

			configuration({"xcode3 or xcode4", "x32"})
				libdirs("../extlibs/lib/xcode/x86")
				targetdir("../extlibs/lib/xcode/x86")

			configuration({"xcode3 or xcode4", "x64"})
				libdirs("../extlibs/lib/xcode/x64")
				targetdir("../extlibs/lib/xcode/x64")

			configuration("Debug*")
				flags("Symbols")

			configuration("Release*")
				flags("NoFramePointer")
                optimize("Speed")
                rtti("Off")
                vectorextensions("SSE2")

			configuration({"Release*", "codeblocks or codelite or gmake or xcode3 or xcode4"})
				buildoptions("-mfpmath=sse") -- Utilisation du SSE pour les calculs flottants
				buildoptions("-ftree-vectorize") -- Activation de la vectorisation du code

			configuration("DebugStatic")
				targetsuffix("-s-d")

			configuration("ReleaseStatic")
				targetsuffix("-s")

			configuration("codeblocks or codelite or gmake or xcode3 or xcode4")
				buildoptions({"-fPIC", "-std=c++14", "-U__STRICT_ANSI__"})

			for k, libTable in ipairs(self.OrderedExtLibs) do
				project(libTable.Name)
				
				language(libTable.Language)
				location(_ACTION .. "/extlibs")

				files(libTable.Files)
				excludes(libTable.FilesExcluded)

				defines(libTable.Defines)
				flags(libTable.Flags)
				includedirs(libTable.Includes)
				links(libTable.Libraries)

				for k,v in pairs(libTable.ConfigurationLibraries) do
					configuration(k)
					links(v)
				end
				
				configuration({})
			end
		end

		workspace("NazaraEngine")
		platforms(platformData)

		-- Configuration générale
		configurations({
		--	"DebugStatic",
		--	"ReleaseStatic",
			"DebugDynamic",
			"ReleaseDynamic"
		})

		language("C++")
		location(_ACTION)

		configuration("Debug*")
			defines("NAZARA_DEBUG")
			flags("Symbols")

		configuration("Release*")
            flags("NoFramePointer")
            optimize("Speed")
            rtti("Off")
            vectorextensions("SSE2")

		configuration({"Release*", "codeblocks or codelite or gmake or xcode3 or xcode4"})
			buildoptions("-mfpmath=sse") -- Utilisation du SSE pour les calculs flottants
			buildoptions("-ftree-vectorize") -- Activation de la vectorisation du code

		configuration("*Static")
			defines("NAZARA_STATIC")

		configuration("codeblocks or codelite or gmake or xcode3 or xcode4")
			buildoptions("-std=c++14")

		configuration({"linux or bsd or macosx", "gmake"})
			buildoptions("-fvisibility=hidden")

		configuration({"linux or bsd or macosx", "gmake"})
			buildoptions("-fvisibility=hidden")

		configuration("vs*")
			buildoptions("/MP") -- Multiprocessus build
			flags("NoMinimalRebuild")
			defines("_CRT_SECURE_NO_WARNINGS")
			defines("_SCL_SECURE_NO_WARNINGS")


		-- Spécification des modules
		if (_OPTIONS["united"]) then
			project("NazaraEngine")
		end

		for k, moduleTable in ipairs(self.OrderedModules) do
			if (not _OPTIONS["united"]) then
				project("Nazara" .. moduleTable.Name)
			end

			location(_ACTION .. "/modules")

			defines("NAZARA_BUILD")

			includedirs({
				"../include",
				"../src/",
				"../extlibs/include"
			})

			libdirs("../lib")
			libdirs("../extlibs/lib/common")

			configuration("x32")
				libdirs("../extlibs/lib/common/x86")

			configuration("x64")
				defines("NAZARA_PLATFORM_x64")
				libdirs("../extlibs/lib/common/x64")

			configuration({"codeblocks or codelite or gmake", "x32"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x86")
				libdirs("../lib/" .. makeLibDir .. "/x86")
				targetdir("../lib/" .. makeLibDir .. "/x86")

			configuration({"codeblocks or codelite or gmake", "x64"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x64")
				libdirs("../lib/" .. makeLibDir .. "/x64")
				targetdir("../lib/" .. makeLibDir .. "/x64")

			-- Copy the module binaries to the example folder
			self:MakeCopyAfterBuild(moduleTable)

			configuration({"vs*", "x32"})
				libdirs("../extlibs/lib/msvc/x86")
				libdirs("../lib/msvc/x86")
				targetdir("../lib/msvc/x86")

			configuration({"vs*", "x64"})
				libdirs("../extlibs/lib/msvc/x64")
				libdirs("../lib/msvc/x64")
				targetdir("../lib/msvc/x64")

			configuration({"xcode3 or xcode4", "x32"})
				libdirs("../extlibs/lib/xcode/x86")
				libdirs("../lib/xcode/x86")
				targetdir("../lib/xcode/x86")

			configuration({"xcode3 or xcode4", "x64"})
				libdirs("../extlibs/lib/xcode/x64")
				libdirs("../lib/xcode/x64")
				targetdir("../lib/xcode/x64")

			configuration("*Static")
				kind("StaticLib")

			configuration("*Dynamic")
				kind("SharedLib")

			configuration("DebugStatic")
				targetsuffix("-s-d")

			configuration("ReleaseStatic")
				targetsuffix("-s")

			configuration("DebugDynamic")
				targetsuffix("-d")

			configuration({})

			files(moduleTable.Files)
			excludes(moduleTable.FilesExcluded)

			defines(moduleTable.Defines)
			flags(moduleTable.Flags)
			includedirs(moduleTable.Includes)
			links(moduleTable.Libraries)

			for k,v in pairs(moduleTable.ConfigurationLibraries) do
				configuration(k)
				links(v)
			end
			
			configuration({})
		end
		
		-- Tools
		for k, toolTable in ipairs(self.OrderedTools) do
			local prefix = "Nazara"
			if (toolTable.Kind == "plugin") then
				prefix = "Plugin"
			end
			
			project(prefix .. toolTable.Name)

			location(_ACTION .. "/tools")
			targetdir(toolTable.Directory)

			if (toolTable.Kind == "plugin" or toolTable.Kind == "library") then
				kind("SharedLib")
			elseif (toolTable.Kind == "consoleapp") then
				debugdir(toolTable.Directory)
				kind("ConsoleApp")
			elseif (toolTable.Kind == "windowapp") then
				debugdir(toolTable.Directory)
				kind("WindowedApp")
			else
				assert(false, "Invalid tool Kind")
			end

			includedirs({
				"../include",
				"../extlibs/include"
			})

			libdirs("../lib")
			libdirs("../extlibs/lib/common")

			configuration("x32")
				libdirs("../extlibs/lib/common/x86")

			configuration("x64")
				defines("NAZARA_PLATFORM_x64")
				libdirs("../extlibs/lib/common/x64")

			configuration({"codeblocks or codelite or gmake", "x32"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x86")
				libdirs("../lib/" .. makeLibDir .. "/x86")
				if (toolTable.Kind == "library") then
					targetdir("../lib/" .. makeLibDir .. "/x86")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/" .. makeLibDir .. "/x86")
				end

			configuration({"codeblocks or codelite or gmake", "x64"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x64")
				libdirs("../lib/" .. makeLibDir .. "/x64")
				if (toolTable.Kind == "library") then
					targetdir("../lib/" .. makeLibDir .. "/x64")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/" .. makeLibDir .. "/x64")
				end

			-- Copy the tool binaries to the example folder
			if (toolTable.CopyTargetToExampleDir) then
				self:MakeCopyAfterBuild(toolTable)
			end

			configuration({"vs*", "x32"})
				libdirs("../extlibs/lib/msvc/x86")
				libdirs("../lib/msvc/x86")
				if (toolTable.Kind == "library") then
					targetdir("../lib/msvc/x86")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/msvc/x86")
				end

			configuration({"vs*", "x64"})
				libdirs("../extlibs/lib/msvc/x64")
				libdirs("../lib/msvc/x64")
				if (toolTable.Kind == "library") then
					targetdir("../lib/msvc/x64")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/msvc/x64")
				end

			configuration({"xcode3 or xcode4", "x32"})
				libdirs("../extlibs/lib/xcode/x86")
				libdirs("../lib/xcode/x86")
				if (toolTable.Kind == "library") then
					targetdir("../lib/xcode/x86")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/xcode/x86")
				end

			configuration({"xcode3 or xcode4", "x64"})
				libdirs("../extlibs/lib/xcode/x64")
				libdirs("../lib/xcode/x64")
				if (toolTable.Kind == "library") then
					targetdir("../lib/xcode/x64")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/xcode/x64")
				end

			if (toolTable.Kind == "library" or toolTable.Kind == "plugin") then
				configuration("*Static")
					kind("StaticLib")

				configuration("*Dynamic")
					kind("SharedLib")
				
				configuration("DebugStatic")
					targetsuffix("-s-d")

				configuration("ReleaseStatic")
					targetsuffix("-s")

				configuration("DebugDynamic")
					targetsuffix("-d")
			end

			configuration({})

			files(toolTable.Files)
			excludes(toolTable.FilesExcluded)

			defines(toolTable.Defines)
			flags(toolTable.Flags)
			includedirs(toolTable.Includes)
			links(toolTable.Libraries)

			for k,v in pairs(toolTable.ConfigurationLibraries) do
				configuration(k)
				links(v)
			end

			configuration({})
		end

		for k, exampleTable in ipairs(self.OrderedExamples) do
			project("Demo" .. exampleTable.Name)

			location(_ACTION .. "/examples")

			if (exampleTable.Console) then
				kind("ConsoleApp")
			else
				kind("Window")
			end

			debugdir("../examples/bin")
			includedirs({
				"../include",
				"../extlibs/include"
			})
			libdirs("../lib")
			targetdir("../examples/bin")

			files(exampleTable.Files)
			excludes(exampleTable.FilesExcluded)

			defines(exampleTable.Defines)
			flags(exampleTable.Flags)
			includedirs(exampleTable.Includes)
			links(exampleTable.Libraries)

			configuration("x32")
				libdirs("../extlibs/lib/common/x86")

			configuration("x64")
				defines("NAZARA_PLATFORM_x64")
				libdirs("../extlibs/lib/common/x64")

			configuration({"codeblocks or codelite or gmake", "x32"})
				libdirs("../lib/" .. makeLibDir .. "/x86")

			configuration({"codeblocks or codelite or gmake", "x64"})
				libdirs("../lib/" .. makeLibDir .. "/x64")

			configuration({"vs*", "x32"})
				libdirs("../lib/msvc/x86")

			configuration({"vs*", "x64"})
				libdirs("../lib/msvc/x64")

			configuration({"xcode3 or xcode4", "x32"})
				libdirs("../lib/xcode/x86")

			configuration({"xcode3 or xcode4", "x64"})
				libdirs("../lib/xcode/x64")

			for k,v in pairs(exampleTable.ConfigurationLibraries) do
				configuration(k)
				links(v)
			end
			
			configuration({})
		end
	end
end

function NazaraBuild:Initialize()
	-- Commençons par les options
	newoption({
		trigger     = "server",
		description = "Excludes client-only modules/tools/examples"
	})

	newoption({
		trigger     = "united",
		description = "Builds all the modules as one united library"
	})

	newoption({
		trigger     = "with-extlibs",
		description = "Builds the extern libraries"
	})

	newoption({
		trigger     = "with-examples",
		description = "Builds the examples"
	})

	self.Actions = {}
	self.Examples = {}
	self.ExtLibs = {}
	self.Modules = {}
	self.Tools = {}

	-- Actions
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

	-- Extern libraries
    local extlibs = os.matchfiles("../extlibs/build/*.lua")
    for k,v in pairs(extlibs) do
        local f, err = loadfile(v)
        if (f) then
            LIBRARY = {}
            self:SetupInfoTable(LIBRARY)

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

	-- Then the modules
	local modules = os.matchfiles("scripts/modules/*.lua")
	for k,v in pairs(modules) do
		local moduleName = v:match(".*/(.*).lua")
		local moduleNameLower = moduleName:lower()

		if (moduleNameLower ~= "core") then -- exclure le noyau n'aurait aucun sens
			newoption({
				trigger     = "exclude-" .. moduleNameLower,
				description = "Exclude the " .. moduleName .. " module from the build system"
			})
		end

		if (not _OPTIONS["exclude-" .. moduleNameLower]) then
			local f, err = loadfile(v)
			if (f) then
				MODULE = {}
				self:SetupInfoTable(MODULE)

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

	-- Continue with the tools (ex: SDK)
	local tools = os.matchfiles("scripts/tools/*.lua")
	for k,v in pairs(tools) do
		local toolName = v:match(".*/(.*).lua")
		local toolNameLower = toolName:lower()

		newoption({
			trigger     = "exclude-" .. toolNameLower,
			description = "Exclude the " .. toolName .. " tool from the build system"
		})

		if (not _OPTIONS["exclude-" .. toolNameLower]) then
			local f, err = loadfile(v)
			if (f) then
				TOOL = {}
				self:SetupInfoTable(TOOL)

				f()

				local succeed, err = self:RegisterTool(TOOL)
				if (not succeed) then
					print("Unable to register tool: " .. err)
				end
			else
				print("Unable to load tool file: " .. err)
			end
		end
	end
	TOOL = nil

	-- Examples
	if (_OPTIONS["with-examples"]) then
		local examples = os.matchdirs("../examples/*")
		for k,v in pairs(examples) do
			local dirName = v:match(".*/(.*)")
			if (dirName ~= "bin" and dirName ~= "build") then
				local f, err = loadfile(v .. "/build.lua")
				if (f) then
					EXAMPLE = {}
					EXAMPLE.Directory = dirName
					self:SetupInfoTable(EXAMPLE)

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
	
	-- Once everything is registred, let's process all the tables
	self.OrderedExamples = {}
	self.OrderedExtLibs  = {}
	self.OrderedModules  = {}
	self.OrderedTools    = {}
	local tables = {self.ExtLibs, self.Modules, self.Tools, self.Examples}
	local orderedTables = {self.OrderedExtLibs, self.OrderedModules, self.OrderedTools, self.OrderedExamples}
	for k,projects in ipairs(tables) do
        -- Begin by resolving every project (because of dependencies in the same category)
		for projectId,projectTable in pairs(projects) do
            self:Resolve(projectTable)
        end

		for projectId,projectTable in pairs(projects) do
			if (self:Process(projectTable)) then
                table.insert(orderedTables[k], projectTable)
            else
                print("Rejected client-only " .. projectTable.Name .. " " .. projectTable.Type) 
            end
		end
		
		table.sort(orderedTables[k], function (a, b) return a.Name < b.Name end)
	end
end

function NazaraBuild:RegisterAction(actionTable)
	if (actionTable.Name == nil or type(actionTable.Name) ~= "string" or string.len(actionTable.Name) == 0) then
		return false, "Invalid action name"
	end

	local lowerCaseName = string.lower(actionTable.Name)
	if (self.Actions[lowerCaseName] ~= nil) then
		return false, "This action name is already in use"
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

	self.Actions[lowerCaseName] = actionTable

	newaction
	{
		trigger     = lowerCaseName,
		description = actionTable.Description,
		execute     = function () actionTable:Function() end
	}

	return true
end

function NazaraBuild:RegisterExample(exampleTable)
	if (exampleTable.Name == nil or type(exampleTable.Name) ~= "string" or string.len(exampleTable.Name) == 0) then
		return false, "Invalid example name"
	end

	local lowerCaseName = exampleTable.Name:lower()
	if (self.Examples[lowerCaseName] ~= nil) then
		return false, "This library name is already in use"
	end

	if (exampleTable.Files == nil or type(exampleTable.Files) ~= "table") then
		return false, "Example files table is invalid"
	end

	if (#exampleTable.Files == 0) then
		return false, "This example has no files"
	end
	
	local files = {}
	for k, file in ipairs(exampleTable.Files) do
		table.insert(files, "../examples/" .. exampleTable.Directory .. "/" .. file)
	end
	exampleTable.Files = files
	
	exampleTable.Type = "Example"
	self.Examples[lowerCaseName] = exampleTable
	return true
end

function NazaraBuild:RegisterExternLibrary(libTable)
	if (libTable.Name == nil or type(libTable.Name) ~= "string" or string.len(libTable.Name) == 0) then
		return false, "Invalid library name"
	end

	local lowerCaseName = libTable.Name:lower()
	if (self.ExtLibs[lowerCaseName] ~= nil) then
		return false, "This library name is already in use"
	end

	if (libTable.Files == nil or type(libTable.Files) ~= "table") then
		return false, "Invalid file table"
	end

	if (#libTable.Files == 0) then
		return false, "This library has no files"
	end

	libTable.Type = "ExternLib"
	self.ExtLibs[lowerCaseName] = libTable
	return true
end

function NazaraBuild:RegisterModule(moduleTable)
	if (moduleTable.Name == nil or type(moduleTable.Name) ~= "string" or string.len(moduleTable.Name) == 0) then
		return false, "Invalid module name"
	end

	local lowerCaseName = moduleTable.Name:lower()
	if (self.Modules[lowerCaseName] ~= nil) then
		return false, "This module name is already in use"
	end

	table.insert(moduleTable.Defines, "NAZARA_" .. moduleTable.Name:upper() .. "_BUILD")
	table.insert(moduleTable.Files, "../include/Nazara/" .. moduleTable.Name .. "/**.hpp")
	table.insert(moduleTable.Files, "../include/Nazara/" .. moduleTable.Name .. "/**.inl")
	table.insert(moduleTable.Files, "../src/Nazara/" .. moduleTable.Name .. "/**.hpp")
	table.insert(moduleTable.Files, "../src/Nazara/" .. moduleTable.Name .. "/**.inl")
	table.insert(moduleTable.Files, "../src/Nazara/" .. moduleTable.Name .. "/**.cpp")
	
	if (_OPTIONS["united"] and lowerCaseName ~= "core") then
		table.insert(moduleTable.FilesExcluded, "../src/Nazara/" .. moduleTable.Name .. "/Debug/NewOverload.cpp")
	end

	moduleTable.Type = "Module"
	self.Modules[lowerCaseName] = moduleTable
	return true
end

function NazaraBuild:RegisterTool(toolTable)
	if (toolTable.Name == nil or type(toolTable.Name) ~= "string" or string.len(toolTable.Name) == 0) then
		return false, "Invalid tool name"
	end

	local lowerCaseName = toolTable.Name:lower()
	if (self.Tools[lowerCaseName] ~= nil) then
		return false, "This tool name is already in use"
	end

	if (toolTable.Directory == nil or type(toolTable.Directory) ~= "string" or string.len(toolTable.Directory) == 0) then
		return false, "Invalid tool directory"
	end

	if (toolTable.Kind == nil or type(toolTable.Kind) ~= "string" or string.len(toolTable.Kind) == 0) then
		return false, "Invalid tool type"
	end

	local lowerCaseKind = toolTable.Kind:lower()
	if (lowerCaseKind == "library" or lowerCaseKind == "plugin" or lowerCaseKind == "consoleapp" or lowerCaseKind == "windowapp") then
		toolTable.Kind = lowerCaseKind
	else
		return false, "Invalid tool type"
	end

	toolTable.Type = "Tool"
	self.Tools[lowerCaseName] = toolTable
	return true
end

local PosixOSes = {
	["bsd"] = true,
	["linux"] = true,
	["macosx"] = true,
	["solaris"] = true
}

function NazaraBuild:Process(infoTable)
    if (infoTable.ClientOnly and _OPTIONS["server"]) then
        return false
    end

	local libraries = {}    
	for k, library in pairs(infoTable.Libraries) do
		local moduleName = library:match("Nazara(%w+)")
		local moduleTable = moduleName and self.Modules[moduleName:lower()]
		local toolTable = moduleName and self.Tools[moduleName:lower()]
		
		if (moduleTable) then
            if (moduleTable.ClientOnly and _OPTIONS["server"]) then
                infoTable.ClientOnly = true
                return false -- We depend on a client-only library
            end

			if (_OPTIONS["united"]) then
				library = "NazaraEngine"
			else
				library = "Nazara" .. moduleTable.Name
			end

			if (not _OPTIONS["united"] or infoTable.Type ~= "Module") then
				table.insert(infoTable.ConfigurationLibraries.DebugStatic, library .. "-s-d")
				table.insert(infoTable.ConfigurationLibraries.ReleaseStatic, library .. "-s")
				table.insert(infoTable.ConfigurationLibraries.DebugDynamic, library .. "-d")
				table.insert(infoTable.ConfigurationLibraries.ReleaseDynamic, library)
			end
		else
			local extLibTable = self.ExtLibs[library:lower()]
			if (extLibTable) then
                if (extLibTable.ClientOnly and _OPTIONS["server"]) then
                    infoTable.ClientOnly = true
                    return false -- We depend on a client-only library
                end

				library = extLibTable.Name
				
				table.insert(infoTable.ConfigurationLibraries.DebugStatic, library .. "-s-d")
				table.insert(infoTable.ConfigurationLibraries.ReleaseStatic, library .. "-s")
				table.insert(infoTable.ConfigurationLibraries.DebugDynamic, library .. "-s-d")
				table.insert(infoTable.ConfigurationLibraries.ReleaseDynamic, library .. "-s")
			else
				if (toolTable and toolTable.Kind == "library") then
                    if (toolTable.ClientOnly and _OPTIONS["server"]) then
                        infoTable.ClientOnly = true
                        return false -- We depend on a client-only library
                    end

					library = "Nazara" .. toolTable.Name
					
					-- Import tools includes
					for k,v in ipairs(toolTable.Includes) do
						table.insert(infoTable.Includes, v)
					end
					
					-- And libraries
					for k, v in pairs(toolTable.Libraries) do
						table.insert(infoTable.Libraries, v)
					end
					
					for config, libs in pairs(toolTable.ConfigurationLibraries) do
						for k,v in pairs(libs) do
							table.insert(infoTable.ConfigurationLibraries[config], v)
						end
					end

					table.insert(infoTable.ConfigurationLibraries.DebugStatic, library .. "-s-d")
					table.insert(infoTable.ConfigurationLibraries.ReleaseStatic, library .. "-s")
					table.insert(infoTable.ConfigurationLibraries.DebugDynamic, library .. "-d")
					table.insert(infoTable.ConfigurationLibraries.ReleaseDynamic, library)
				else
					table.insert(libraries, library)
				end
			end
		end
	end
	infoTable.Libraries = libraries

	for k,v in pairs(infoTable) do
		local target = k:match("Os(%w+)")
		if (target) then
			local targetTable = infoTable[target]
			if (targetTable) then
				local excludeTargetTable = infoTable[target .. "Excluded"]
				for platform, defineTable in pairs(v) do
					platform = string.lower(platform)
					if (platform == "posix") then
						local osname = os.get()
						if (PosixOSes[osname]) then
							platform = osname
						end
					end

					if (os.is(platform)) then
						for k,v in ipairs(defineTable) do
							table.insert(targetTable, v)
						end
					elseif (excludeTargetTable) then
						for k,v in ipairs(defineTable) do
							table.insert(excludeTargetTable, v)
						end
					end
				end

				infoTable[k] = nil
			end
		end
	end

    return true
end

function NazaraBuild:Resolve(infoTable)
    if (type(infoTable.Libraries) == "function") then
        infoTable.Libraries = infoTable.Libraries()
    end
end

function NazaraBuild:MakeCopyAfterBuild(infoTable)
    if (PremakeVersion < 50) then
        return
    end

	if (os.is("windows")) then
		configuration({})
			postbuildcommands({[[xcopy "%{path.translate(cfg.linktarget.relpath):sub(1, -5) .. ".dll"}" "..\..\..\examples\bin\" /E /Y]]})

		for k,v in pairs(table.join(infoTable.Libraries, infoTable.DynLib)) do
			local paths = {}
			table.insert(paths, {"x32", "../extlibs/lib/common/x86/" .. v .. ".dll"})
			table.insert(paths, {"x32", "../extlibs/lib/common/x86/lib" .. v .. ".dll"})
			table.insert(paths, {"x64", "../extlibs/lib/common/x64/" .. v .. ".dll"})
			table.insert(paths, {"x64", "../extlibs/lib/common/x64/lib" .. v .. ".dll"})

			for k,v in pairs(paths) do
				local config = v[1]
				local path = v[2]
				if (os.isfile(path)) then
					if (infoTable.Kind == "plugin") then
						path = "../../" .. path
					end

					configuration(config)
					postbuildcommands({[[xcopy "%{path.translate(cfg.linktarget.relpath:sub(1, -#cfg.linktarget.name - 1) .. "../../]] .. path .. [[")}" "..\..\..\examples\bin\" /E /Y]]})
				end
			end
		end
	end
end

function NazaraBuild:SetupInfoTable(infoTable)
	infoTable.ConfigurationLibraries = {}
	infoTable.ConfigurationLibraries.DebugStatic = {}
	infoTable.ConfigurationLibraries.ReleaseStatic = {}
	infoTable.ConfigurationLibraries.DebugDynamic = {}
	infoTable.ConfigurationLibraries.ReleaseDynamic = {}
	
	local infos = {"Defines", "DynLib", "Files", "FilesExcluded", "Flags", "Includes", "Libraries"}
	for k,v in ipairs(infos) do
		infoTable[v] = {}
		infoTable["Os" .. v] = {}
	end
end