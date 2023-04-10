----------------------- Modules -----------------------

local rendererBackends = {
	OpenGLRenderer = {
		Option = "opengl",
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

			if is_plat("wasm") then
				add_ldflags('-sFULL_ES2 -sFULL_ES3', { public = true })
				remove_files("src/Nazara/OpenGLRenderer/Wrapper/EGL/**.cpp")
			else
				remove_files("src/Nazara/OpenGLRenderer/Wrapper/Web/**.cpp")
			end
		end
	},
	VulkanRenderer = {
		Option = "vulkan",
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
		Option = "audio",
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
				add_defines("AL_ALEXT_PROTOTYPES")
				add_packages("openal-soft", { links = {} })
			end
		end
	},
	BulletPhysics3D = {
		Option = "bulletphysics",
		Deps = {"NazaraUtility"},
		Packages = { "bullet3", "entt" }
	},
	Core = {
		Custom = function ()
			add_headerfiles("include/(Nazara/*.hpp)")
			-- NazaraMath is header-only, make it part of the core project
			add_headerfiles("include/(Nazara/Math/**.hpp)", "include/(Nazara/Math/**.inl)")

			if has_config("embed_plugins") then
				add_defines("NAZARA_PLUGINS_STATIC", { public = true })
			end

			if is_plat("windows", "mingw") then
				add_syslinks("ole32")
			elseif is_plat("linux") then
				add_packages("libuuid")
				add_syslinks("dl", "pthread")
			elseif is_plat("wasm") then
				--[[
				Have to fix issues with libsdl first
				add_ldflags("-sPTHREAD_POOL_SIZE=4", { public = true })
				add_cxflags("-pthread", { public = true })
				add_ldflags("-pthread", { public = true })
				]]
			end

			if is_plat("macosx", "iphoneos") then
				add_headerfiles("src/Nazara/Core/Darwin/TimeImpl.hpp", { prefixdir = "private", install = false })
				add_files("src/Nazara/Core/Darwin/TimeImpl.cpp")

				remove_headerfiles("src/Nazara/Core/Posix/TimeImpl.hpp")
				remove_files("src/Nazara/Core/Posix/TimeImpl.cpp")
			end
		end,
		Packages = { "entt", "frozen" },
		PublicPackages = { "nazarautils" }
	},
	Graphics = {
		Option = "graphics",
		Deps = {"NazaraRenderer"},
		Packages = {"entt"}
	},
	JoltPhysics3D = {
		Option = "joltphysics",
		Deps = {"NazaraUtility"},
		Packages = { "joltphysics", "entt", "ordered_map" }
	},
	Network = {
		Option = "network",
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
		Option = "chipmunkphysics",
		Deps = {"NazaraUtility"},
		Packages = { "chipmunk2d", "entt" }
	},
	Platform = {
		Option = "platform",
		Deps = {"NazaraUtility"},
		Custom = function()
			add_packages("libsdl", { components = {"lib"}})
			if is_plat("windows", "mingw") then
				add_defines("SDL_VIDEO_DRIVER_WINDOWS=1")
			elseif is_plat("linux") then
				add_defines("SDL_VIDEO_DRIVER_X11=1")
				add_defines("SDL_VIDEO_DRIVER_WAYLAND=1")
				add_packages("libxext", "wayland", { links = {} }) -- we only need X11 headers
			elseif is_plat("macosx") then
				add_defines("SDL_VIDEO_DRIVER_COCOA=1")
				add_packages("libx11", { links = {} }) -- we only need X11 headers
			elseif is_plat("wasm") then
				-- emscripten enables USE_SDL by default which will conflict with the sdl headers
				add_cxflags("-sUSE_SDL=0")
				add_ldflags("-sUSE_SDL=0", { public = true })
			end
		end
	},
	Renderer = {
		Option = "renderer",
		Deps = {"NazaraPlatform"},
		PublicPackages = { "nazarautils", "nzsl" },
		Custom = function ()
			if has_config("embed_rendererbackends") then
				-- Embed backends code inside our own modules
				add_defines("NAZARA_RENDERER_EMBEDDEDBACKENDS")
				for name, module in table.orderpairs(rendererBackends) do
					if not module.Option or has_config(module.Option) then
						ModuleTargetConfig(name, module)
					end
				end
			end
		end
	},
	Utility = {
		Option = "utility",
		Deps = {"NazaraCore"},
		Packages = {"entt", "freetype", "frozen", "ordered_map", "stb"}
	},
	Widgets = {
		Option = "widgets",
		Deps = {"NazaraGraphics"},
		Packages = {"entt", "kiwisolver"}
	}
}

