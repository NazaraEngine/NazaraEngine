
task("update-globalheaders")

set_menu({
	-- Settings menu usage
	usage = "xmake update-globalheaders [options]",
	description = "Regenerate global headers for each module"
})

on_run(function ()
	local paths = {}

	local excludedFiles = {
		["ConfigCheck.hpp"] = true,
		["Debug.hpp"] = true,
		["DebugOff.hpp"] = true,
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

		table.insert(paths, {
			Excludes = excludedFiles,
			Header = head,
			HeaderGuard = "NAZARA_GLOBAL_" .. moduleName:upper() .. "_HPP",
			Name = "Nazara" .. moduleName,
			SearchDir = modulePath,
			Target = modulePath .. ".hpp"
		})
	end

	table.insert(paths, {
		Excludes = {
			["DeviceFunctions.hpp"] = true,
			["GlobalFunctions.hpp"] = true,
			["InstanceFunctions.hpp"] = true,
		},
		HeaderGuard = "NAZARA_GLOBAL_OPENGLRENDERER_WRAPPER_HPP",
		Name = "OpenGL wrapper",
		SearchDir = "include/Nazara/OpenGLRenderer/Wrapper", 
		Target = "include/Nazara/OpenGLRenderer/Wrapper.hpp"
	})

	table.insert(paths, {
		Excludes = {
			["DeviceFunctions.hpp"] = true,
			["GlobalFunctions.hpp"] = true,
			["InstanceFunctions.hpp"] = true,
		},
		HeaderGuard = "NAZARA_GLOBAL_VULKANRENDERER_WRAPPER_HPP",
		Name = "Vulkan wrapper",
		SearchDir = "include/Nazara/VulkanRenderer/Wrapper", 
		Target = "include/Nazara/VulkanRenderer/Wrapper.hpp"
	})

	for k,v in ipairs(paths) do
		print(v.Name)
		local header, err = io.open(v.Target, "w+")
		if (not header) then
			error("Failed to create header file (" .. v.Target .. "): " .. err)
		end

		header:write("// This file was automatically generated\n\n")
		if (v.Header) then
			header:write(v.Header)
		end

		header:write("#pragma once\n\n")
		header:write("#ifndef " .. v.HeaderGuard .. "\n")
		header:write("#define " .. v.HeaderGuard .. "\n\n")

		local files = os.files(v.SearchDir .. "/*.hpp")
		local count = 0
		for _, filePath in pairs(files) do
			local pathParts = path.split(filePath)
			if (pathParts[1] == "include") then
				table.remove(pathParts, 1)
			end

			local include = table.concat(pathParts, "/")
			print(include)
			local fileName = path.filename(filePath)
			if (not v.Excludes[fileName]) then
				header:write("#include <" .. include .. ">\n")
				count = count + 1
			end
		end
		
		header:write("\n#endif // " .. v.HeaderGuard .. "\n")
		header:close()
		
		print(string.format("-#include count: %d", count))
	end
end)
