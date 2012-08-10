kind "ConsoleApp"

files "main.cpp"

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