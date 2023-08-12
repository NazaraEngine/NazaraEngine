-- Compile shaders to includables headers
rule("nzsl.compile.shaders")
	set_extensions(".nzsl", ".nzslb")

	before_buildcmd_file(function (target, batchcmds, shaderfile, opt)
		import("core.project.project")
		import("core.tool.toolchain")

		local nzsl = project.required_package("nzsl~host") or project.required_package("nzsl")
		assert(nzsl, "nzsl package not found")

		-- warning: project.required_package is not a stable interface, this may break in the future
		local nzslc = path.join(nzsl:installdir(), "bin", "nzslc")

		-- add commands
		batchcmds:show_progress(opt.progress, "${color.build.object}compiling.shader %s", shaderfile)
		local argv = { "--compile=nzslb-header", "--partial", "--optimize" }

		-- handle --log-format
		local kind = target:data("plugin.project.kind") or ""
		if kind:match("vs") then
			table.insert(argv, "--log-format=vs")
		end

		table.insert(argv, shaderfile)

		-- on mingw we need run envs because of .dll dependencies which may be not part of the PATH
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
		batchcmds:add_depvalues(nzsl:version())
		batchcmds:set_depmtime(os.mtime(outputFile))
		batchcmds:set_depcache(target:dependfile(outputFile))
	end)
