LIBRARY.Name = "stb_image"

LIBRARY.Defines = {
	"STBI_NO_STDIO"
}

LIBRARY.Language = "C++" -- On compile en C++ car le C99 n'est pas supporté partout

LIBRARY.Files = {
	"../extlibs/include/stb/*.h",
	"../extlibs/src/stb/*.cpp"
}
