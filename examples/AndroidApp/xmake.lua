if not is_plat("android") then
	return
end

target("AndroidApp")
	set_kind("shared")
	add_files("main.cpp")
	set_filename("libNazaraApp.so")
	add_deps("NazaraGraphics")
	add_packages("entt")
	add_defines("NAZARA_ENTT")
