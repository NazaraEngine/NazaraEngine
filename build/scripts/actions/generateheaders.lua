function generateHeaders()
	local modules = os.matchdirs("../include/Nazara/*")
	for k, modulePath in pairs(modules) do
		local moduleName = modulePath:match(".*/(.*)")
		print(moduleName)

		local config, err = io.open(modulePath .. "/Config.hpp", "r")
		local head = ""
		if (not config) then
			error("Failed to read config file: " .. err)
		end

		for line in config:lines() do
			head = head .. line .. "\n"
			if (line == "#pragma once") then -- On s'arrête au #pragma once, qu'on inclut quand même
				break
			end
		end

		config:close()

		local header, err = io.open(modulePath .. ".hpp", "w+")
		if (not header) then
			error("Failed to create header file: " .. err)
		end

		header:write("// This file was automatically generated on " .. os.date("%d %b %Y at %X") .. "\n\n")
		header:write(head .. "\n")

		-- Protection du multi-including
		local preprocessorName = "NAZARA_GLOBAL_" .. string.upper(moduleName) .. "_HPP"
		header:write("#ifndef " .. preprocessorName .. "\n")
		header:write("#define " .. preprocessorName .. "\n\n")

		local files = os.matchfiles(modulePath .. "/*.hpp")
		local count = 0
		for k, filePath in pairs(files) do
			local include, fileName = filePath:match(".*(Nazara/.*/(.*))")
			if (fileName ~= "ConfigCheck.hpp" and
			    fileName ~= "Debug.hpp" and
			    fileName ~= "DebugOff.hpp" and
			    fileName ~= "ThreadSafety.hpp" and
			    fileName ~= "ThreadSafetyOff.hpp") then
				header:write("#include <" .. include .. ">\n")
				count = count + 1
			end
		end

		header:write("\n#endif // " .. preprocessorName .. "\n")
		header:close()

		print(string.format("-#include count: %d", count))
	end
end

newaction
{
	trigger     = "generateheaders",
	description = "Generate a global header for each module",
	execute     = generateHeaders
}
