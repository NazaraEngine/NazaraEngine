if is_plat("wasm") then
	return -- Physics3D is not yet supported on wasm because of Newton
end

target("PhysicsDemo")
	add_deps("NazaraGraphics", "NazaraPhysics3D")
	add_packages("entt")
	add_files("main.cpp")
	add_defines("NAZARA_ENTT")
