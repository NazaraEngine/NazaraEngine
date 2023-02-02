if is_plat("wasm") then 
	return -- Compute shaders are not supported with WebGL (but are with WebGPU)
end

target("ComputeTest")
	add_deps("NazaraRenderer")
	add_files("main.cpp")
