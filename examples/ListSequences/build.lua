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
