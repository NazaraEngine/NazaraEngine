local modules = {
	Audio = {
		Deps = {"NazaraCore"},
		Packages = {"dr_wav", "libflac", "libvorbis", "minimp3"},
		Custom = function ()
			add_packages("openal-soft", {links = {}}) -- Don't link OpenAL (it will be loaded dynamically)
		end
	},
	Core = {
		Custom = function ()
			add_headerfiles("include/(Nazara/*.hpp)")
			-- NazaraMath is header-only, make it part of the core project
			add_headerfiles("include/(Nazara/Math/**.hpp)", "include/(Nazara/Math/**.inl)")

			if is_plat("windows", "mingw") then
				add_syslinks("ole32")
			elseif is_plat("linux") then
				add_syslinks("dl", "pthread", "uuid")
			end
		end
	},
	Graphics = {
		Deps = {"NazaraRenderer"},
		Packages = {"entt"}
	},
	Network = {
		Deps = {"NazaraCore"},
		Custom = function()
			if is_plat("windows", "mingw") then 
				add_syslinks("ws2_32")
			end

			if is_plat("linux") then
				remove_files("src/Nazara/Network/Posix/SocketPollerImpl.hpp")
				remove_files("src/Nazara/Network/Posix/SocketPollerImpl.cpp")
			end
		end
	},
	OpenGLRenderer = {
		Deps = {"NazaraRenderer"},
		Custom = function()
			if is_plat("windows", "mingw") then
				add_syslinks("gdi32", "user32")
			else
				remove_files("src/Nazara/OpenGLRenderer/Wrapper/Win32/**.cpp")
				remove_files("src/Nazara/OpenGLRenderer/Wrapper/WGL/**.cpp")
			end

			if is_plat("linux") then
				add_defines("EGL_NO_X11")
			else
				remove_files("src/Nazara/OpenGLRenderer/Wrapper/Linux/**.cpp")
			end
		end
	},
	Physics2D = {
		Deps = {"NazaraUtility"},
		Packages = {"entt", "chipmunk2d"}
	},
	Physics3D = {
		Deps = {"NazaraUtility"},
		Packages = {"entt", "newtondynamics"}
	},
	Platform = {
		Deps = {"NazaraUtility"},
		Packages = {"libsdl"},
		Custom = function()
			if is_plat("windows", "mingw") then
				add_defines("SDL_VIDEO_DRIVER_WINDOWS=1")
			elseif is_plat("linux") then
				add_defines("SDL_VIDEO_DRIVER_X11=1")
				add_defines("SDL_VIDEO_DRIVER_WAYLAND=1")
			elseif is_plat("macosx") then
				add_defines("SDL_VIDEO_DRIVER_COCOA=1")
				add_packages("libx11", { links = {} }) -- we only need X11 headers
			end
		end
	},
	Renderer = {
		Deps = {"NazaraPlatform", "NazaraShader"}
	},
	Shader = {
		Deps = {"NazaraUtility"},
		Packages = {"efsw", "fmt"},
		Custom = function()
			-- Set precise floating-points models to ensure shader optimization leads to correct results
			set_fpmodels("precise")
		end
	},
	Utility = {
		Deps = {"NazaraCore"},
		Packages = {"entt", "freetype", "stb"}
	},
	VulkanRenderer = {
		Deps = {"NazaraRenderer"},
		Custom = function()
			add_defines("VK_NO_PROTOTYPES")
			if is_plat("windows", "mingw") then
				add_defines("VK_USE_PLATFORM_WIN32_KHR")
				add_syslinks("user32")
			elseif is_plat("linux") then
				add_defines("VK_USE_PLATFORM_XLIB_KHR")
				add_defines("VK_USE_PLATFORM_WAYLAND_KHR")
			elseif is_plat("macosx") then
				add_defines("VK_USE_PLATFORM_METAL_EXT")
			end
		end
	},
	Widgets = {
		Deps = {"NazaraGraphics"},
		Packages = {"entt", "kiwisolver"}
	}
}

NazaraModules = modules

includes("xmake/**.lua")

option("usepch")
	set_default(false)
	set_showmenu(true)
	set_description("Use precompiled headers to speedup compilation")
option_end()

option("unitybuild")
	set_default(false)
	set_showmenu(true)
	set_description("Build the engine using unity build")
option_end()

set_project("NazaraEngine")
set_xmakever("2.6.3")

add_requires("chipmunk2d", "dr_wav", "efsw", "entt >=3.9", "fmt", "kiwisolver", "libflac", "libsdl", "minimp3", "stb")
add_requires("freetype", { configs = { bzip2 = true, png = true, woff2 = true, zlib = true, debug = is_mode("debug") } })
add_requires("libvorbis", { configs = { with_vorbisenc = false } })
add_requires("openal-soft", { configs = { shared = true }})
add_requires("newtondynamics", { debug = is_plat("windows") and is_mode("debug") }) -- Newton doesn't like compiling in Debug on Linux

