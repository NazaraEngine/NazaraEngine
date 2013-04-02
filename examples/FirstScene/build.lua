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
		links "NazaraGraphics-s-d"
		links "NazaraRenderer-s-d"
		links "NazaraUtility-s-d"
		links "NazaraCore-s-d"

	configuration "ReleaseStatic"
		links "NazaraGraphics-s"
		links "NazaraRenderer-s"
		links "NazaraUtility-s"
		links "NazaraCore-s"

	configuration "DebugDLL"
		links "NazaraGraphics-d"
		links "NazaraRenderer-d"
		links "NazaraUtility-d"
		links "NazaraCore-d"

	configuration "ReleaseDLL"
		links "NazaraGraphics"
		links "NazaraRenderer"
		links "NazaraUtility"
		links "NazaraCore"
end
