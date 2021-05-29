local modules = {
	Audio = {
		Deps = {"NazaraCore"},
		Packages = {"libsndfile"}
	},
	Core = {},
	Graphics = {
		Deps = {"NazaraRenderer"}
	},
	Network = {
		Deps = {"NazaraCore"},
		Custom = function()
			if is_plat("windows") then 
				add_syslinks("ws2_32")
			end

			if is_plat("linux") then
				del_files("src/Nazara/Network/Posix/SocketPollerImpl.hpp")
				del_files("src/Nazara/Network/Posix/SocketPollerImpl.cpp")
			end
		end
	},
	OpenGLRenderer = {
		Deps = {"NazaraRenderer"},
		Custom = function()
			if is_plat("windows") then
				add_syslinks("gdi32", "user32")
			else
				del_files("src/Nazara/OpenGLRenderer/Wrapper/Win32/**.cpp")
				del_files("src/Nazara/OpenGLRenderer/Wrapper/WGL/**.cpp")
			end

			if not is_plat("linux") then
				del_files("src/Nazara/OpenGLRenderer/Wrapper/Linux/**.cpp")
			end
		end
	},
	Physics2D = {
		Deps = {"NazaraUtility"},
		Packages = {"chipmunk2d"}
	},
	Physics3D = {
		Deps = {"NazaraUtility"},
		Packages = {"newtondynamics"}
	},
	Platform = {
		Deps = {"NazaraUtility"},
		Packages = {"libsdl"},
		Custom = function()
			if is_plat("windows") then
				add_defines("SDL_VIDEO_DRIVER_WINDOWS=1")
			elseif is_plat("linux") then
				add_defines("SDL_VIDEO_DRIVER_X11=1")
				add_defines("SDL_VIDEO_DRIVER_WAYLAND=1")
			elseif is_plat("macosx") then
				add_defines("SDL_VIDEO_DRIVER_COCOA=1")
			end
		end
	},
	Renderer = {
		Deps = {"NazaraPlatform", "NazaraShader"}
	},
	Shader = {
		Deps = {"NazaraUtility"}
	},
	Utility = {
		Deps = {"NazaraCore"},
		Packages = {"freetype", "stb"}
	},
	VulkanRenderer = {
		Deps = {"NazaraRenderer"},
		Custom = function()
			add_defines("VK_NO_PROTOTYPES")
			if is_plat("windows") then
				add_defines("VK_USE_PLATFORM_WIN32_KHR")
				add_syslinks("user32")
			elseif is_plat("linux") then
				add_defines("VK_USE_PLATFORM_XLIB_KHR")
				add_defines("VK_USE_PLATFORM_WAYLAND_KHR")
			elseif is_plat("macosx") then
				add_defines("VK_USE_PLATFORM_MACOS_MVK")
			end
		end
	}
}

add_repositories("local-repo xmake-repo")

add_requires("chipmunk2d", "freetype", "libsndfile", "libsdl", "stb")
add_requires("newtondynamics", { debug = is_plat("windows") and is_mode("debug") }) -- Newton doesn't like compiling in Debug on Linux

set_project("NazaraEngine")

add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")
add_rules("build_rendererplugins")

if is_mode("debug") then
	add_rules("debug_suffix")
end

add_includedirs("include")
add_sysincludedirs("thirdparty/include")

set_languages("c89", "cxx17")
set_rundir("./bin/$(os)_$(arch)_$(mode)")
set_symbols("debug", "hidden")
set_targetdir("./bin/$(os)_$(arch)_$(mode)")
set_warnings("allextra")

if is_mode("releasedbg") then
	set_fpmodels("fast")
	add_vectorexts("sse", "sse2", "sse3", "ssse3")
end

if is_plat("windows") then
	set_runtimes(is_mode("debug") and "MDd" or "MD")

	add_defines("_CRT_SECURE_NO_WARNINGS")
	add_cxxflags("/bigobj", "/permissive-", "/Zc:__cplusplus", "/Zc:referenceBinding", "/Zc:throwingNew")
	add_cxxflags("/FC")
	add_cxflags("/w44062") -- Enable warning: switch case not handled
	add_cxflags("/wd4251") -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
