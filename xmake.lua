local rendererBackends = {
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
				add_packages("wayland", { links = {} }) -- we only need wayland headers
			else
				remove_files("src/Nazara/OpenGLRenderer/Wrapper/Linux/**.cpp")
			end
		end
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
				add_packages("libxext", "wayland", { links = {} }) -- we only need X11 and waylands headers
			elseif is_plat("macosx") then
				add_defines("VK_USE_PLATFORM_METAL_EXT")
				add_files("src/Nazara/VulkanRenderer/**.mm")
				add_frameworks("quartzcore", "AppKit")
			end
		end
	}
}
NazaraRendererBackends = rendererBackends

local modules = {
	Audio = {
		Deps = {"NazaraCore"},
		Packages = {"dr_wav", "frozen", "libflac", "libvorbis", "minimp3"},
		Custom = function ()
			if is_plat("wasm") or has_config("link_openal") then
				add_defines("NAZARA_AUDIO_OPENAL_LINK")
				if is_plat("wasm") then
					add_syslinks("openal")
				else
					add_defines("AL_ALEXT_PROTOTYPES")
					add_packages("openal-soft")
				end
			else
				add_packages("openal-soft", { links = {} })
			end
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
				add_packages("libuuid")
				add_syslinks("dl", "pthread")
			end
		end,
		Packages = { "entt", "frozen" },
		PublicPackages = { "nazarautils" }
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
	Physics2D = {
		Deps = {"NazaraUtility"},
		Packages = {"entt", "chipmunk2d"}
	},
	Physics3D = {
		Deps = {"NazaraUtility"},
		Packages = {"entt", "newtondynamics3"}
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
				add_packages("libxext", "wayland", { links = {} }) -- we only need X11 headers
			elseif is_plat("macosx") then
				add_defines("SDL_VIDEO_DRIVER_COCOA=1")
			end
		end
	},
	Renderer = {
		Deps = {"NazaraPlatform"},
		PublicPackages = { "nazarautils", "nzsl" },
		Custom = function ()
			if has_config("embed_rendererbackends") then
				-- Embed backends code inside our own modules
				add_defines("NAZARA_RENDERER_EMBEDDEDBACKENDS")
				for name, module in pairs(rendererBackends) do
					ModuleTargetConfig(name, module)
				end
			end
		end
	},
	Utility = {
		Deps = {"NazaraCore"},
		Packages = {"entt", "freetype", "frozen", "ordered_map", "stb"}
	},
	Widgets = {
		Deps = {"NazaraGraphics"},
		Packages = {"entt", "kiwisolver"}
	}
}

if not has_config("embed_rendererbackends") then
	-- Register backends as separate modules
	for name, module in pairs(rendererBackends) do
		if (modules[name] ~= nil) then
			os.raise("overriding module " .. name)
		end

		modules[name] = module
	end
end

NazaraModules = modules

includes("xmake/**.lua")

option("compile_shaders", { description = "Compile nzsl shaders into an includable binary version", default = true })
option("embed_rendererbackends", { description = "Embed renderer backend code into NazaraRenderer instead of loading them dynamically", default = false })
option("embed_resources", { description = "Turn builtin resources into includable headers", default = true })
option("link_openal", { description = "Link OpenAL in the executable instead of dynamically loading it", default = false })
option("override_runtime", { description = "Override vs runtime to MD in release and MDd in debug", default = true })
option("usepch", { description = "Use precompiled headers to speedup compilation", default = false })
option("unitybuild", { description = "Build the engine using unity build", default = false })

set_project("NazaraEngine")
set_xmakever("2.7.3")

add_requires("chipmunk2d", "dr_wav", "efsw", "entt 3.10.1", "fmt", "frozen", "kiwisolver", "libflac", "libsdl", "minimp3", "ordered_map", "stb")
add_requires("freetype", { configs = { bzip2 = true, png = true, woff2 = true, zlib = true, debug = is_mode("debug") } })
add_requires("libvorbis", { configs = { with_vorbisenc = false } })
add_requires("openal-soft", { configs = { shared = true }})
add_requires("newtondynamics3", { debug = is_plat("windows") and is_mode("debug") }) -- Newton doesn't like compiling in Debug on Linux

add_repositories("nazara-engine-repo https://github.com/NazaraEngine/xmake-repo")
add_requires("nazarautils")
add_requires("nzsl", { debug = is_mode("debug"), configs = { with_symbols = not is_mode("release"), shared = true } })

if is_plat("linux") then
	add_requires("libxext", "libuuid", "wayland")
end

add_rules("mode.asan", "mode.tsan", "mode.coverage", "mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("build.rendererplugins")

if has_config("examples") then
	add_rules("download.assets.examples")
end

if has_config("tests") then
	add_rules("download.assets.tests")
end

set_allowedplats("windows", "mingw", "linux", "macosx")
set_allowedmodes("debug", "releasedbg", "release", "asan", "tsan", "coverage")
set_defaultmode("debug")

if is_mode("debug") then
	add_rules("debug.suffix")
elseif is_mode("asan") then
	set_optimize("none") -- by default xmake will optimize asan builds
elseif is_mode("tsan") then
	set_optimize("none") -- by default xmake will optimize asan builds
elseif is_mode("coverage") then
	if not is_plat("windows") then
		add_links("gcov")
	end
elseif is_mode("releasedbg", "release") then
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
	if has_config("override_runtime") then
		set_runtimes(is_mode("debug") and "MDd" or "MD")
	end

	add_defines("_CRT_SECURE_NO_WARNINGS")
	add_cxxflags("/bigobj", "/permissive-", "/Zc:__cplusplus", "/Zc:externConstexpr", "/Zc:inline", "/Zc:lambda", "/Zc:preprocessor", "/Zc:referenceBinding", "/Zc:strictStrings", "/Zc:throwingNew")
	add_cxflags("/w44062") -- Enable warning: switch case not handled
	add_cxflags("/wd4251") -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
	add_cxflags("/wd4275") -- Disable warning: DLL-interface class 'class_1' used as base for DLL-interface blah
elseif is_plat("mingw") then
	add_cxflags("-Og", "-Wa,-mbig-obj")
	add_ldflags("-Wa,-mbig-obj")
end

function ModuleTargetConfig(name, module)
	add_defines("NAZARA_" .. name:upper() .. "_BUILD")
	if is_mode("debug") then
		add_defines("NAZARA_" .. name:upper() .. "_DEBUG")
	end

	-- Add header and source files
	local headerExts = {".h", ".hpp", ".inl", ".natvis"}
	for _, ext in ipairs(headerExts) do
		add_headerfiles("include/(Nazara/" .. name .. "/**" .. ext .. ")")
		add_headerfiles("src/Nazara/" .. name .. "/**" .. ext, { prefixdir = "private", install = false })
		add_headerfiles("src/Nazara/" .. name .. "/Resources/**.nzsl", { prefixdir = "private", install = false })
	end

	remove_headerfiles("src/Nazara/" .. name .. "/Resources/**.h")

	add_files("src/Nazara/" .. name .. "/**.cpp")
	if has_config("embed_resources") then
		local embedResourceRule = false
		for _, filepath in pairs(os.files("src/Nazara/" .. name .. "/Resources/**|**.h|**.nzsl|**.nzslb")) do
			if not embedResourceRule then
				add_rules("embed.resources")
				embedResourceRule = true
			end

			add_files(filepath, {rule = "embed.resources"})
		end
	end

	if has_config("compile_shaders") then
		local compileShaderRule = false
		for _, filepath in pairs(os.files("src/Nazara/" .. name .. "/Resources/**.nzsl")) do
			if not compileShaderRule then
				add_rules("nzsl.compile.shaders")
				compileShaderRule = true
			end

			add_files(filepath, {rule = "nzsl.compile.shaders"})
		end
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

	if module.Custom then
		module.Custom()
	end
end

for name, module in pairs(modules) do
	target("Nazara" .. name, function ()
		set_kind("shared")
		set_group("Modules")

		add_rpathdirs("$ORIGIN")

		if module.Deps then
			add_deps(table.unpack(module.Deps))
		end

		if module.Packages then
			add_packages(table.unpack(module.Packages))
		end

		if module.PublicPackages then
			for _, pkg in ipairs(module.PublicPackages) do
				add_packages(pkg, { public = true })
			end
		end

		if has_config("usepch") then
			set_pcxxheader("include/Nazara/" .. name .. ".hpp")
		end

		if has_config("unitybuild") then
			add_defines("NAZARA_UNITY_BUILD")
			add_rules("c++.unity_build", {uniqueid = "NAZARA_UNITY_ID", batchsize = 12})
		end

		add_defines("NAZARA_BUILD")
		add_defines("NAZARA_UTILS_WINDOWS_NT6=1")

		add_includedirs("src")

		ModuleTargetConfig(name, module)
	end)
end

includes("tools/*.lua")
includes("tests/*.lua")
includes("examples/*.lua")
includes("plugins/*/xmake.lua")
