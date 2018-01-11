LIBRARY.Name = "stb_image"

LIBRARY.Defines = {
	"STBI_NO_STDIO"
}

LIBRARY.Language = "C++" -- Compile as C++ because C99 isn't widely supported

LIBRARY.Files = {
	"../thirdparty/include/stb/*.h",
	"../thirdparty/src/stb/*.cpp"
}
