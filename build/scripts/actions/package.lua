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
			libDir = libDirs[1]
			print("No directory was set by the --pack-libdir command, \"" .. libDir .. "\" will be used")
		end
	else
		libDir = "../lib/" .. libDir .. "/"
	end

	if (not os.isdir(libDir)) then
		error(string.format("\"%s\" doesn't seem to be an existing directory", libDir))
	end

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
		}
	}

	if (os.is("windows")) then
		-- Engine/SDK binaries
		table.insert(copyTargets, { 
			Masks = {"**.dll"},
			Source = libDir,
			Target = "bin/"
		})
	
		-- Engine/SDK libraries
		table.insert(copyTargets, { 
			Masks = {"**.lib"},
			Source = libDir,
			Target = "lib/"
		})

		-- 3rd party binary dep
		table.insert(copyTargets, { 
			Masks = {"**.dll"},
			Source = "../extlibs/lib/common/",
			Target = "bin/"
		})

		-- Demo executable (Windows)
		table.insert(copyTargets, {
			Masks = {"Demo*.exe"},
			Source = "../examples/bin",
			Target = "examples/bin"
		})
	elseif (os.is("macosx")) then
		-- Engine/SDK binaries
		table.insert(copyTargets, { 
			Masks = {"**.dynlib"},
			Source = libDir,
			Target = "bin/"
		})
	
		-- Engine/SDK libraries
		table.insert(copyTargets, { 
			Masks = {"**.a"},
			Source = libDir,
			Target = "lib/"
		})

		-- 3rd party binary dep
		table.insert(copyTargets, { 
			Masks = {"**.dynlib"},
			Source = "../extlibs/lib/common/",
			Target = "bin/"
		})

		-- Demo executable (Windows)
		table.insert(copyTargets, {
			Masks = {"Demo*"},
			Filter = function (path) return path.getextension(path) == "" end,
			Source = "../examples/bin",
			Target = "examples/bin"
		})
	else
		-- Engine/SDK binaries
		table.insert(copyTargets, { 
			Masks = {"**.so"},
			Source = libDir,
			Target = "bin/"
		})
	
		-- Engine/SDK libraries
		table.insert(copyTargets, { 
			Masks = {"**.a"},
			Source = libDir,
			Target = "lib/"
		})

		-- 3rd party binary dep
		table.insert(copyTargets, { 
			Masks = {"**.so"},
			Source = "../extlibs/lib/common/",
			Target = "bin/"
		})

		-- Demo executable (Windows)
		table.insert(copyTargets, {
			Masks = {"Demo*"},
			Filter = function (path) return path.getextension(path) == "" end,
			Source = "../examples/bin",
			Target = "examples/bin"
		})
	end


	-- Processing
	os.mkdir(packageDir)

	for k,v in pairs(copyTargets) do
		local target = packageDir .. v.Target
		local includePrefix = v.Source
		for k,v in pairs(os.matchdirs(includePrefix .. "**")) do
			local relPath = v:sub(#includePrefix + 1)

			local targetPath = target .. relPath
			
			if (not os.isdir(targetPath)) then
				local ok, err = os.mkdir(targetPath)
				if (not ok) then
					print("Failed to create directory \"" .. targetPath .. "\": " .. err)
				end
			end
		end

		local targetFiles = {}
		for k, mask in pairs(v.Masks) do
			local files = os.matchfiles(includePrefix .. mask)
			if (v.Filter) then
				for k,path in pairs(files) do
					if (not v.Filter(v)) then
						files[k] = nil
					end
				end
			end
		
			targetFiles = table.join(targetFiles, files)
		end
		
		for k,v in pairs(targetFiles) do
			local relPath = v:sub(#includePrefix + 1)

			local targetPath = target .. relPath
			
			local ok, err = os.copyfile(v, targetPath)
			if (not ok) then
				print("Failed to copy \"" .. v .. "\" to \"" .. targetPath .. "\": " .. err)
			end
		end
	end
	
	print(string.format("Package successfully created at \"%s\"", packageDir))
end
