-- Compile shaders to includables headers
rule("nzsl.compile.shaders")
	on_load(function (target)
		target:add("packages", "nzsl")
	end)

	before_buildcmd_file(function (target, batchcmds, shaderfile, opt)
		import("core.tool.toolchain")

		local nzslc = path.join(target:pkg("nzsl"):installdir(), "bin", "nzslc")

		-- add commands
		batchcmds:show_progress(opt.progress, "${color.build.object}compiling shader %s", shaderfile)
		local argv = { "--compile=nzslb-header", "--partial", "--optimize" }

		-- handle --log-format
		local kind = target:data("plugin.project.kind") or ""
		if kind:match("vs") then
			table.insert(argv, "--log-format=vs")
		end

		table.insert(argv, shaderfile)

		local envs
		if is_plat("mingw") then
			local mingw = toolchain.load("mingw")
			if mingw and mingw:check() then
				envs = mingw:runenvs()
			end
		end
	
		batchcmds:vrunv(nzslc, argv, { curdir = ".", envs = envs })

		local outputFile = path.join(path.directory(shaderfile), path.basename(shaderfile) .. ".nzslb.h")

		-- add deps
		batchcmds:add_depfiles(shaderfile)
		batchcmds:set_depmtime(os.mtime(outputFile))
		batchcmds:set_depcache(target:dependfile(outputFile))
	end)
