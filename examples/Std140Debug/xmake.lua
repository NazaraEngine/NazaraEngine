target("Std140Debug")
	add_files("main.cpp")

	if has_config("embed_rendererbackends") then
		add_deps("NazaraRenderer")
	else
		add_deps("NazaraOpenGLRenderer")
	end