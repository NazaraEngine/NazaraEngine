
task("update-globalheaders")

set_menu({
	-- Settings menu usage
	usage = "xmake update-globalheaders [options]",
	description = "Regenerate global headers for each module"
})

on_run(function ()
	local paths = {}

	local excludedFiles = {
		["Components.hpp"] = true,
		["ConfigCheck.hpp"] = true,
		["Debug.hpp"] = true,
		["DebugOff.hpp"] = true,
		["Systems.hpp"] = true,
		["ThreadSafety.hpp"] = true,
		["ThreadSafetyOff.hpp"] = true
	}

	local modules = os.dirs("include/Nazara/*")
	for _, modulePath in pairs(modules) do
		local moduleName = modulePath:match(".*[\\/](.*)")

		local config, err = io.open(modulePath .. "/Config.hpp", "r")
		local head = ""
		if (not config) then
			error("Failed to read config file: " .. err)
		end

		for line in config:lines() do
			if (line == "#pragma once") then -- Stop before including the #pragma once as it's already written automatically
				break
			end
			head = head .. line .. "\n"
		end

		config:close()

		paths[moduleName] = {
			Excludes = table.copy(excludedFiles),
			Header = head,
			HeaderGuard = "NAZARA_GLOBAL_" .. moduleName:upper() .. "_HPP",
			Name = "Nazara" .. moduleName,
			SearchDir = modulePath,
			Target = modulePath .. ".hpp"
		}

		-- Handle components and systems
		if (os.isdir(modulePath .. "/Components")) then
			paths[moduleName .. "_Components"] = {
				Excludes = table.copy(excludedFiles),
				Header = head,
				HeaderGuard = "NAZARA_" .. moduleName:upper() .. "_COMPONENTS_HPP",
				Name = "Nazara" .. moduleName .. " components",
				SearchDir = modulePath .. "/Components",
				Target = modulePath .. "/Components.hpp"
			}	
		end

		if (os.isdir(modulePath .. "/Systems")) then
			paths[moduleName .. "_Systems"] = {
				Excludes = table.copy(excludedFiles),
				Header = head,
				HeaderGuard = "NAZARA_" .. moduleName:upper() .. "_SYSTEMS_HPP",
				Name = "Nazara" .. moduleName .. " systems",
				SearchDir = modulePath .. "/Systems",
				Target = modulePath .. "/Systems.hpp"
			}	
		end
	end

	paths["Audio"].Excludes["OpenALFunctions.hpp"] = true
	paths["Core"].Excludes["ECS.hpp"] = true
	paths["OpenGLRenderer"].Excludes["Wrapper.hpp"] = true
	paths["VulkanRenderer"].Excludes["Wrapper.hpp"] = true

	-- OpenGL renderer wrapper
	paths["OpenGLWrapper"] = {
		Excludes = {
			["DeviceFunctions.hpp"] = true,
			["GlobalFunctions.hpp"] = true,
			["InstanceFunctions.hpp"] = true,
		},
		Header = paths["OpenGLRenderer"].Header,
		HeaderGuard = "NAZARA_OPENGLRENDERER_WRAPPER_HPP",
		Name = "OpenGL wrapper",
		SearchDir = "include/Nazara/OpenGLRenderer/Wrapper", 
		Target = "include/Nazara/OpenGLRenderer/Wrapper.hpp"
	}

	-- Vulkan renderer wrapper
	paths["VulkanWrapper"] = {
		Excludes = {
			["DeviceFunctions.hpp"] = true,
			["GlobalFunctions.hpp"] = true,
			["InstanceFunctions.hpp"] = true,
		},
		Header = paths["VulkanRenderer"].Header,
		HeaderGuard = "NAZARA_VULKANRENDERER_WRAPPER_HPP",
		Name = "Vulkan wrapper",
		SearchDir = "include/Nazara/VulkanRenderer/Wrapper", 
		Target = "include/Nazara/VulkanRenderer/Wrapper.hpp"
	}

	for _,v in pairs(paths) do
		print(v.Name)
		local files = os.files(v.SearchDir .. "/*.hpp")
		if (#files == 0) then
			print("Skipped (no file matched)")
			goto continue
		end

		local header, err = io.open(v.Target, "w+")
		if (not header) then
			error("Failed to create header file (" .. v.Target .. "): " .. err)
		end

		header:write("// this file was automatically generated and should not be edited\n\n")
		if (v.Header) then
			header:write(v.Header)
		end

		header:write("#pragma once\n\n")
		header:write("#ifndef " .. v.HeaderGuard .. "\n")
		header:write("#define " .. v.HeaderGuard .. "\n\n")

		local count = 0
		for _, filePath in pairs(files) do
			local pathParts = path.split(filePath)
			if (pathParts[1] == "include") then
				table.remove(pathParts, 1)
			end

			local include = table.concat(pathParts, "/")
			local fileName = path.filename(filePath)
			if (not v.Excludes[fileName]) then
				header:write("#include <" .. include .. ">\n")
				count = count + 1
			end
		end
		
		header:write("\n#endif // " .. v.HeaderGuard .. "\n")
		header:close()
		
		print(string.format("-#include count: %d", count))
		::continue::
	end
end)
