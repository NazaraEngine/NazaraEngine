task("compile-shaders")

	set_menu({
		-- Settings menu usage
		usage = "xmake compile-shaders [options]",
		description = "Compile engine shaders",
		options = {
			{nil, "measure", "k", nil, "Measure time taken for every step of the shader compilation." },
			{nil, "benchmark-iteration", "kv", nil, "Benchmark using a number of iterations." },
		}
	})

	on_run(function ()
		import("core.base.option")
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
			local argv = {"--compile=nzslb-header", "--partial", filepath }
			if option.get("measure") then
				table.insert(argv, "--measure")
			end
			if option.get("benchmark-iteration") then
				table.insert(argv, "--benchmark-iteration=" .. option.get("benchmark-iteration"))
			end
			os.execv(nzsl, argv, { envs = envs })
		end
	end)
