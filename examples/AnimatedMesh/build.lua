kind "ConsoleApp"

files "main.cpp"

if (_OPTIONS["one-library"]) then
	configuration "DebugStatic"
		links "NazaraEngine-s-d"

	configuration "ReleaseStatic"
		links "NazaraEngine-s"

	configuration "DebugDLL"
		links "NazaraEngine-d"

	configuration "ReleaseDLL"
		links "NazaraEngine"
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
