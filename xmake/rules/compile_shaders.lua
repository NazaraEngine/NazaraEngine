-- Compile shaders to includables headers
rule("compile_shaders")
	on_load(function (target)
		target:add("deps", "NazaraShaderCompiler")
		target:set("policy", "build.across_targets_in_parallel", false)
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
		local nzslc = target:dep("NazaraShaderCompiler")

		-- add commands
		batchcmds:show_progress(opt.progress, "${color.build.object}compiling shader %s", shaderfile)
		local argv = {"--compile", "--partial", "--header-file", "--log-format=vs", shaderfile}
		batchcmds:vrunv(nzslc:targetfile(), argv, { curdir = "." })

		local outputFile = path.join(path.directory(shaderfile), path.basename(shaderfile) .. ".nzslb.h")

		-- add deps
		batchcmds:add_depfiles(shaderfile)
		batchcmds:set_depmtime(os.mtime(outputFile))
		batchcmds:set_depcache(target:dependfile(outputFile))
	end)
