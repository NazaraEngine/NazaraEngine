target("WidgetDemo")
	add_deps("NazaraGraphics", "NazaraWidgets")
	add_packages("entt")
	add_files("main.cpp")
	add_defines("NAZARA_ENTT")
