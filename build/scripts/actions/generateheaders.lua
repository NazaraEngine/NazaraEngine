ACTION.Name = "GenerateHeaders"
ACTION.Description = "Generate a global header for each module"

ACTION.ModuleExcludes = {}
ACTION.ModuleExcludes["ConfigCheck.hpp"] = true
ACTION.ModuleExcludes["Debug.hpp"] = true
ACTION.ModuleExcludes["DebugOff.hpp"] = true
ACTION.ModuleExcludes["ThreadSafety.hpp"] = true
ACTION.ModuleExcludes["ThreadSafetyOff.hpp"] = true

local action = ACTION
ACTION.Function = function ()
	local paths = {}

	local modules = os.matchdirs("../include/Nazara/*")
	for k, modulePath in pairs(modules) do
		local moduleName = modulePath:match(".*/(.*)")

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
			Excludes = action.ModuleExcludes,
			Header = head,
			HeaderGuard = "NAZARA_GLOBAL_" .. moduleName:upper() .. "_HPP",
			Name = "Nazara" .. moduleName,
			SearchDir = modulePath,
			Target = modulePath .. ".hpp",
			TopDir = "Nazara"
		})
	end
	
	table.insert(paths, {
		Excludes = {},
		HeaderGuard = "NDK_COMPONENTS_GLOBAL_HPP",
		Name = "NDK Components",
		SearchDir = "../SDK/include/NDK/Components", 
		TopDir = "NDK", 
		Target = "../SDK/include/NDK/Components.hpp"
	})
	
	table.insert(paths, {
		Excludes = {},
		HeaderGuard = "NDK_SYSTEMS_GLOBAL_HPP",
		Name = "NDK Systems",
		SearchDir = "../SDK/include/NDK/Systems", 
		TopDir = "NDK", 
		Target = "../SDK/include/NDK/Systems.hpp"
	})
	
	table.insert(paths, {
		Excludes = {},
		HeaderGuard = "NDK_WIDGETS_GLOBAL_HPP",
		Name = "NDK Widgets",
		SearchDir = "../SDK/include/NDK/Widgets", 
		TopDir = "NDK", 
		Target = "../SDK/include/NDK/Widgets.hpp"
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

		local files = os.matchfiles(v.SearchDir .. "/*.hpp")
		local count = 0
		for k, filePath in pairs(files) do
			local include, fileName = filePath:match(".*(" .. v.TopDir .. "/.*/(.*))")
			if (not v.Excludes[fileName]) then
				header:write("#include <" .. include .. ">\n")
				count = count + 1
			end
		end
		
		header:write("\n#endif // " .. v.HeaderGuard .. "\n")
		header:close()
		
		print(string.format("-#include count: %d", count))
	end
end
