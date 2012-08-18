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
		links "NazaraRenderer-s-d"
		links "NazaraUtility-s-d"
		links "NazaraCore-s-d"

	configuration "ReleaseStatic"
		links "NazaraRenderer-s"
		links "NazaraUtility-s"
		links "NazaraCore-s"

	configuration "DebugDLL"
		links "NazaraRenderer-d"
		links "NazaraCore-d"
		links "NazaraUtility-d"

	configuration "ReleaseDLL"
		links "NazaraRenderer"
		links "NazaraUtility"
		links "NazaraCore"
end
