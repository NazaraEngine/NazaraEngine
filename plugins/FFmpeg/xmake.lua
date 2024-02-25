option("ffmpeg", { description = "Build FFmpeg plugin", default = false, category = "Plugins" })

if has_config("ffmpeg") then
	add_requires("ffmpeg", { configs = { asan = false, gpl = false, vs_runtime = "MD" } }) -- runtime is not used since FFmpeg is a C library

	target("PluginFFmpeg")
		set_group("Plugins")
		add_rpathdirs("$ORIGIN")
		set_license("LGPL-3.0")

		add_deps("NazaraCore")
		add_packages("ffmpeg")

		add_headerfiles("**.hpp", "**.inl", { prefixdir = "private", install = false })
		add_includedirs(".")
		add_files("**.cpp")
end
