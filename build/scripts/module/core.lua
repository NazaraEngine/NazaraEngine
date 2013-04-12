if (not _OPTIONS["united"]) then
	project "NazaraCore"
end

files
{
	"../include/Nazara/Prerequesites.hpp",
	"../include/Nazara/Core/**.hpp",
	"../include/Nazara/Core/**.inl",
	"../include/Nazara/Math/**.hpp",
	"../include/Nazara/Math/**.inl",
	"../src/Nazara/Core/**.hpp",
	"../src/Nazara/Core/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/Core/Posix/**.hpp", "../src/Nazara/Core/Posix/**.cpp" }
else
	excludes { "../src/Nazara/Core/Win32/**.hpp", "../src/Nazara/Core/Win32/**.cpp" }
end
