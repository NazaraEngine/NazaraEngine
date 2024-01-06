package("ffmpeg")
    set_homepage("https://www.ffmpeg.org")
    set_description("A collection of libraries to process multimedia content such as audio, video, subtitles and related metadata.")
    set_license("GPL-3.0")

    if is_plat("windows", "mingw") then
        add_urls("https://github.com/xmake-mirror/ffmpeg-releases/releases/download/releases/ffmpeg-$(version)-full_build-shared.7z")
        add_versions("6.1", "6eab95d8cd00b4601ce443ff3aeab5f477c581379c633742f5df30856a95158d")
        add_versions("6.0", "a852648944cdf4ce3aeebf16d750a036fe67ab657b82eb43f9f7fdcc7bef3af7")
        add_versions("5.1.2", "d9eb97b72d7cfdae4d0f7eaea59ccffb8c364d67d88018ea715d5e2e193f00e9")
        add_versions("5.0.1", "ded28435b6f04b74f5ef5a6a13761233bce9e8e9f8ecb0eabe936fd36a778b0c")

        add_configs("shared", {description = "Download shared binaries.", default = true, type = "boolean", readonly = true})
        add_configs("vs_runtime", {description = "Set vs compiler runtime.", default = "MD", readonly = true})
    else
        add_urls("https://ffmpeg.org/releases/ffmpeg-$(version).tar.bz2", {alias = "home"})
        add_urls("https://github.com/FFmpeg/FFmpeg/archive/n$(version).zip", {alias = "github"})
        add_urls("https://git.ffmpeg.org/ffmpeg.git", "https://github.com/FFmpeg/FFmpeg.git", {alias = "git"})
        add_versions("home:6.1", "eb7da3de7dd3ce48a9946ab447a7346bd11a3a85e6efb8f2c2ce637e7f547611")
        add_versions("home:6.0.1", "2c6e294569d1ba8e99cbf1acbe49e060a23454228a540a0f45d679d72ec69a06")
        add_versions("home:5.1.2", "39a0bcc8d98549f16c570624678246a6ac736c066cebdb409f9502e915b22f2b")
        add_versions("home:5.1.1", "cd0e16f903421266d5ccddedf7b83b9e5754aef4b9f7a7f06ce9e4c802f0545b")
        add_versions("home:5.0.1", "28df33d400a1c1c1b20d07a99197809a3b88ef765f5f07dc1ff067fac64c59d6")
        add_versions("home:4.0.2", "346c51735f42c37e0712e0b3d2f6476c86ac15863e4445d9e823fe396420d056")
        add_versions("github:6.1", "7da07ff7e30bca95c0593db20442becba13ec446dd9c3331ca3d1b40eecd3c93")
        add_versions("github:6.0.1", "2acb5738a1b4b262633ac2d646340403ae47120d9eb289ecad23fc90093c0d6c")
        add_versions("github:5.1.2", "0c99f3609160f40946e2531804175eea16416320c4b6365ad075e390600539db")
        add_versions("github:5.1.1", "a886fcc94792764c27c88ebe71dffbe5f0d37df8f06f01efac4833ac080c11bf")
        add_versions("github:5.0.1", "f9c2e06cafa4381df8d5c9c9e14d85d9afcbc10c516c6a206f821997cc7f6440")
        add_versions("github:4.0.2", "4df1ef0bf73b7148caea1270539ef7bd06607e0ea8aa2fbf1bb34062a097f026")
        add_versions("git:6.1", "n6.1")
        add_versions("git:6.0.1", "n6.0.1")
        add_versions("git:5.1.2", "n5.1.2")
        add_versions("git:5.1.1", "n5.1.1")
        add_versions("git:5.0.1", "n5.0.1")
        add_versions("git:4.0.2", "n4.0.2")

        add_configs("gpl",              {description = "Enable GPL code", default = true, type = "boolean"})
        add_configs("ffprobe",          {description = "Enable ffprobe program.", default = false, type = "boolean"})
        add_configs("ffmpeg",           {description = "Enable ffmpeg program.", default = true, type = "boolean"})
        add_configs("ffplay",           {description = "Enable ffplay program.", default = false, type = "boolean"})
        add_configs("zlib",             {description = "Enable zlib compression library.", default = false, type = "boolean"})
        add_configs("lzma",             {description = "Enable liblzma compression library.", default = false, type = "boolean"})
        add_configs("bzlib",            {description = "Enable bzlib compression library.", default = false, type = "boolean"})
        add_configs("libx264",          {description = "Enable libx264 decoder.", default = false, type = "boolean"})
        add_configs("libx265",          {description = "Enable libx265 decoder.", default = false, type = "boolean"})
        add_configs("iconv",            {description = "Enable libiconv library.", default = false, type = "boolean"})
        add_configs("vaapi",            {description = "Enable vaapi library.", default = false, type = "boolean"})
        add_configs("vdpau",            {description = "Enable vdpau library.", default = false, type = "boolean"})
        add_configs("hardcoded-tables", {description = "Enable hardcoded tables.", default = true, type = "boolean"})
    end

    add_links("avfilter", "avdevice", "avformat", "avcodec", "swscale", "swresample", "avutil")
    if is_plat("macosx") then
        add_frameworks("CoreFoundation", "Foundation", "CoreVideo", "CoreMedia", "AudioToolbox", "VideoToolbox", "Security")
    elseif is_plat("linux") then
        add_syslinks("pthread")
    end

    if is_plat("linux", "macosx") then
        add_deps("yasm")
    end

    if on_fetch then
        on_fetch("mingw", "linux", "macosx", function (package, opt)
            import("lib.detect.find_tool")
            if opt.system then
                local result
                for _, name in ipairs({"libavcodec", "libavdevice", "libavfilter", "libavformat", "libavutil", "libpostproc", "libswresample", "libswscale"}) do
                    local pkginfo = package:find_package("pkgconfig::" .. name, opt)
                    if pkginfo then
                        pkginfo.version = nil
                        if not result then
                            result = pkginfo
                        else
                            result = result .. pkginfo
                        end
                    else
                        return
                    end
                end
                local ffmpeg = find_tool("ffmpeg", {check = "-help", version = true, command = "-version", parse = "%d+%.?%d+%.?%d+", force = true})
                if ffmpeg then
                    result.version = ffmpeg.version
                end
                return result
            end
        end)
    end

    on_load("linux", "macosx", "android", function (package)
        local configdeps = {zlib    = "zlib",
                            bzlib   = "bzip2",
                            lzma    = "xz",
                            libx264 = "x264",
                            libx265 = "x265",
                            iconv   = "libiconv"}
        for name, dep in pairs(configdeps) do
            if package:config(name) then
                package:add("deps", dep)
            end
        end
        -- https://www.ffmpeg.org/platform.html#toc-Advanced-linking-configuration
        if package:config("pic") ~= false and not package:is_plat("macosx") then
            package:add("shflags", "-Wl,-Bsymbolic")
            package:add("ldflags", "-Wl,-Bsymbolic")
        end
        if not package:config("gpl") then
            package:set("license", "LGPL-3.0")
        end
    end)

    on_install("windows|x64", "mingw|x86_64", function (package)
        os.cp("bin", package:installdir())
        os.cp("include", package:installdir())
        if package:is_plat("windows") then
            os.cp("lib/*.def", package:installdir("lib"))
            os.cp("lib/*.lib", package:installdir("lib"))
        else
            os.cp("lib/*.a", package:installdir("lib"))
        end
        package:addenv("PATH", "bin")
    end)

    on_install("linux", "macosx", "android@linux,macosx", function (package)
        local configs = {"--enable-version3",
                         "--disable-doc"}
        if package:config("gpl") then
            table.insert(configs, "--enable-gpl")
        end
        if package:is_plat("macosx") and macos.version():ge("10.8") then
            table.insert(configs, "--enable-videotoolbox")
        end
        for name, enabled in pairs(package:configs()) do
            if not package:extraconf("configs", name, "builtin") then
                if enabled then
                    table.insert(configs, "--enable-" .. name)
                else
                    table.insert(configs, "--disable-" .. name)
                end
            end
        end
        if package:config("shared") then
            table.insert(configs, "--enable-shared")
            table.insert(configs, "--disable-static")
        else
            table.insert(configs, "--enable-static")
            table.insert(configs, "--disable-shared")
        end
        if package:debug() then
            table.insert(configs, "--enable-debug")
        else
            table.insert(configs, "--disable-debug")
        end
        if package:is_plat("android") then
            import("core.base.option")
            import("core.tool.toolchain")
            local ndk = toolchain.load("ndk", {plat = package:plat(), arch = package:arch()})
            local bin = ndk:bindir()
            local ndk_sdkver = ndk:config("ndk_sdkver")
            local arch, cpu, triple, cross_prefix
            if package:is_arch("arm64-v8a") then
                arch = "arm64"
                cpu = "armv8-a"
                triple = "aarch64-linux-android"
                cross_prefix = path.join(bin, "aarch64-linux-android-")
            elseif package:arch():startswith("arm") then
                arch = "arm"
                cpu = "armv7-a"
                triple = "armv7a-linux-androideabi"
                cross_prefix = path.join(bin, "arm-linux-androideabi-")
            elseif package:is_arch("x86") then
                arch = "x86_android"
                cpu = "x86"
                triple = "i686-linux-android"
                cross_prefix = path.join(bin, "i686-linux-android-")
             elseif package:is_arch("x86_64") then
                arch = "x86_64_android"
                cpu = "x86_64"
                triple = "x86_64-linux-android"
                cross_prefix = path.join(bin, "x86_64-linux-android-")
            else
                raise("unknown arch(%s) for android!", package:arch())
            end
            local sysroot  = path.join(path.directory(bin), "sysroot")
            local cflags   = table.join(table.wrap(package:config("cxflags")), table.wrap(package:config("cflags")), table.wrap(get_config("cxflags")), get_config("cflags"))
            local cxxflags = table.join(table.wrap(package:config("cxflags")), table.wrap(package:config("cxxflags")), table.wrap(get_config("cxflags")), get_config("cxxflags"))
            assert(os.isdir(sysroot), "we do not support old version ndk!")
            if package:is_arch("arm64-v8a") then
                table.insert(cflags, "-mfpu=neon")
                table.insert(cflags, "-mfloat-abi=soft")
            else
                table.insert(cflags, "-mfpu=neon")
                table.insert(cflags, "-mfloat-abi=soft")
            end
            table.insert(configs, "--enable-neon")
            table.insert(configs, "--enable-asm")
            table.insert(configs, "--enable-jni")
            table.insert(configs, "--target-os=android")
            table.insert(configs, "--enable-cross-compile")
            table.insert(configs, "--disable-avdevice")
            table.insert(configs, "--arch=" .. arch)
            table.insert(configs, "--cpu=" .. cpu)
            table.insert(configs, "--cc=" .. path.join(bin, triple .. ndk_sdkver .. "-clang"))
            table.insert(configs, "--cxx=" .. path.join(bin, triple .. ndk_sdkver .. "-clang++"))
            table.insert(configs, "--ar=" .. path.join(bin, "llvm-ar"))
            table.insert(configs, "--ranlib=" .. path.join(bin, "llvm-ranlib"))
            table.insert(configs, "--strip=" .. path.join(bin, "llvm-strip"))
            table.insert(configs, "--extra-cflags=" .. table.concat(cflags, ' '))
            table.insert(configs, "--extra-cxxflags=" .. table.concat(cxxflags, ' '))
            table.insert(configs, "--sysroot=" .. sysroot)
            table.insert(configs, "--cross-prefix=" .. cross_prefix)
            table.insert(configs, "--prefix=" .. package:installdir())
            os.vrunv("./configure", configs)
            local argv = {"-j4"}
            if option.get("verbose") then
                table.insert(argv, "V=1")
            end
            os.vrunv("make", argv)
            os.vrun("make install")
        else
            if package:is_cross() then
                table.insert(configs, "--arch=" .. package:targetarch())
                configs.host = "" -- prevents xmake to add a --host=xx parameter
            end
            import("package.tools.autoconf").install(package, configs)
        end
        package:addenv("PATH", "bin")
    end)

    on_test(function (package)
        assert(package:has_cfuncs("avformat_open_input", {includes = "libavformat/avformat.h"}))
    end)