-- Vulkan doesn't run on web
if is_plat("wasm") then
	rendererBackends.VulkanRenderer = nil
end

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

set_project("NazaraEngine")
set_xmakever("2.7.3")

includes("xmake/**.lua")

----------------------- Global options -----------------------

option("compile_shaders", { description = "Compile nzsl shaders into an includable binary version", default = true })
option("embed_rendererbackends", { description = "Embed renderer backend code into NazaraRenderer instead of loading them dynamically", default = is_plat("wasm") or false })
option("embed_resources", { description = "Turn builtin resources into includable headers", default = true })
option("embed_plugins", { description = "Embed enabled plugins code as static libraries", default = is_plat("wasm") or false })
option("link_openal", { description = "Link OpenAL in the executable instead of dynamically loading it", default = is_plat("wasm") or false })
option("override_runtime", { description = "Override vs runtime to MD in release and MDd in debug", default = true })
option("usepch", { description = "Use precompiled headers to speedup compilation", default = false })
option("unitybuild", { description = "Build the engine using unity build", default = false })

-- Allow to disable some modules
for name, module in pairs(modules) do
	if module.Option then
		option(module.Option, { description = "Enables the " .. name .. " module", default = true, category = "Modules" })
	end
end

----------------------- Dependencies -----------------------

add_repositories("nazara-engine-repo https://github.com/NazaraEngine/xmake-repo")

add_requires("entt 3.11.1", "fmt", "frozen", "kiwisolver", "nazarautils")

-- Module dependencies
if has_config("audio") then
	add_requires("dr_wav", "libflac", "minimp3")
	add_requires("libvorbis", { configs = { with_vorbisenc = false } })

	if not is_plat("wasm") then
		-- OpenAL is supported as a system library on wasm
		add_requires("openal-soft", { configs = { shared = true }})
	end
end

if has_config("bulletphysics") then
	add_requires("bullet3", { configs = { debug = is_mode("debug") }})
end

if has_config("chipmunkphysics") then
	add_requires("chipmunk2d")
end

if has_config("joltphysics") then
	add_requires("joltphysics 24147a6936c63f4f09c6c37173169d8f366f9fb4", { configs = { debug = is_mode("debug") }})
	add_requires("ordered_map")
end

if has_config("platform") then
	add_requires("libsdl >=2.26.0")
end

if has_config("renderer") then
	add_requires("nzsl", { debug = is_mode("debug"), configs = { with_symbols = not is_mode("release"), shared = not is_plat("wasm", "android") } })

	-- When cross-compiling, compile shaders using host shader compiler
	if is_plat("android", "iphoneos", "wasm") or (is_plat("mingw") and not is_host("windows")) then
		if has_config("compile_shaders") then
			add_requires("nzsl~host", { kind = "binary", host = true })
		end
	end
end

if has_config("utility") then
	add_requires("freetype", { configs = { bzip2 = true, png = true, woff2 = true, zlib = true, debug = is_mode("debug") } })
	add_requires("ordered_map", "stb")
end

if has_config("widgets") then
	add_requires("kiwisolver")
end

-- Platform-specific dependencies
if is_plat("linux", "android") then 
	add_requires("libuuid")
