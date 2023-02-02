if is_plat("wasm") then
	return -- Physics3D is not yet supported on wasm because of Newton
end

target("Tut00_EmptyProject")
	add_deps("NazaraAudio", "NazaraGraphics", "NazaraNetwork", "NazaraPhysics2D", "NazaraPhysics3D", "NazaraRenderer", "NazaraUtility")
	add_files("main.cpp")
