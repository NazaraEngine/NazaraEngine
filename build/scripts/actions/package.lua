newoption({
	trigger     = "pack-libdir",
	description = "Specifiy the subdirectory in lib/ to be used when packaging the project"
})

ACTION.Name = "Package"
ACTION.Description = "Pack Nazara binaries/include/lib together"

ACTION.Function = function ()
	local libDir = _OPTIONS["pack-libdir"]
	if (not libDir or #libDir == 0) then
		local libDirs = os.matchdirs("../lib/*")
		if (#libDirs > 1) then
			error("More than one subdirectory was found in the lib directory, please use the --pack-libdir command to clarify which directory should be used")
		elseif (#libDirs == 0) then
			error("No subdirectory was found in the lib directory, have you built the engine yet?")
		else
			libDir = path.getname(libDirs[1])
			print("No directory was set by the --pack-libdir command, \"" .. libDir .. "\" will be used")
		end
	end

	local realLibDir = "../lib/" .. libDir .. "/"
	if (not os.isdir(realLibDir)) then
		error(string.format("\"%s\" doesn't seem to be an existing directory", realLibDir))
	end

	local archEnabled = {
		["x64"] = false,
		["x86"] = false
	}

	local enabledArchs = {}
	for k,v in pairs(os.matchdirs(realLibDir .. "*")) do
		local arch = path.getname(v)
		if (archEnabled[arch] ~= nil) then
			archEnabled[arch] = true
			table.insert(enabledArchs, arch)
		else
			print("Unknown directory " .. v .. " found, ignored")
		end
	end
	enabledArchs = table.concat(enabledArchs, ", ")
	print(enabledArchs .. " arch found")

	local packageDir = "../package/"

	local copyTargets = {
		{ -- Engine headers
			Masks = {"**.hpp", "**.inl"},
			Source = "../include/",
			Target = "include/"
		},
		{ -- SDK headers
			Masks = {"**.hpp", "**.inl"},
			Source = "../SDK/include/",
			Target = "include/"
		},
		{ -- Examples files
			Masks = {"**.hpp", "**.inl", "**.cpp"},
			Source = "../examples/",
			Target = "examples/"
		},
		{ -- Demo resources
			Masks = {"**.*"},
			Source = "../examples/bin/resources/",
			Target = "examples/bin/resources/"
		},
		-- Unit test sources
		{
			Masks = {"**.hpp", "**.inl", "**.cpp"},
			Source = "../tests/",
			Target = "tests/src/"
		},
		-- Unit test resources
		{
			Masks = {"**.*"},
			Source = "../tests/resources/",
			Target = "tests/resources/"
		}
	}

	local binFileMasks
	local libFileMasks
	local exeFileExt
	local exeFilterFunc
	if (os.is("windows")) then	
		binFileMasks = {"**.dll", "**.pdb"}
		libFileMasks = {"**.lib", "**.a"}
		exeFileExt = ".exe"
		exeFilterFunc = function (filePath) return true end
	else
		if (os.is("macosx")) then
			binFileMasks = {"**.dynlib"}
		else
			binFileMasks = {"**.so"}
		end

		libFileMasks = {"**.a"}
		exeFileExt = ""
		exeFilterFunc = function (filePath) return path.getextension(filePath):contains('/') end
	end

	for arch, enabled in pairs(archEnabled) do
		if (enabled) then
			local archLibSrc = realLibDir .. arch .. "/"
			local arch3rdPartyBinSrc = "../extlibs/lib/common/" .. arch .. "/"
			local archBinDst = "bin/" .. arch .. "/"
			local archLibDst = "lib/" .. arch .. "/"
			
			-- Engine/SDK binaries
			table.insert(copyTargets, { 
				Masks  = binFileMasks,
				Source = archLibSrc,
				Target = archBinDst
			})
		
			-- Engine/SDK libraries
			table.insert(copyTargets, { 
				Masks  = libFileMasks,
				Source = archLibSrc,
				Target = archLibDst
			})

			-- 3rd party binary dep
			table.insert(copyTargets, { 
				Masks  = binFileMasks,
				Source = arch3rdPartyBinSrc,
				Target = archBinDst
			})
		end
	end

	-- Demo executable
	table.insert(copyTargets, {
		Masks = {"Demo*" .. exeFileExt},
		Filter = exeFilterFunc,
		Source = "../examples/bin/",
		Target = "examples/bin/"
	})
		
	-- Unit test
	table.insert(copyTargets, {
		Masks = {"*" .. exeFileExt},
		Filter = exeFilterFunc,
		Source = "../tests/",
		Target = "tests/"
	})

	-- Processing
	os.mkdir(packageDir)

	local size = 0
	for k,v in pairs(copyTargets) do
		local target = packageDir .. v.Target
		local includePrefix = v.Source

		local targetFiles = {}
		for k, mask in pairs(v.Masks) do
			print(includePrefix .. mask .. " => " .. target)
			local files = os.matchfiles(includePrefix .. mask)
			if (v.Filter) then
				for k,path in pairs(files) do
					if (not v.Filter(path)) then
						files[k] = nil
					end
				end
			end

			targetFiles = table.join(targetFiles, files)
		end
		
		for k,v in pairs(targetFiles) do
			local relPath = v:sub(#includePrefix + 1)

			local targetPath = target .. relPath
			local targetDir = path.getdirectory(targetPath)
			
			if (not os.isdir(targetDir)) then
				local ok, err = os.mkdir(targetDir)
				if (not ok) then
					print("Failed to create directory \"" .. targetDir .. "\": " .. err)
				end
			end

			local ok, err
			if (os.is("windows")) then
				ok, err = os.copyfile(v, targetPath)
			else
				-- Workaround: As premake is translating this to "cp %s %s", it fails if space are presents in source/destination paths.
				ok, err = os.copyfile(string.format("\"%s\"", v), string.format("\"%s\"", targetPath))
			end

			if (not ok) then
				print("Failed to copy \"" .. v .. "\" to \"" .. targetPath .. "\": " .. err)
			end
			
			local stat = os.stat(targetPath)
			if (stat) then
				size = size + stat.size
			end
		end
	end
	
	local config = libDir .. " - " .. enabledArchs
	print(string.format("Package successfully created at \"%s\" (%u MB, %s)", packageDir, size / (1024 * 1024), config))
end
