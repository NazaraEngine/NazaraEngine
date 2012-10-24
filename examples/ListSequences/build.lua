kind "ConsoleApp"

files "main.cpp"

if (_OPTIONS["united"]) then
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
		links "NazaraUtility-s-d"
		links "NazaraCore-s-d"

	configuration "ReleaseStatic"
		links "NazaraUtility-s"
		links "NazaraCore-s"

	configuration "DebugDLL"
		links "NazaraUtility-d"
		links "NazaraCore-d"

	configuration "ReleaseDLL"
		links "NazaraUtility"
		links "NazaraCore"
end