package_end()

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
				add_syslinks("Ole32")
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
				add_headerfiles("src/Nazara/Core/Darwin/TimeImpl.hpp", { prefixdir = "private", install = false })
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
		Packages = {"entt"}
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

if not has_config("embed_rendererbackends", "static") then
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
set_xmakever("2.8.3")
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
	"nazarautils >=2024.07.19",
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
	add_requires("dr_wav", "libflac", "minimp3")
	add_requires("libvorbis", { configs = { with_vorbisenc = false } })

	if not is_plat("wasm") then
		-- OpenAL is supported as a system library on wasm
		add_requires("openal-soft", { configs = { shared = true }})
	end
end

if has_config("physics2d") then
	add_requires("chipmunk2d")
end

if has_config("physics3d") then
	add_requires("joltphysics >=4", { configs = { debug = is_mode("debug") }})
end

if has_config("network") then
	-- emscripten fetch API is used for WebService on wasm
	if not is_plat("wasm") then
		if has_config("link_curl") then
			add_requires("libcurl", { configs = { asan = false } })
		else
			add_requires("libcurl", { configs = { asan = false, shared = true }})
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
	add_requires("nzsl >=2023.12.31", { debug = is_mode("debug"), configs = { symbols = not is_mode("release"), shared = not is_plat("wasm", "android") and not has_config("static") } })

	local function is_cross_compiling()
		if os.host() == "windows" then
			local host_arch = os.arch()
			if is_plat("windows") then
				-- maybe cross-compilation for arm64 on x86/x64
				if (host_arch == "x86" or host_arch == "x64") and is_arch("arm64") then
					return true
				-- maybe cross-compilation for x86/64 on arm64
				elseif host_arch == "arm64" and not is_arch("arm64") then
					return true
				end
				return false
			elseif is_plat("mingw") then
				return false
			end
		end
		if not is_plat(os.host()) and not is_plat(os.subhost()) then
			return true
		end
		if not is_arch(os.arch()) and not is_arch(os.subarch()) then
			return true
		end
		return false
	end

	-- When cross-compiling, compile shaders using host shader compiler
	if has_config("compile_shaders") and is_cross_compiling() then
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
		add_headerfiles("src/Nazara/" .. name .. "/**" .. ext, { prefixdir = "private", install = false })
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
		add_rules("@nzsl/compile.shaders", { inplace = true })
		for _, filepath in pairs(table.join(os.files("src/Nazara/" .. name .. "/Resources/**.nzsl"), os.files("src/Nazara/" .. name .. "/Resources/**.nzslb"))) do
			add_files(filepath)
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
