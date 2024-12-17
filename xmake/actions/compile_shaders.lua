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
		import("core.base.task")
		import("core.project.project")

		task.run("config", {}, {disable_dump = true})

		local nzsl = project.required_package("nzsl")
		local nzsla = path.join(nzsl:installdir(), "bin", "nzsla")
		local nzslc = path.join(nzsl:installdir(), "bin", "nzslc")

		local envs = nzsl:get("envs")
		if is_plat("mingw") then
			local mingw = toolchain.load("mingw")
			if mingw and mingw:check() then
				for name, value in pairs(mingw:runenvs()) do
					envs[name] = table.join(envs[name] or {}, table.wrap(value))
				end
			end
		end

		print("Compiling standalone shaders...")
		for _, filepath in pairs(os.files("src/Nazara/*/Shaders/**.nzsl")) do
			print(" - Compiling " .. filepath)
			local argv = {"--compile=nzslb-header", "--partial", "--optimize" }
			if option.get("measure") then
				table.insert(argv, "--measure")
			end
			if option.get("benchmark-iteration") then
				table.insert(argv, "--benchmark-iteration=" .. option.get("benchmark-iteration"))
			end
			table.insert(argv, filepath)
			os.vrunv(nzslc, argv, { envs = envs })
		end

		print("Compiling shader archives...")
		for _, archivefolder in pairs(os.dirs("src/Nazara/*/ShaderArchives/*")) do
			print(" - Compiling " .. archivefolder .. " shaders")
			local archive = archivefolder .. ".nzsla.h"
			local nzsla_argv = {"--archive", "--compress=lz4hc", "--header" }
			table.insert(nzsla_argv, "--output=" .. archive)

			for _, filepath in pairs(os.files(archivefolder .. "/**.nzsl")) do
				print("   - Compiling " .. filepath .. "...")
				local nzslc_argvs = {"--compile=nzslb", "--partial", "--optimize" }
				if option.get("measure") then
					table.insert(nzslc_argvs, "--measure")
				end
				if option.get("benchmark-iteration") then
					table.insert(nzslc_argvs, "--benchmark-iteration=" .. option.get("benchmark-iteration"))
				end
				table.insert(nzslc_argvs, filepath)
				os.vrunv(nzslc, nzslc_argvs, { envs = envs })

				table.insert(nzsla_argv, path.join(path.directory(filepath), path.basename(filepath) .. ".nzslb"))
			end
			
			os.vrunv(nzsla, nzsla_argv, { envs = envs })
		end
	end)