if is_plat("macosx") then
	add_requires("libx11")
end

add_rules("mode.asan", "mode.coverage", "mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")
add_rules("build_rendererplugins")

set_allowedplats("windows", "mingw", "linux", "macosx")
set_allowedarchs("windows|x64", "mingw|x86_64", "linux|x86_64", "macosx|x86_64")
set_allowedmodes("debug", "releasedbg", "asan", "coverage", "fuzz")
set_defaultmode("debug")

if is_mode("debug") then
	add_rules("debug_suffix")
elseif is_mode("asan") then
	set_optimize("none") -- by default xmake will optimize asan builds
elseif is_mode("coverage") then
	if not is_plat("windows") then
		add_links("gcov")
	end
elseif is_mode("releasedbg") then
	set_fpmodels("fast")
	add_vectorexts("sse", "sse2", "sse3", "ssse3")
end

add_includedirs("include")
add_sysincludedirs("thirdparty/include")

set_languages("c89", "cxx17")
set_rundir("./bin/$(plat)_$(arch)_$(mode)")
set_symbols("debug", "hidden")
set_targetdir("./bin/$(plat)_$(arch)_$(mode)")
set_warnings("allextra")

if is_mode("debug") then
	add_defines("NAZARA_DEBUG")
end


if is_plat("windows") then
	set_runtimes(is_mode("debug") and "MDd" or "MD")

	add_defines("_CRT_SECURE_NO_WARNINGS")
	add_cxxflags("/bigobj", "/permissive-", "/Zc:__cplusplus", "/Zc:externConstexpr", "/Zc:inline", "/Zc:lambda", "/Zc:preprocessor", "/Zc:referenceBinding", "/Zc:strictStrings", "/Zc:throwingNew")
	add_cxflags("/w44062") -- Enable warning: switch case not handled
	add_cxflags("/wd4251") -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
elseif is_plat("mingw") then
	add_cxflags("-Og", "-Wa,-mbig-obj")
	add_ldflags("-Wa,-mbig-obj")
end

for name, module in pairs(modules) do
	target("Nazara" .. name)

	set_kind("shared")
	set_group("Modules")

	add_rules("embed_resources")
	add_rpathdirs("$ORIGIN")

	if module.Deps then
		add_deps(table.unpack(module.Deps))
	end

	if module.Packages then
		add_packages(table.unpack(module.Packages))
	end

	if module.Custom then
		module.Custom()
	end

	if has_config("usepch") then
		set_pcxxheader("include/Nazara/" .. name .. ".hpp")
	end

	if has_config("unitybuild") then
		add_defines("NAZARA_UNITY_BUILD")
		add_rules("c++.unity_build", {uniqueid = "NAZARA_UNITY_ID", batchsize = 12})
	end

	add_defines("NAZARA_BUILD")
	add_defines("NAZARA_" .. name:upper() .. "_BUILD")

	if is_mode("debug") then
		add_defines("NAZARA_" .. name:upper() .. "_DEBUG")
	end

	-- Add header and source files
	local headerExts = {".h", ".hpp", ".inl", ".natvis"}
	for _, ext in ipairs(headerExts) do
		add_headerfiles("include/(Nazara/" .. name .. "/**" .. ext .. ")")
		add_headerfiles("src/Nazara/" .. name .. "/**" .. ext)
	end

	add_files("src/Nazara/" .. name .. "/**.cpp")
	add_includedirs("src")

	for _, filepath in pairs(os.files("src/Nazara/" .. name .. "/Resources/**|**.h")) do
		add_files(filepath, {rule = "embed_resources"})
	end

	-- Remove platform-specific files
	if is_plat("windows", "mingw") then
		for _, ext in ipairs(headerExts) do
			remove_headerfiles("src/Nazara/" .. name .. "/Posix/**" .. ext)
		end

		remove_files("src/Nazara/" .. name .. "/Posix/**.cpp")
	else
		for _, ext in ipairs(headerExts) do
			remove_headerfiles("src/Nazara/" .. name .. "/Posix/**" .. ext)
		end

		remove_files("src/Nazara/" .. name .. "/Win32/**.cpp")
	end

	if not is_plat("linux") then
		for _, ext in ipairs(headerExts) do
			remove_headerfiles("src/Nazara/" .. name .. "/Linux/**" .. ext)
		end

		remove_files("src/Nazara/" .. name .. "/Linux/**.cpp")
	end
end

includes("tools/xmake.lua")
includes("tests/xmake.lua")
includes("plugins/*/xmake.lua")
includes("examples/*/xmake.lua")