end

for name, module in pairs(modules) do
	target("Nazara" .. name)

	set_kind("shared")
	set_group("Modules")

	add_rules("embed_resources")

	if module.Deps then
		add_deps(module.Deps)
	end

	if module.Packages then
		add_packages(module.Packages)
	end

	add_defines("NAZARA_BUILD")
	add_defines("NAZARA_" .. name:upper() .. "_BUILD")

	if is_mode("debug") then
		add_defines("NAZARA_DEBUG")
		add_defines("NAZARA_" .. name:upper() .. "_DEBUG")
	end

	add_headerfiles("include/Nazara/" .. name .. "/**.hpp")
	add_headerfiles("include/Nazara/" .. name .. "/**.inl")
	add_headerfiles("src/Nazara/" .. name .. "/**.hpp")
	add_headerfiles("src/Nazara/" .. name .. "/**.inl")
	add_files("src/Nazara/" .. name .. "/**.cpp")
	add_includedirs("src")

	for _, filepath in pairs(os.files("src/Nazara/" .. name .. "/Resources/**|**.h")) do
		add_files(filepath, {rule="embed_resources"})
	end

	if is_plat("windows") then
		del_files("src/Nazara/" .. name .. "/Posix/**.cpp")
	else
		del_files("src/Nazara/" .. name .. "/Win32/**.cpp")
	end

	if not is_plat("linux") then
		del_files("src/Nazara/" .. name .. "/Linux/**.cpp")
	end

	if module.Custom then
		module.Custom()
	end
end

includes("xmake/actions/*.lua")
includes("tools/xmake.lua")
includes("tests/xmake.lua")
includes("plugins/*/xmake.lua")
includes("examples/*/xmake.lua")

-- Adds -d as a debug suffix
rule("debug_suffix")
	on_load(function (target)
		if target:kind() ~= "binary" then
			target:set("basename", target:basename() .. "-d")
		end
	end)

-- Builds renderer plugins if linked to NazaraRenderer
rule("build_rendererplugins")
	after_load(function (target)
		if target:kind() == "binary" and target:dep("NazaraRenderer") then
			for name, _ in pairs(modules) do
				local depName = "Nazara" .. name
				if name:match("^.+Renderer$") and target:dep(depName) == nil then -- don't overwrite dependency
					target:add("deps", depName, {inherit = false})
				end
			end
		end
	end)

-- Turns resources into includables headers
rule("embed_resources")
	before_build(function (target, opt)
		import("core.base.option")
		import("private.utils.progress")

		local function GenerateEmbedHeader(filepath, targetpath)
			local bufferSize = 1024 * 1024

			progress.show(opt.progress, "${color.build.object}embedding %s", filepath)

			local resource = assert(io.open(filepath, "rb"))
			local targetFile = assert(io.open(targetpath, "w+"))

			local resourceSize = resource:size()

			local remainingSize = resourceSize
			local headerSize = 0

			while remainingSize > 0 do
				local readSize = math.min(remainingSize, bufferSize)
				local data = resource:read(readSize)
				remainingSize = remainingSize - readSize

				local headerContentTable = {}
				for i = 1, #data do
					table.insert(headerContentTable, string.format("%d,", data:byte(i)))
				end
				local content = table.concat(headerContentTable)

				headerSize = headerSize + #content

				targetFile:write(content)
			end

			resource:close()
			targetFile:close()
		end

		for _, sourcebatch in pairs(target:sourcebatches()) do
			if sourcebatch.rulename == "embed_resources" then
				for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
					local targetpath = sourcefile .. ".h"
					if option.get("rebuild") or os.mtime(sourcefile) >= os.mtime(targetpath) then
						GenerateEmbedHeader(sourcefile, targetpath)
					end
				end
			end
		end
	end)
