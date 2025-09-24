if is_plat("wasm") then
	return -- Threads are broken for now
end

target("PlayMusic")
	add_deps("NazaraAudio2")
	add_files("main.cpp")
