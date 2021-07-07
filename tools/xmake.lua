
add_requires("nodeeditor", {debug = is_mode("debug"), optional = true})

target("NazaraShaderNodes")
	set_group("Tools")
	set_kind("binary")
	add_rules("qt.console", "qt.moc")

	add_deps("NazaraShader")
	add_frameworks("QtCore", "QtGui", "QtWidgets")
	add_packages("nodeeditor")

	add_includedirs("../src")
	add_headerfiles("../src/ShaderNode/**.hpp", "../src/ShaderNode/**.inl")
	add_files("../src/ShaderNode/**.cpp")

	on_load(function (target)
		import("detect.sdks.find_qt")

		if (not has_package("nodeeditor") or not find_qt()) then
			-- Disable building by default if nodeeditor or Qt is not found
			target:set("default", false)
		end
	end)
