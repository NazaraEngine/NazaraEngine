task("compile-shaders")

	set_menu({
		-- Settings menu usage
		usage = "xmake compile-shaders [options]",
		description = "Compile engine shaders"
	})

	on_run(function ()
		import("core.project.project")
		local nzsl = path.join(project.required_package("nzsl"):installdir(), "bin", "nzslc")

		local envs
		if is_plat("mingw") then
			local mingw = toolchain.load("mingw")
			if mingw and mingw:check() then
				envs = mingw:runenvs()
			end
		end
		
		print("Compiling shaders...")
		for _, filepath in pairs(os.files("src/Nazara/*/Resources/**.nzsl")) do
			print(" - Compiling " .. filepath)
			local argv = {"--compile=nzslb", "--partial", "--header-file", filepath }
			os.execv(nzsl, argv, { envs = envs })
		end
	end)
