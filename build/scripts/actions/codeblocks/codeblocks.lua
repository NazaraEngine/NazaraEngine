--
-- codeblocks_workspace.lua
-- Generate a Code::Blocks workspace.
-- Copyright (c) 2009 Jason Perkins and the Premake project
--

	local p = premake

	p.modules.codeblocks = {}
	p.modules.codeblocks._VERSION = p._VERSION

	local codeblocks = p.modules.codeblocks
	local project = p.project


	function codeblocks.cfgname(cfg)
		local cfgname = cfg.buildcfg
		if codeblocks.workspace.multiplePlatforms then
			cfgname = string.format("%s|%s", cfg.platform, cfg.buildcfg)
		end
		return cfgname
	end

	function codeblocks.esc(value)
		local result = value:gsub('"', '&quot;')
		result = result:gsub('<', '&lt;')
		result = result:gsub('>', '&gt;')
		return result
	end

	function codeblocks.generateWorkspace(wks)
		p.eol("\r\n")
		p.indent("\t")
		p.escaper(codeblocks.esc)

		p.generate(wks, ".workspace", codeblocks.workspace.generate)
	end

	function codeblocks.generateProject(prj)
		p.eol("\r\n")
		p.indent("\t")
		p.escaper(codeblocks.esc)

		if project.iscpp(prj) then
			p.generate(prj, ".cbp", codeblocks.project.generate)
		end
	end

	function codeblocks.cleanWorkspace(wks)
		p.clean.file(wks, wks.name .. ".workspace")
		p.clean.file(wks, wks.name .. ".workspace.layout")
	end

	function codeblocks.cleanProject(prj)
			p.clean.file(prj, prj.name .. ".workspace")
			p.clean.file(prj, prj.name .. ".depend")
			p.clean.file(prj, prj.name .. ".layout")
	end

	function codeblocks.cleanTarget(prj)
		-- TODO..
	end

	include("codeblocks_workspace.lua")
	include("codeblocks_project.lua")


