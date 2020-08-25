-- This file contains special configurations values, such as directories to extern libraries (Qt)
-- Editing this file is not required to use/compile the engine, as default values should be enough

-- Additionnal compilation options
--AdditionalCompilationOptions = "-fsanitize=address" -- Enable ASan

-- Builds Nazara extern libraries (such as lua/STB)
BuildDependencies = true

-- Builds Nazara examples
BuildExamples = true

-- Setup configurations array (valid values: Debug, Release, ReleaseWithDebug) 
Configurations = "Debug,Release,ReleaseWithDebug" -- "Debug,Release,ReleaseWithDebug"

-- Setup additionnals install directories, separated by a semi-colon ; (library binaries will be copied there)
--InstallDir = "/usr/local/lib64"

-- Adds a project which will recall premake with its original arguments when built (only works on Windows for now)
PremakeProject = false

-- Excludes client-only modules/tools/examples
ServerMode = false

-- Builds modules as one united library (useless on POSIX systems)
UniteModules = false

-- Qt5 directories (required for ShaderNodes editor)
--Qt5IncludeDir = [[C:\Projets\Libs\Qt\5.15.0\msvc2019\include]]
--Qt5BinDir_x86 = [[C:\Projets\Libs\Qt\5.15.0\msvc2019\bin]]
--Qt5BinDir_x64 = [[C:\Projets\Libs\Qt\5.15.0\msvc2019_64\bin]]
--Qt5LibDir_x86 = [[C:\Projets\Libs\Qt\5.15.0\msvc2019\lib]]
--Qt5LibDir_x64 = [[C:\Projets\Libs\Qt\5.15.0\msvc2019_64\lib]]


-- QtNodes directories (required for ShaderNodes editor)
--QtNodesIncludeDir = [[C:\Projets\Libs\nodeeditor\include]]
--QtNodesBinDir_x86 = [[C:\Projets\Libs\nodeeditor\build32\bin\Release]]
--QtNodesBinDir_x64 = [[C:\Projets\Libs\nodeeditor\build64\bin\Release]]
--QtNodesLibDir_x86 = [[C:\Projets\Libs\nodeeditor\build32\lib\Release]]
--QtNodesLibDir_x64 = [[C:\Projets\Libs\nodeeditor\build64\lib\Release]]