option("shadernodes")
	set_default(false)
	set_showmenu(true)
	set_description("Build ShaderNodes tool")

option_end()

if has_config("shadernodes") then
	add_requires("nodeeditor", "qt5core", "qt5gui", "qt5widgets", {debug = is_mode("debug")})

	rule("qt5.env")
		after_load(function (target)
			-- retrieve qtbase
			local qtdir

			local qtbase = target:pkg("qt5base")
			if qtbase then
				qtdir = assert(qtbase:get("qtdir"))
			else
				local qtcore = target:pkg("qt5core")
				if not qtcore then
					os.raise("target " .. target:name() .. " does not use qt5")
				end
				qtdir = assert(qtcore:get("qtdir"))
			end

			target:data_set("qt", qtdir)
		end)


	rule("qt5.moc")
		add_deps("qt5.env")
		set_extensions(".h", ".hpp")
		before_buildcmd_file(function (target, batchcmds, sourcefile, opt)
			local qtbase = target:dep("qt5core")
			local qt = assert(qtbase:fetch().qtbase, "qt not found!")

			-- imports
			import("core.tool.compiler")

			-- get moc
			local moc = path.join(qt.bindir, is_host("windows") and "moc.exe" or "moc")
			if not os.isexec(moc) and qt.libexecdir then
				moc = path.join(qt.libexecdir, is_host("windows") and "moc.exe" or "moc")
			end
			assert(moc and os.isexec(moc), "moc not found!")

			-- get c++ source file for moc
			--
			-- add_files("mainwindow.h") -> moc_MainWindow.cpp
			-- add_files("mainwindow.cpp", {rules = "qt.moc"}) -> mainwindow.moc, @see https://github.com/xmake-io/xmake/issues/750
			--
			local basename = path.basename(sourcefile)
			local filename_moc = "moc_" .. basename .. ".cpp"
			if sourcefile:endswith(".cpp") then
				filename_moc = basename .. ".moc"
			end
			local sourcefile_moc = path.join(target:autogendir(), "rules", "qt", "moc", filename_moc)

			-- add objectfile
			local objectfile = target:objectfile(sourcefile_moc)
			table.insert(target:objectfiles(), objectfile)

			-- add commands
			batchcmds:show_progress(opt.progress, "${color.build.object}compiling.qt.moc %s", sourcefile)

			-- generate c++ source file for moc
			local flags = {}
			table.join2(flags, compiler.map_flags("cxx", "define", target:get("defines")))
			table.join2(flags, compiler.map_flags("cxx", "includedir", target:get("includedirs")))
			table.join2(flags, compiler.map_flags("cxx", "includedir", target:get("sysincludedirs"))) -- for now, moc process doesn't support MSVC external includes flags and will fail
			table.join2(flags, compiler.map_flags("cxx", "frameworkdir", target:get("frameworkdirs")))
			batchcmds:mkdir(path.directory(sourcefile_moc))
			batchcmds:vrunv(moc, table.join(flags, sourcefile, "-o", sourcefile_moc))

			-- we need compile this moc_xxx.cpp file if exists Q_PRIVATE_SLOT, @see https://github.com/xmake-io/xmake/issues/750
			local mocdata = io.readfile(sourcefile)
			if mocdata and mocdata:find("Q_PRIVATE_SLOT") or sourcefile_moc:endswith(".moc") then
				-- add includedirs of sourcefile_moc
				target:add("includedirs", path.directory(sourcefile_moc))

				-- remove the object file of sourcefile_moc
				local objectfiles = target:objectfiles()
				for idx, objectfile in ipairs(objectfiles) do
					if objectfile == target:objectfile(sourcefile_moc) then
						table.remove(objectfiles, idx)
						break
					end
				end
			else
				-- compile c++ source file for moc
				batchcmds:compile(sourcefile_moc, objectfile)
			end

			-- add deps
			batchcmds:add_depfiles(sourcefile)
			batchcmds:set_depmtime(os.mtime(objectfile))
			batchcmds:set_depcache(target:dependfile(objectfile))
		end)

	target("NazaraShaderNodes")
		set_group("Tools")
		set_kind("binary")
		add_rules("qt5.moc")

		add_deps("NazaraCore")
		add_packages("nzsl")
		add_packages("nodeeditor")
		add_packages("qt5core", "qt5gui", "qt5widgets")
		if not is_plat("windows") then
			add_cxflags("-fPIC")
		end

		if has_config("unitybuild") then
			add_rules("c++.unity_build")
		end

		add_includedirs("../src")
		add_headerfiles("../src/ShaderNode/**.hpp", "../src/ShaderNode/**.inl", { prefixdir = "private", install = false })
		add_files("../src/ShaderNode/**.cpp")
		add_files("../src/ShaderNode/Previews/PreviewValues.cpp", { unity_ignored = true }) -- fixes an issue with MSVC and operator*

end
