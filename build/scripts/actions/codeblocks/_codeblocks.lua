--
-- _codeblocks.lua
-- Define the Code::Blocks action(s).
-- Copyright (c) 2002-2011 Jason Perkins and the Premake project
--
	local p = premake

	p.modules.codeblocks = {}
	p.modules.codeblocks._VERSION = p._VERSION

	local codeblocks = p.modules.codeblocks

	newaction {
		trigger         = "codeblocks",
		shortname       = "Code::Blocks",
		description     = "Generate Code::Blocks project files",
		
		valid_kinds     = { "ConsoleApp", "WindowedApp", "StaticLib", "SharedLib" },
		
		valid_languages = { "C", "C++" },
		
		valid_tools     = {
			cc   = { "clang", "gcc", "ow" },
		},
		
		onWorkspace = function(wks)
			p.modules.codeblocks.generateWorkspace(wks)
		end,
		
		onProject = function(prj)
			p.modules.codeblocks.generateProject(prj)
		end,
		
		onCleanWorkspace = function(wks)
			p.clean.file(wks, wks.name .. ".workspace")
			p.clean.file(wks, wks.name .. ".workspace.layout")
		end,

		onCleanProject = function(prj)
			p.clean.file(prj, prj.name .. ".workspace")
			p.clean.file(prj, prj.name .. ".depend")
			p.clean.file(prj, prj.name .. ".layout")
		end
	}
