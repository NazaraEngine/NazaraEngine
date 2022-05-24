-- Compile shaders to includables headers
rule("compile_shaders")
	on_load(function (target)
		target:add("packages", "nzsl")
	end)

	-- temporary fix
    before_build("mingw", function (target)
		local mingw = target:toolchain("mingw")
		local bindir = mingw:bindir()
		local targetdir = target:targetdir()
		if bindir then
			os.trycp(path.join(bindir, "libgcc_s_seh-1.dll"), targetdir)
			os.trycp(path.join(bindir, "libstdc++-6.dll"), targetdir)
			os.trycp(path.join(bindir, "libwinpthread-1.dll"), targetdir)
		end
	end)

	before_buildcmd_file(function (target, batchcmds, shaderfile, opt)
		local nzsl = path.join(target:pkg("nzsl"):installdir(), "bin", "nzslc")

		-- add commands
		batchcmds:show_progress(opt.progress, "${color.build.object}compiling shader %s", shaderfile)
		local argv = {"--compile=nzslb", "--partial", "--header-file"}

		-- handle --log-format
		local kind = target:data("plugin.project.kind") or ""
		if kind:match("vs") then
			table.insert(argv, "--log-format=vs")
		end

		table.insert(argv, shaderfile)

		batchcmds:vrunv(nzsl, argv, { curdir = "." })

		local outputFile = path.join(path.directory(shaderfile), path.basename(shaderfile) .. ".nzslb.h")

		-- add deps
		batchcmds:add_depfiles(shaderfile)
		batchcmds:set_depmtime(os.mtime(outputFile))
		batchcmds:set_depcache(target:dependfile(outputFile))
	end)
