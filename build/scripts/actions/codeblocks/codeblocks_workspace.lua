--
-- Name:        codelite/codelite_workspace.lua
-- Purpose:     Generate a CodeLite workspace.
-- Author:      Ryan Pusztai
-- Modified by: Andrea Zanellato
--              Manu Evans
-- Created:     2013/05/06
-- Copyright:   (c) 2008-2015 Jason Perkins and the Premake project
--

	local p = premake
	local project = p.project
	local workspace = p.workspace
	local tree = p.tree
	local codeblocks = p.modules.codeblocks

	codeblocks.workspace = {}
	local m = codeblocks.workspace

--
-- Generate a CodeBlocks workspace
--
	function m.generate(wks)
		p.utf8()

		_p('<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>')
		_p('<CodeBlocks_workspace_file>')
		_p(1,'<Workspace title="%s">', wks.name)
		
		for prj in workspace.eachproject(wks) do
			local fname = path.join(path.getrelative(wks.location, prj.location), prj.name)
			local active = iif(prj.project == wks.projects[1], ' active="1"', '')
			
			_p(2,'<Project filename="%s.cbp"%s>', fname, active)
			for _,dep in ipairs(project.getdependencies(prj)) do
				_p(3,'<Depends filename="%s.cbp" />', path.join(path.getrelative(wks.location, dep.location), dep.name))
			end
		
			_p(2,'</Project>')
		end
		
		_p(1,'</Workspace>')
		_p('</CodeBlocks_workspace_file>')
	end