option("ffmpeg", { description = "Build FFmpeg plugin", default = false, category = "Plugins" })

if has_config("ffmpeg") then
	add_requires("ffmpeg", { configs = { shared = true } })

	target("PluginFFmpeg")
		set_group("Plugins")
		add_rpathdirs("$ORIGIN")

		add_deps("NazaraUtility")
		add_packages("ffmpeg")

		add_headerfiles("**.hpp", "**.inl", { prefixdir = "private", install = false })
		add_includedirs(".")
		add_files("**.cpp")
end
