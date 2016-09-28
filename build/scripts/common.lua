NazaraBuild = {} -- L'équivalent d'un namespace en Lua est une table

function NazaraBuild:AddExecutablePath(path)
	self.ExecutableDir[path] = true
	self.InstallDir[path] = true
end

function NazaraBuild:AddInstallPath(path)
	self.InstallDir[path] = true
end

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

		if (self.Config["BuildDependencies"]) then
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

			configuration({"codeblocks or codelite or gmake", "x32"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x86")
				targetdir("../extlibs/lib/" .. makeLibDir .. "/x86")

			configuration({"codeblocks or codelite or gmake", "x64"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x64")
				targetdir("../extlibs/lib/" .. makeLibDir .. "/x64")

			configuration("vs*")
				buildoptions({"/MP", "/bigobj"}) -- Multiprocessus build and big .obj

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

			configuration({"not windows", "codeblocks or codelite or gmake or xcode3 or xcode4"})
				buildoptions("-fPIC")

			configuration("codeblocks or codelite or gmake or xcode3 or xcode4")
				buildoptions({"-std=c++14", "-U__STRICT_ANSI__"})

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

				configuration("x32")
					libdirs(libTable.LibraryPaths.x86)

				configuration("x64")
					libdirs(libTable.LibraryPaths.x64)

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

		configuration("vs*")
			buildoptions({"/MP", "/bigobj"}) -- Multiprocessus build and big .obj
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
				libdirs(moduleTable.LibraryPaths.x86)

			configuration("x64")
				defines("NAZARA_PLATFORM_x64")
				libdirs(moduleTable.LibraryPaths.x64)

			configuration({"codeblocks or codelite or gmake", "x32"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x86")
				libdirs("../lib/" .. makeLibDir .. "/x86")
				targetdir("../lib/" .. makeLibDir .. "/x86")

			configuration({"codeblocks or codelite or gmake", "x64"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x64")
				libdirs("../lib/" .. makeLibDir .. "/x64")
				targetdir("../lib/" .. makeLibDir .. "/x64")

			-- Copy the module binaries to the example folder
			self:MakeInstallCommands(moduleTable)

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
				defines("NAZARA_STATIC")
				kind("StaticLib")

			configuration("*Dynamic")
				kind("SharedLib")

			configuration("DebugStatic")
				targetsuffix("-s-d")

			configuration("ReleaseStatic")
				targetsuffix("-s")

			configuration("DebugDynamic")
				targetsuffix("-d")

			configuration("Release*")
				rtti(moduleTable.EnableRTTI and "On" or "Off")

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

			if (toolTable.Kind == "plugin" or toolTable.Kind == "library") then
				kind("SharedLib")
				
				-- Copy the tool binaries to the example folder
				self:MakeInstallCommands(toolTable)
			elseif (toolTable.Kind == "application") then
				debugdir(toolTable.TargetDirectory)
				targetdir(toolTable.TargetDirectory)
				if (toolTable.EnableConsole) then
					kind("ConsoleApp")
				else
					kind("WindowedApp")
				end
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
				libdirs(toolTable.LibraryPaths.x86)

			configuration("x64")
				defines("NAZARA_PLATFORM_x64")
				libdirs(toolTable.LibraryPaths.x64)

			configuration({"codeblocks or codelite or gmake", "x32"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x86")
				libdirs("../lib/" .. makeLibDir .. "/x86")
				if (toolTable.Kind == "library") then
					targetdir(toolTable.TargetDirectory .. "/" .. makeLibDir .. "/x86")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/" .. makeLibDir .. "/x86")
				end

			configuration({"codeblocks or codelite or gmake", "x64"})
				libdirs("../extlibs/lib/" .. makeLibDir .. "/x64")
				libdirs("../lib/" .. makeLibDir .. "/x64")
				if (toolTable.Kind == "library") then
					targetdir(toolTable.TargetDirectory .. "/" .. makeLibDir .. "/x64")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/" .. makeLibDir .. "/x64")
				end

			configuration({"vs*", "x32"})
				libdirs("../extlibs/lib/msvc/x86")
				libdirs("../lib/msvc/x86")
				if (toolTable.Kind == "library") then
					targetdir(toolTable.TargetDirectory .. "/msvc/x86")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/msvc/x86")
				end

			configuration({"vs*", "x64"})
				libdirs("../extlibs/lib/msvc/x64")
				libdirs("../lib/msvc/x64")
				if (toolTable.Kind == "library") then
					targetdir(toolTable.TargetDirectory .. "/msvc/x64")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/msvc/x64")
				end

			configuration({"xcode3 or xcode4", "x32"})
				libdirs("../extlibs/lib/xcode/x86")
				libdirs("../lib/xcode/x86")
				if (toolTable.Kind == "library") then
					targetdir(toolTable.TargetDirectory .. "/xcode/x86")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/xcode/x86")
				end

			configuration({"xcode3 or xcode4", "x64"})
				libdirs("../extlibs/lib/xcode/x64")
				libdirs("../lib/xcode/x64")
				if (toolTable.Kind == "library") then
					targetdir(toolTable.TargetDirectory .. "/xcode/x64")
				elseif (toolTable.Kind == "plugin") then
					targetdir("../plugins/" .. toolTable.Name .. "/lib/xcode/x64")
				end

			configuration("*Static")
				defines("NAZARA_STATIC")

			configuration("Release*")
				rtti(toolTable.EnableRTTI and "On" or "Off")

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
			local destPath = "../examples/bin"
				
			project("Demo" .. exampleTable.Name)

			location(_ACTION .. "/examples")

			if (exampleTable.Kind == "plugin" or exampleTable.Kind == "library") then
				kind("SharedLib")
				
				self:MakeInstallCommands(toolTable)
			elseif (exampleTable.Kind == "application") then
				debugdir(exampleTable.TargetDirectory)
				if (exampleTable.EnableConsole) then
					kind("ConsoleApp")
				else
					kind("WindowedApp")
				end
			else
				assert(false, "Invalid tool Kind")
			end

			debugdir(destPath)
			includedirs({
				"../include",
				"../extlibs/include"
			})
			libdirs("../lib")
			targetdir(destPath)

			files(exampleTable.Files)
			excludes(exampleTable.FilesExcluded)

			defines(exampleTable.Defines)
			flags(exampleTable.Flags)
			includedirs(exampleTable.Includes)
			links(exampleTable.Libraries)

			configuration("x32")
				libdirs(exampleTable.LibraryPaths.x86)

			configuration("x64")
				defines("NAZARA_PLATFORM_x64")
				libdirs(exampleTable.LibraryPaths.x64)

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

function NazaraBuild:GetConfig()
	return self.Config
end

function NazaraBuild:GetDependency(infoTable, name)
	local projectName = name:match("Nazara(%w+)")
	if (projectName) then
		-- tool or module
		local moduleTable = self.Modules[projectName:lower()]
		if (moduleTable) then
			return moduleTable
		else
			local toolTable = self.Tools[projectName:lower()]
			if (toolTable) then
				return toolTable
			end
		end
	else
		return self.ExtLibs[name:lower()]
	end
end

function NazaraBuild:Initialize()
	self.Actions = {}
	self.Examples = {}
	self.ExecutableDir = {}
	self.ExtLibs = {}
	self.InstallDir = {}
	self.Modules = {}
	self.Tools = {}
	
	self.Config = {}
	self:LoadConfig()

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
			self:SetupExtlibTable(LIBRARY)

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

		local f, err = loadfile(v)
		if (f) then
			MODULE = {}
			self:SetupModuleTable(MODULE)

			f()

			local succeed, err = self:RegisterModule(MODULE)
			if (not succeed) then
				print("Unable to register module: " .. err)
			end
		else
			print("Unable to load module file: " .. err)
		end
	end
	MODULE = nil

	-- Continue with the tools (ex: SDK)
	local tools = os.matchfiles("scripts/tools/*.lua")
	for k,v in pairs(tools) do
		local toolName = v:match(".*/(.*).lua")
		local toolNameLower = toolName:lower()

		local f, err = loadfile(v)
		if (f) then
			TOOL = {}
			self:SetupToolTable(TOOL)

			f()

			local succeed, err = self:RegisterTool(TOOL)
			if (not succeed) then
				print("Unable to register tool " .. tostring(TOOL.Name) .. ": " .. err)
			end
		else
			print("Unable to load tool file " .. v .. ": " .. err)
		end
	end
	TOOL = nil

	-- Examples
	if (self.Config["BuildExamples"]) then
		local examples = os.matchdirs("../examples/*")
		for k,v in pairs(examples) do
			local dirName = v:match(".*/(.*)")
			if (dirName ~= "bin" and dirName ~= "build") then
				local f, err = loadfile(v .. "/build.lua")
				if (f) then
					EXAMPLE = {}
					EXAMPLE.Directory = dirName
					self:SetupExampleTable(EXAMPLE)

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
				print("Rejected " .. projectTable.Name .. " " .. string.lower(projectTable.Type) .. ": " .. projectTable.ExcludeReason)
			end
		end

		table.sort(orderedTables[k], function (a, b) return a.Name < b.Name end)
	end
end

function NazaraBuild:LoadConfig()
    local f = io.open("config.lua", "r")
	if (f) then
	    local content = f:read("*a")
		f:close()

		local func, err = loadstring(content)
		if (func) then
			setfenv(func, self.Config)

			local status, err = pcall(func)
			if (not status) then
				print("Failed to load config.lua: " .. err)
			end
		else
			print("Failed to parse config.lua: " .. err)
		end
	else
		print("Failed to open config.lua")
	end

	local configTable = self.Config
	local AddBoolOption = function (option, name, description)
		newoption({
			trigger     = name,
			description = description
		})

		local str = _OPTIONS[name]
		if (str) then
			if (#str == 0 or str == "1" or str == "yes" or str == "true") then
				configTable[option] = true
			elseif (str == "0" or str == "no" or str == "false") then
				configTable[option] = false
			else
				error("Invalid entry for " .. name .. " option: \"" .. str .. "\"")
			end
		end
	end

	AddBoolOption("BuildDependencies", "with-extlibs", "Builds the extern libraries")
	AddBoolOption("BuildExamples", "with-examples", "Builds the examples")
	AddBoolOption("ServerMode", "server", "Excludes client-only modules/tools/examples")
	AddBoolOption("UniteModules", "united", "Builds all the modules as one united library")

	-- InstallDir
	newoption({
		trigger     = "install-path",
		description = "Setup additionnals install directories (library binaries will be copied there)"
	})

	self.Config["InstallDir"] = self.Config["InstallDir"] or ""
	if (_OPTIONS["install-path"] ~= nil) then
		self.Config["InstallDir"] = self.Config["InstallDir"] .. ";" .. _OPTIONS["install-path"]
	end

	local paths = string.explode(self.Config["InstallDir"], ";")
	for k,v in pairs(paths) do
		if (#v > 0) then
			self:AddInstallPath(v)
		end
	end
end

function NazaraBuild:MakeInstallCommands(infoTable)
	if (PremakeVersion < 50) then
		return
	end

	if (os.is("windows")) then
		configuration("*Dynamic")
		
		for k,v in pairs(self.InstallDir) do
			local destPath = path.translate(path.isabsolute(k) and k or "../../" .. k)
			postbuildcommands({[[xcopy "%{path.translate(cfg.linktarget.relpath):sub(1, -5) .. ".dll"}" "]] .. destPath .. [[\" /E /Y]]})
		end

		for k,fileName in pairs(table.join(infoTable.Libraries, infoTable.DynLib)) do
			local paths = {}
			for k,v in pairs(infoTable.BinaryPaths.x86) do
				table.insert(paths, {"x32", v .. "/" .. fileName .. ".dll"})
				table.insert(paths, {"x32", v .. "/lib" .. fileName .. ".dll"})
			end

			for k,v in pairs(infoTable.BinaryPaths.x64) do
				table.insert(paths, {"x64", v .. "/" .. fileName .. ".dll"})
				table.insert(paths, {"x64", v .. "/lib" .. fileName .. ".dll"})
			end

			for k,v in pairs(paths) do
				local config = v[1]
				local srcPath = v[2]
				if (os.isfile(srcPath)) then
					if (infoTable.Kind == "plugin") then
						srcPath = "../../" .. srcPath
					end

					configuration(config)
					
					for k,v in pairs(self.ExecutableDir) do
						local srcPath = path.isabsolute(srcPath) and path.translate(srcPath) or [[%{path.translate(cfg.linktarget.relpath:sub(1, -#cfg.linktarget.name - 1) .. "../../]] .. srcPath .. [[")}]]
						local destPath = path.translate(path.isabsolute(k) and k or "../../" .. k)
						postbuildcommands({[[xcopy "]] .. srcPath .. [[" "]] .. destPath .. [[\" /E /Y]]})
					end
				end
			end
		end
	end
end

local PosixOSes = {
	["bsd"] = true,
	["linux"] = true,
	["macosx"] = true,
	["solaris"] = true
}

function NazaraBuild:Process(infoTable)
	local libraries = {}
	for k, library in pairs(infoTable.Libraries) do
		local libraryTable = self:GetDependency(infoTable, library)
		if (libraryTable) then
			if (libraryTable.Excluded) then
				infoTable.Excluded = true
				infoTable.ExcludeReason = "depends on excluded " .. library .. " " .. libraryTable.Type:lower()
				return false
			end

			if (libraryTable.Type == "Module") then
				if (_OPTIONS["united"]) then
					library = "NazaraEngine"
				else
					library = "Nazara" .. libraryTable.Name
				end

				if (not self.Config["UniteModules"] or infoTable.Type ~= "Module") then
					table.insert(infoTable.ConfigurationLibraries.DebugStatic, library .. "-s-d")
					table.insert(infoTable.ConfigurationLibraries.ReleaseStatic, library .. "-s")
					table.insert(infoTable.ConfigurationLibraries.DebugDynamic, library .. "-d")
					table.insert(infoTable.ConfigurationLibraries.ReleaseDynamic, library)
				end
			elseif (libraryTable.Type == "ExternLib") then
				library = libraryTable.Name

				table.insert(infoTable.ConfigurationLibraries.DebugStatic, library .. "-s-d")
				table.insert(infoTable.ConfigurationLibraries.ReleaseStatic, library .. "-s")
				table.insert(infoTable.ConfigurationLibraries.DebugDynamic, library .. "-s-d")
				table.insert(infoTable.ConfigurationLibraries.ReleaseDynamic, library .. "-s")
			elseif (libraryTable.Type == "Tool") then
				library = "Nazara" .. libraryTable.Name

				-- Import tools includes
				for k,v in ipairs(libraryTable.Includes) do
					table.insert(infoTable.Includes, v)
				end

				-- And libraries
				for k, v in pairs(libraryTable.Libraries) do
					table.insert(infoTable.Libraries, v)
				end

				for config, libs in pairs(libraryTable.ConfigurationLibraries) do
					for k,v in pairs(libs) do
						table.insert(infoTable.ConfigurationLibraries[config], v)
					end
				end

				table.insert(infoTable.ConfigurationLibraries.DebugStatic, library .. "-s-d")
				table.insert(infoTable.ConfigurationLibraries.ReleaseStatic, library .. "-s")
				table.insert(infoTable.ConfigurationLibraries.DebugDynamic, library .. "-d")
				table.insert(infoTable.ConfigurationLibraries.ReleaseDynamic, library)
			else
				infoTable.Excluded = true
				infoTable.ExcludeReason = "dependency " .. library .. " has invalid type \"" .. libraryTable.Type .. "\""
				return false
			end		
		else
			table.insert(libraries, library)
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

	if (infoTable.Kind == "application") then
		self:AddExecutablePath(infoTable.TargetDirectory)
	end

	if (infoTable.Validate) then
		local ret, err = infoTable:Validate()
		if (not ret) then
			infoTable.Excluded = true
			infoTable.ExcludeReason = "validation failed: " .. err
			return false
		end
	end
	
	return true
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

	if (self.Config["UniteModules"] and lowerCaseName ~= "core") then
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

	if (toolTable.Kind == nil or type(toolTable.Kind) ~= "string" or string.len(toolTable.Kind) == 0) then
		return false, "Invalid tool type"
	end

	local lowerCaseKind = toolTable.Kind:lower()
	if (lowerCaseKind == "library" or lowerCaseKind == "plugin" or lowerCaseKind == "application") then
		toolTable.Kind = lowerCaseKind
	else
		return false, "Invalid tool type"
	end

	if (lowerCaseKind ~= "plugin" and (toolTable.TargetDirectory == nil or type(toolTable.TargetDirectory) ~= "string" or string.len(toolTable.TargetDirectory) == 0)) then
		return false, "Invalid tool directory"
	end

	toolTable.Type = "Tool"
	self.Tools[lowerCaseName] = toolTable
	return true
end

function NazaraBuild:Resolve(infoTable)
	if (infoTable.ClientOnly and self.Config["ServerMode"]) then
		infoTable.Excluded = true
		infoTable.ExcludeReason = "excluded by command-line options (client-only)"
	end

	if (infoTable.Excludable) then
		local optionName = "excludes-" .. string.lower(infoTable.Type .. "-" .. infoTable.Name)
		newoption({
			trigger     = optionName,
			description = "Excludes the " .. infoTable.Name .. " " .. string.lower(infoTable.Type) .. " and projects relying on it"
		})

		if (_OPTIONS[optionName]) then
			infoTable.Excluded = true
			infoTable.ExcludeReason = "excluded by command-line options"
		end
	end

	if (type(infoTable.Libraries) == "function") then
		infoTable.Libraries = infoTable.Libraries()
	end
end

function NazaraBuild:SetupInfoTable(infoTable)
	infoTable.BinaryPaths = {}
	infoTable.BinaryPaths.x86 = {}
	infoTable.BinaryPaths.x64 = {}
	infoTable.ConfigurationLibraries = {}
	infoTable.ConfigurationLibraries.DebugStatic = {}
	infoTable.ConfigurationLibraries.ReleaseStatic = {}
	infoTable.ConfigurationLibraries.DebugDynamic = {}
	infoTable.ConfigurationLibraries.ReleaseDynamic = {}
	infoTable.Excludable = true
	infoTable.LibraryPaths = {}
	infoTable.LibraryPaths.x86 = {}
	infoTable.LibraryPaths.x64 = {}

	local infos = {"Defines", "DynLib", "Files", "FilesExcluded", "Flags", "Includes", "Libraries"}
	for k,v in ipairs(infos) do
		infoTable[v] = {}
		infoTable["Os" .. v] = {}
	end
end

function NazaraBuild:SetupExampleTable(infoTable)
	self:SetupInfoTable(infoTable)

	infoTable.Kind = "application"
	infoTable.TargetDirectory = "../examples/bin"
end

function NazaraBuild:SetupExtlibTable(infoTable)
	self:SetupInfoTable(infoTable)

	infoTable.Kind = "library"

	table.insert(infoTable.BinaryPaths.x86, "../extlibs/lib/common/x86")
	table.insert(infoTable.BinaryPaths.x64, "../extlibs/lib/common/x64")
	table.insert(infoTable.LibraryPaths.x86, "../extlibs/lib/common/x86")
	table.insert(infoTable.LibraryPaths.x64, "../extlibs/lib/common/x64")
end

function NazaraBuild:SetupModuleTable(infoTable)
	self:SetupInfoTable(infoTable)

	infoTable.Kind = "library"

	table.insert(infoTable.BinaryPaths.x86, "../extlibs/lib/common/x86")
	table.insert(infoTable.BinaryPaths.x64, "../extlibs/lib/common/x64")
	table.insert(infoTable.LibraryPaths.x86, "../extlibs/lib/common/x86")
	table.insert(infoTable.LibraryPaths.x64, "../extlibs/lib/common/x64")
end

NazaraBuild.SetupToolTable = NazaraBuild.SetupInfoTable
