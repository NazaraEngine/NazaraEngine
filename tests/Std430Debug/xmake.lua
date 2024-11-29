target("Std430Debug")
	add_files("main.cpp")

	add_packages("opengl-headers")

	if has_config("embed_rendererbackends", "static") then
		add_deps("NazaraRenderer")
	else
		add_deps("NazaraOpenGLRenderer")
	end
