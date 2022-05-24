task("compile-shaders")

	set_menu({
		-- Settings menu usage
		usage = "xmake compile-shaders [options]",
		description = "Compile engine shaders"
	})

	on_run(function ()
		import("core.project.project")
		local nzsl = path.join(project.required_package("nzsl"):installdir(), "bin", "nzslc")

		print("Compiling shaders...")
		for _, filepath in pairs(os.files("src/Nazara/*/Resources/**.nzsl")) do
			print(" - Compiling " .. filepath)
			local argv = {"--compile=nzslb", "--partial", "--header-file", filepath }
			os.execv(nzsl, argv)
		end
	end)
