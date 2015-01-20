LIBRARY.Name = "stb_image"

LIBRARY.Defines = {
	"STBI_NO_STDIO"
}

LIBRARY.Flags = {
	"EnableSSE2"
}

LIBRARY.Language = "C++" -- On compile en C++ car le C99 n'est pas supporté partout

LIBRARY.Files = {
	"../extlibs/include/stb_image/stb_image.h",
	"../extlibs/src/stb_image/stb_image.c"
}
