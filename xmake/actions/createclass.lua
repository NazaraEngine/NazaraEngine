
local modules = NazaraModules

-- defined at the end of the file
local headerTemplate, inlineTemplate, sourceTemplate

task("create-class")

set_menu({
	-- Settings menu usage
	usage = "xmake create-class [options] name",
	description = "Helper for class creation",
	options =
	{
		-- Set k mode as key-only bool parameter
		{nil, "nocpp", "k", nil, "Set this if you don't want a .cpp to be created (header-only classes)" },
		{'m', "module", "v", nil, "Module where the class should be created" },
		{nil, "name", "v", nil, "Class name" }
	}
})

on_run(function ()
	import("core.base.option")

	local moduleName = option.get("module")
	if not moduleName then
		os.raise("missing module name")
	end

	local classPath = option.get("name")
	if not classPath then
		os.raise("missing class name")
	end
	
	local module = modules[moduleName]
	if not module then
		os.raise("unknown module " .. moduleName)
	end

	local className = path.basename(classPath)

	local files = {
		{ TargetPath = path.join("include", "Nazara", moduleName, classPath) .. ".hpp", Template = headerTemplate },
		{ TargetPath = path.join("include", "Nazara", moduleName, classPath) .. ".inl", Template = inlineTemplate }
	}

	if not option.get("nocpp") then
		table.insert(files, { TargetPath = path.join("src", "Nazara", moduleName, classPath) .. ".cpp", Template = sourceTemplate })
	end

	local replacements = {
		CLASS_NAME = className,
		CLASS_PATH = classPath,
		COPYRIGHT = os.date("%Y") .. [[ Jérôme "SirLynix" Leclercq (lynix680@gmail.com)]],
		HEADER_GUARD = "NAZARA_" .. moduleName:upper() .. "_" .. classPath:gsub("[/\\]", "_"):upper() .. "_HPP",
		MODULE_API = "NAZARA_" .. moduleName:upper() .. "_API",
		MODULE_NAME = moduleName,
	}

	for _, file in pairs(files) do
		local content = file.Template:gsub("%%([%u_]+)%%", function (kw)
			local r = replacements[kw]
			if not r then
				os.raise("missing replacement for " .. kw)
			end

			return r
		end)

		io.writefile(file.TargetPath, content)
	end
end)

headerTemplate = [[
// Copyright (C) %COPYRIGHT%
// This file is part of the "Nazara Engine - %MODULE_NAME% module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef %HEADER_GUARD%
#define %HEADER_GUARD%

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/%MODULE_NAME%/Export.hpp>

namespace Nz
{
	class %MODULE_API% %CLASS_NAME%
	{
		public:
			%CLASS_NAME%() = default;
			%CLASS_NAME%(const %CLASS_NAME%&) = delete;
			%CLASS_NAME%(%CLASS_NAME%&&) = delete;
			~%CLASS_NAME%() = default;

			%CLASS_NAME%& operator=(const %CLASS_NAME%&) = delete;
			%CLASS_NAME%& operator=(%CLASS_NAME%&&) = delete;

		private:
	};
}

#include <Nazara/%MODULE_NAME%/%CLASS_PATH%.inl>

#endif // %HEADER_GUARD%
]]

inlineTemplate = [[
// Copyright (C) %COPYRIGHT%
// This file is part of the "Nazara Engine - %MODULE_NAME% module"
// For conditions of distribution and use, see copyright notice in Config.hpp


namespace Nz
{
}

]]

sourceTemplate = [[
// Copyright (C) %COPYRIGHT%
// This file is part of the "Nazara Engine - %MODULE_NAME% module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/%MODULE_NAME%/%CLASS_PATH%.hpp>

namespace Nz
{
}
]]
