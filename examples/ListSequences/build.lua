kind "ConsoleApp"

files "main.cpp"

if (_OPTIONS["one-library"]) then
	configuration "DebugStatic"
		links "Nazara-s-d"

	configuration "ReleaseStatic"
		links "Nazara-s"

	configuration "DebugDLL"
		links "Nazara-d"

	configuration "ReleaseDLL"
		links "Nazara"
else
	configuration "DebugStatic"
		links "NazaraCore-s-d"
		links "NazaraUtility-s-d"

	configuration "ReleaseStatic"
		links "NazaraCore-s"
		links "NazaraUtility-s"

	configuration "DebugDLL"
		links "NazaraCore-d"
		links "NazaraUtility-d"

	configuration "ReleaseDLL"
		links "NazaraCore"
		links "NazaraUtility"
end
