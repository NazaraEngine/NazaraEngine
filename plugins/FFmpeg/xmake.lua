option("ffmpeg")
	set_default(false)
	set_showmenu(true)
	set_description("Build FFmpeg plugin")

option_end()

if has_config("ffmpeg") then
	add_requires("ffmpeg", { configs = { shared = true } })

	target("PluginFFmpeg")
		set_kind("shared")
		set_group("Plugins")
		add_rpathdirs("$ORIGIN")

		add_deps("NazaraUtility")
		add_packages("ffmpeg")

		add_headerfiles("**.hpp", "**.inl", { prefixdir = "private" })
		add_includedirs(".")
		add_files("**.cpp")
end
