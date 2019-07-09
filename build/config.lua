-- This file contains special configurations values, such as directories to extern libraries (Qt)
-- Editing this file is not required to use/compile the engine, as default values should be enough

-- Additionnal compilation options
--AdditionalCompilationOptions = "-fsanitize=address" -- Enable ASan

-- Builds Nazara extern libraries (such as lua/STB)
BuildDependencies = true

-- Builds Nazara examples
BuildExamples = true

-- Setup configurations array (valid values: Debug, Release, ReleaseWithDebug) 
Configurations = "Debug,Release" -- "Debug,Release,ReleaseWithDebug"

-- Setup additionnals install directories, separated by a semi-colon ; (library binaries will be copied there)
--InstallDir = "/usr/local/lib64"

-- Adds a project which will recall premake with its original arguments when built (only works on Windows for now)
PremakeProject = false

-- Excludes client-only modules/tools/examples
ServerMode = false

-- Builds modules as one united library (useless on POSIX systems)
UniteModules = false
