rule("msys2.isystem.fix")
	on_load(function (target)
		import("core.project.project")

		local external = project.policy("package.include_external_headers")
		if external == nil then
			external = target:policy("package.include_external_headers")
		end

		-- disable -isystem for packages as it seems broken on msys2 (see https://github.com/msys2/MINGW-packages/issues/10761)
		if external == nil then
			target:set("policy", "package.include_external_headers", false)
		end	
	end)
