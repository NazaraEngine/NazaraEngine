if is_plat("wasm") then
	return -- Compute shaders are not supported with WebGL (but are with WebGPU)
end

target("ComputeParticlesTest")
	add_deps("NazaraRenderer")
	add_files("main.cpp")
	add_filegroups("shaders", {rootdir = "../../assets/shaders/compute"})
	add_headerfiles("../../assets/shaders/compute/compute_particle_texture.nzsl")
	add_headerfiles("../../assets/shaders/compute/compute_particles.nzsl")
