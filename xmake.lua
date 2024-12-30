----------------------- Modules -----------------------

local rendererBackends = {
	OpenGLRenderer = {
		Option = "opengl",
		Deps = {"NazaraRenderer"},
		Packages = {"opengl-headers"},
		Custom = function()
			if is_plat("windows", "mingw") then
				add_syslinks("User32")
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

			if is_plat("bsd") then
				add_defines("EGL_NO_X11")
				add_packages("wayland", { links = {} }) -- we only need wayland headers
            end

			if is_plat("wasm") then
				add_ldflags("-sFULL_ES2", "-sFULL_ES3", { public = true })
				remove_files("src/Nazara/OpenGLRenderer/Wrapper/EGL/**.cpp")
			else
				remove_files("src/Nazara/OpenGLRenderer/Wrapper/Web/**.cpp")
			end
		end
	},
	VulkanRenderer = {
		Option = "vulkan",
		Deps = {"NazaraRenderer"},
		Packages = {"vulkan-headers", "vulkan-memory-allocator"},
		Custom = function()
			add_defines("VK_NO_PROTOTYPES")
			if is_plat("windows", "mingw") then
				add_defines("VK_USE_PLATFORM_WIN32_KHR")
				add_syslinks("User32")
			elseif is_plat("linux", "bsd") then
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
		Packages = {"dr_mp3", "dr_wav", "frozen", "libflac", "libvorbis"},
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
	Core = {
		Custom = function ()
			add_headerfiles("include/(Nazara/*.hpp)")
			-- NazaraMath is header-only, make it part of the core project
			add_headerfiles("include/(Nazara/Math/**.hpp)", "include/(Nazara/Math/**.inl)")

			if has_config("embed_plugins", "static") then
				add_defines("NAZARA_PLUGINS_STATIC", { public = true })
			end

			if has_config("embed_unicodetable") then
				add_defines("NAZARA_CORE_EMBED_UNICODEDATA")
			end

			if is_plat("windows", "mingw") then
				add_syslinks("Advapi32", "Ole32", "Shell32", "Userenv")
				if is_plat("mingw") then
					-- uuid syslinks seems necessary for folderid
					-- https://stackoverflow.com/questions/66338153/undefined-reference-to-folderid-c
					add_syslinks("uuid")
				end
			elseif is_plat("linux") then
				add_packages("libuuid")
				add_syslinks("dl", "pthread")
			elseif is_plat("bsd") then
				add_packages("libuuid")
				add_syslinks("pthread")
			elseif is_plat("wasm") then
				--[[
				Have to fix issues with libsdl first
				add_ldflags("-sPTHREAD_POOL_SIZE=4", { public = true })
				add_cxflags("-pthread", { public = true })
				add_ldflags("-pthread", { public = true })
				]]
			end

			if is_plat("macosx", "iphoneos") then
				add_headerfiles("src/Nazara/Core/Darwin/TimeImpl.hpp", { install = false })
				add_files("src/Nazara/Core/Darwin/TimeImpl.cpp")

				remove_headerfiles("src/Nazara/Core/Posix/TimeImpl.hpp")
				remove_files("src/Nazara/Core/Posix/TimeImpl.cpp")
			end
		end,
		Packages = { "concurrentqueue", "entt", "frozen", "ordered_map", "stb", "utfcpp" },
		PublicPackages = { "nazarautils" }
	},
	Graphics = {
		Option = "graphics",
		Deps = {"NazaraRenderer", "NazaraTextRenderer"},
		Packages = {"entt", "fast_float"}
	},
	Physics2D = {
		Option = "physics2d",
		Deps = {"NazaraCore"},
		Packages = { "chipmunk2d", "entt" }
	},
	Physics3D = {
		Option = "physics3d",
		Deps = {"NazaraCore"},
		Packages = { "joltphysics", "entt", "ordered_map" }
	},
	Network = {
		Option = "network",
		Deps = {"NazaraCore"},
		Custom = function ()
			if not is_plat("wasm") then
				if has_config("link_curl") then
					add_defines("NAZARA_NETWORK_CURL_LINK")
					add_packages("libcurl")
				else
					add_packages("libcurl", { links = {} })
				end
			else
				add_ldflags("-sFETCH", { public = true })
				remove_headerfiles("include/Nazara/Network/CurlLibrary.hpp")
				remove_files("src/Nazara/Network/CurlLibrary.cpp")
			end

			if is_plat("windows", "mingw") then
				add_syslinks("WS2_32")
			end

			if is_plat("linux") then
				remove_files("src/Nazara/Network/Posix/SocketPollerImpl.hpp")
				remove_files("src/Nazara/Network/Posix/SocketPollerImpl.cpp")
			end
		end
	},
	Platform = {
		Option = "platform",
		Deps = {"NazaraCore"},
		Packages = {"utfcpp"},
		Custom = function()
			add_packages("libsdl", { components = {"lib"} })
			if is_plat("windows", "mingw") then
				add_defines("SDL_VIDEO_DRIVER_WINDOWS=1")
			elseif is_plat("linux", "bsd") then
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
		Packages = { "frozen" },
		PublicPackages = { "nazarautils", "nzsl" },
		Custom = function ()
			if has_config("embed_rendererbackends", "static") then
				-- Embed backends code inside our own modules
				add_defines("NAZARA_RENDERER_EMBEDDEDBACKENDS")
				for name, module in table.orderpairs(rendererBackends) do
					if not module.Option or has_config(module.Option) then
						if module.Deps then
							module = table.clone(module, 1) -- shallow clone
							module.Deps = table.remove_if(table.clone(module.Deps), function (idx, dep) return dep == "NazaraRenderer" end)
							if #module.Deps == 0 then 
								module.Deps = nil 
							end
						end

						ModuleTargetConfig(name, module)
					end
				end
			end
		end
	},
	TextRenderer = {
		Option = "textrenderer",
		Deps = {"NazaraCore"},
		Packages = {"freetype", "frozen"}
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

if not has_config("embed_rendererbackends", "static") and has_config("renderer") then
	-- Register renderer backends as separate modules
	for name, module in pairs(rendererBackends) do
		if (modules[name] ~= nil) then
			os.raise("overriding module " .. name)
		end

		modules[name] = module
	end
end

NazaraModules = modules

set_project("NazaraEngine")
set_xmakever("2.8.7")
set_license("MIT")

includes("xmake/**.lua")

----------------------- Global options -----------------------

option("compile_shaders", { description = "Compile nzsl shaders into an includable binary version", default = true })
option("embed_rendererbackends", { description = "Embed renderer backend code into NazaraRenderer instead of loading them dynamically", default = is_plat("wasm") or false })
option("embed_resources", { description = "Turn builtin resources into includable headers", default = true })
option("embed_plugins", { description = "Embed enabled plugins code as static libraries", default = is_plat("wasm") or false })
option("embed_unicodetable", { description = "Embed Unicode characters table", default = true })
option("link_curl", { description = "Link libcurl in the executable instead of dynamically loading it", default = false })
option("link_openal", { description = "Link OpenAL in the executable instead of dynamically loading it", default = is_plat("wasm") or false })
option("static", { description = "Build the engine statically (implies embed_rendererbackends and embed_plugins)", default = is_plat("wasm") or false })
option("override_runtime", { description = "Override vs runtime to MD in release and MDd in debug", default = true })
option("unitybuild", { description = "Build the engine using unity build", default = false })
option("usepch", { description = "Use precompiled headers to speedup compilation", default = false })

-- Sanitizers
local sanitizers = {
	asan = "address",
	lsan = "leak",
	tsan = "thread",
}

for opt, policy in table.orderpairs(sanitizers) do
	option(opt, { description = "Enable " .. opt, default = false })

	if has_config(opt) then
		add_defines("NAZARA_WITH_" .. opt:upper())
		set_policy("build.sanitizer." .. policy, true)
	end
end

-- Allow to disable some modules
for name, module in table.orderpairs(modules) do
	if module.Option then
		option(module.Option, { description = "Enables the " .. name .. " module", default = true, category = "Modules" })
	end
end

----------------------- Dependencies -----------------------

add_repositories("nazara-engine-repo https://github.com/NazaraEngine/xmake-repo")

add_requires(
	"concurrentqueue",
	"entt 3.13.2",
	"fmt",
	"frozen",
	"ordered_map",
	"nazarautils >=2024.11.23",
	"stb",
	"utfcpp"
)

-- Don't link with system-installed libs on CI
if os.getenv("CI") then
	add_requireconfs("*", { system = false })
	add_requireconfs("freetype.*", { system = false })
end

-- Module dependencies
if has_config("audio") then
	add_requires("dr_mp3", "dr_wav", "libflac")
	add_requires("libvorbis", { configs = { with_vorbisenc = false } })

	if not is_plat("wasm") then
		-- OpenAL is supported as a system library on wasm
		add_requires("openal-soft", { configs = { shared = true }})
	end
end

if has_config("graphics") then
	add_requires("fast_float")
end

if has_config("physics2d") then
	add_requires("chipmunk2d")
end

if has_config("physics3d") then
	add_requires("joltphysics v5.2.0", { configs = { debug = is_mode("debug") }})
end

if has_config("network") then
	-- emscripten fetch API is used for WebService on wasm
	if not is_plat("wasm") then
		if has_config("link_curl") then
			add_requires("libcurl", { configs = { asan = false, openssl = is_plat("linux", "android") } })
		else
			add_requires("libcurl", { configs = { asan = false, openssl = is_plat("linux", "android"), shared = true }})
		end
	end
end

if has_config("opengl") then
	add_requires("opengl-headers")
end

if has_config("platform") then
	add_requires("libsdl >=2.26.0")
	if is_plat("linux", "bsd") then
		add_requires("libxext", "wayland", { configs = { asan = false } })
	end
end

if has_config("renderer") then
	add_requires("nzsl >=2024.11.27", { debug = is_mode("debug"), configs = { symbols = not is_mode("release"), shared = not is_plat("wasm", "android") and not has_config("static") } })

	-- When cross-compiling, compile shaders using host shader compiler
	if has_config("compile_shaders") and is_cross() then
		add_requires("nzsl~host", { kind = "binary", host = true })
	end
end

if has_config("textrenderer") then
	add_requires("freetype", { configs = { bzip2 = true, png = true, woff2 = true, zlib = true, debug = is_mode("debug") } })
end

if has_config("vulkan") and not is_plat("wasm") then
	add_requires("vulkan-headers", "vulkan-memory-allocator")
end

if has_config("widgets") then
	add_requires("kiwisolver")
end

-- Platform-specific dependencies
if is_plat("linux", "android", "bsd") then
	add_requires("libuuid")
end

----------------------- Global config -----------------------

add_rules("mode.coverage", "mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("build.rendererplugins")
add_rules("natvis")

if has_config("examples") then
	add_rules("download.assets.examples")
end

if has_config("tests") then
	add_rules("download.assets.unittests")
end

set_allowedplats("windows", "mingw", "linux", "macosx", "bsd", "wasm")
set_allowedmodes("debug", "releasedbg", "release", "coverage")
set_defaultmode("debug")

add_includedirs("include")
add_sysincludedirs("thirdparty/include")

set_encodings("utf-8")
set_exceptions("cxx")
set_languages("c89", "c++20")
set_rundir("./bin/$(plat)_$(arch)_$(mode)")
set_targetdir("./bin/$(plat)_$(arch)_$(mode)")
set_warnings("allextra")

if is_mode("debug") then
	add_rules("debug.suffix")
	add_defines("NAZARA_DEBUG")
elseif is_mode("coverage") then
	if not is_plat("windows") then
		add_links("gcov")
	end
elseif is_mode("releasedbg", "release") then
	set_fpmodels("fast")
	add_vectorexts("sse", "sse2", "sse3", "ssse3")
end

if not is_mode("release") then
	set_symbols("debug", "hidden")
end

-- Compiler-specific options
add_rules("@nazarautils/compiler_setup")
add_cxflags("-Wno-missing-field-initializers", { tools = { "clang", "gcc" } })

-- Platform-specific options

if is_plat("windows") then
	if has_config("override_runtime") then
		set_runtimes(is_mode("debug") and "MDd" or "MD")
	end
elseif is_plat("mingw") then
	-- Use some optimizations even in debug for MinGW to reduce object size
	if is_mode("debug") then
		add_cxflags("-Og")
	end
	add_cxflags("-Wa,-mbig-obj")
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
	for _, ext in ipairs({".h", ".hpp", ".inl"}) do
		add_headerfiles("include/(Nazara/" .. name .. "/**" .. ext .. ")")
		add_headerfiles("src/Nazara/" .. name .. "/**" .. ext, { install = false })
	end
	remove_headerfiles("src/Nazara/" .. name .. "/Resources/**.h")

	-- Add extra files for projects
	for _, ext in ipairs({".natvis", ".nzsl"}) do
		add_extrafiles("include/Nazara/" .. name .. "/**" .. ext)
		add_extrafiles("src/Nazara/" .. name .. "/**" .. ext)
	end

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
		add_rules("@nzsl/archive.shaders")
		add_rules("@nzsl/compile.shaders", { inplace = true })

		-- Regular shaders (Shaders/**.nzsl => .nzslb.h)
		for _, filepath in pairs(os.files("src/Nazara/" .. name .. "/Shaders/**.nzsl")) do
			add_files(filepath)
		end

		-- Shader archives (ShaderArchives/foo/**.nzsl => ShaderArchives/foo.nzsla.h)
		for _, archivefolder in pairs(os.dirs("src/Nazara/" .. name .. "/ShaderArchives/*")) do
			local archive = archivefolder .. ".nzsla.h"
			for _, filepath in pairs(os.files(archivefolder .. "/**.nzsl")) do
				add_files(filepath, { archive = archive })
			end
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

	if not is_plat("linux", "macosx", "iphoneos", "android", "wasm", "cross", "bsd") then
		remove_headerfiles("src/Nazara/" .. name .. "/Posix/**")
		remove_files("src/Nazara/" .. name .. "/Posix/**")
	end

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

		-- handle shared/static kind
		if is_plat("wasm") or has_config("static") then
			set_kind("static")
			add_defines("NAZARA_STATIC", { public = true })
		else
			set_kind("shared")
		end
		
		add_defines("NAZARA_BUILD")
		add_includedirs("src")
		add_packages("fmt")	-- fmt is a special package that is not public but required by all Nazara modules
		add_rpathdirs("$ORIGIN")

		if has_config("usepch") then
			set_pcxxheader("include/Nazara/" .. name .. ".hpp")
		end

		if has_config("unitybuild") then
			add_rules("c++.unity_build", {uniqueid = "NAZARA_UNITY_ID", batchsize = 12})
		end

		if is_plat("windows", "mingw") then
			add_defines("NAZARAUTILS_WINDOWS_NT6=1")
		end

		ModuleTargetConfig(name, module)
	end)

	::continue::
end

includes("tools/*.lua")
includes("tests/*.lua")
includes("examples/*.lua")
includes("plugins/*.lua")
includes("documentation/*.lua")