end

if is_plat("linux") then
	add_requires("libxext", "wayland")
end

----------------------- Global config -----------------------

add_rules("mode.asan", "mode.tsan", "mode.coverage", "mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("build.rendererplugins")

if has_config("examples") then
	add_rules("download.assets.examples")
end

if has_config("tests") then
	add_rules("download.assets.unittests")
end

set_allowedplats("windows", "mingw", "linux", "macosx", "wasm")
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
set_targetdir("./bin/$(plat)_$(arch)_$(mode)")
set_warnings("allextra")

if not is_mode("release") then
	set_symbols("debug", "hidden")
end

if is_mode("debug") then
	add_defines("NAZARA_DEBUG")
end

if is_plat("windows") then
	if has_config("override_runtime") then
		set_runtimes(is_mode("debug") and "MDd" or "MD")
	end

	add_defines("_CRT_SECURE_NO_WARNINGS", "_ENABLE_EXTENDED_ALIGNED_STORAGE")
	add_cxxflags("/bigobj", "/permissive-", "/Zc:__cplusplus", "/Zc:externConstexpr", "/Zc:inline", "/Zc:lambda", "/Zc:preprocessor", "/Zc:referenceBinding", "/Zc:strictStrings", "/Zc:throwingNew")
	add_cxflags("/w44062") -- Enable warning: switch case not handled
	add_cxflags("/wd4251") -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
	add_cxflags("/wd4275") -- Disable warning: DLL-interface class 'class_1' used as base for DLL-interface blah
elseif is_plat("mingw") then
	add_cxflags("-Og", "-Wa,-mbig-obj")
	add_ldflags("-Wa,-mbig-obj")
elseif is_plat("wasm") then
	add_cxflags("-sNO_DISABLE_EXCEPTION_CATCHING")
	add_ldflags("-sNO_DISABLE_EXCEPTION_CATCHING", "-sALLOW_MEMORY_GROWTH", "-sWASM_BIGINT")
	if is_mode("debug") then
		add_ldflags("-sERROR_ON_WASM_CHANGES_AFTER_LINK", { force = true })
	end
end

----------------------- Targets -----------------------

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
	if not is_plat("windows", "mingw") then
		remove_headerfiles("src/Nazara/" .. name .. "/Win32/**")
		remove_files("src/Nazara/" .. name .. "/Win32/**")
	end

	if not is_plat("linux", "android", "cross") then
		remove_headerfiles("src/Nazara/" .. name .. "/Linux/**")
		remove_files("src/Nazara/" .. name .. "/Linux/**")
	end

	if not is_plat("macosx", "iphoneos") then
		remove_headerfiles("src/Nazara/" .. name .. "/Darwin/**")
		remove_files("src/Nazara/" .. name .. "/Darwin/**")
	end

	if not is_plat("linux", "macosx", "iphoneos", "android", "wasm", "cross") then
		remove_headerfiles("src/Nazara/" .. name .. "/Posix/**")
		remove_files("src/Nazara/" .. name .. "/Posix/**")
	end

	if module.Custom then
		module.Custom()
	end
end

for name, module in pairs(modules) do
	if module.Option and not has_config(module.Option) then
		goto continue
	end

	target("Nazara" .. name, function ()
		set_group("Modules")

		-- for now only shared compilation is supported (except on platforms like wasm)
		if not is_plat("wasm") then
			set_kind("shared")
		else
			set_kind("static")
			add_defines("NAZARA_STATIC", { public = true })
		end

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
		if is_plat("windows", "mingw") then
			add_defines("NAZARA_UTILS_WINDOWS_NT6=1")
		end

		add_includedirs("src")

		ModuleTargetConfig(name, module)
	end)

	::continue::
end

includes("tools/*.lua")
includes("tests/*.lua")
includes("examples/*.lua")
includes("plugins/*.lua")
includes("documentation/*.lua")
