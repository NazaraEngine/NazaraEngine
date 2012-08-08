kind "ConsoleApp"

files "main.cpp"

configuration "DebugStatic"
	links "NazaraCored-s"
	links "NazaraRendererd-s"
	links "NazaraUtilityd-s"

configuration "ReleaseStatic"
	links "NazaraCore-s"
	links "NazaraRenderer-s"
	links "NazaraUtility-s"

configuration "DebugDLL"
	links "NazaraCored"
	links "NazaraRendererd"
	links "NazaraUtilityd"

configuration "ReleaseDLL"
	links "NazaraCore"
	links "NazaraRenderer"
	links "NazaraUtility"