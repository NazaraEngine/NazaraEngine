kind "ConsoleApp"

files "main.cpp"

if (_OPTIONS["one-library"]) then
	configuration "DebugStatic"
		links "Nazarad-s"

	configuration "ReleaseStatic"
		links "Nazara-s"

	configuration "DebugDLL"
		links "Nazarad"

	configuration "ReleaseDLL"
		links "Nazara"
else
	configuration "DebugStatic"
		links "NazaraCored-s"
		links "NazaraUtilityd-s"

	configuration "ReleaseStatic"
		links "NazaraCore-s"
		links "NazaraUtility-s"

	configuration "DebugDLL"
		links "NazaraCored"
		links "NazaraUtilityd"

	configuration "ReleaseDLL"
		links "NazaraCore"
		links "NazaraUtility"
end
