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
		links "NazaraRendererd-s"
		links "NazaraUtilityd-s"
		links "NazaraCored-s"

	configuration "ReleaseStatic"
		links "NazaraRenderer-s"
		links "NazaraUtility-s"
		links "NazaraCore-s"

	configuration "DebugDLL"
		links "NazaraRendererd"
		links "NazaraCored"
		links "NazaraUtilityd"

	configuration "ReleaseDLL"
		links "NazaraRenderer"
		links "NazaraUtility"
		links "NazaraCore"
end
