option("tests-fuzzing")
	set_default(false)
	set_showmenu(true)
	set_description("Build fuzzing targets")

option_end()

if has_config("tests-fuzzing") then
        if is_plat("linux", "macosx") then
        	if is_mode("asan") then
			add_defines("CATCH_CONFIG_NO_WINDOWS_SEH")
			add_defines("CATCH_CONFIG_NO_POSIX_SIGNALS")
		end

		add_requires("catch2", "glslang", "spirv-tools")

		-- Common config
		set_group("Fuzz")
		set_kind("binary")

		add_deps("NazaraCore", "NazaraNetwork", "NazaraPhysics2D", "NazaraShader")
		add_packages("catch2", "glslang", "spirv-tools")
		add_headerfiles("Engine/**.hpp")
		add_files("Engine/Physics2D/**.cpp")
		add_includedirs(".")

		--[[if xmake.version():ge("2.5.9") then
			add_rules("c++.unity_build")
		end]]

		target("Nazara2DPhysicsUnitTestsFuzzing")
			add_files("Engine/Physics2D/PhysWorld2DFuzzTest.cpp")        
		end
end
