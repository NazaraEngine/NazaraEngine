-- This file contains special configurations values, such as directories to extern libraries (Qt)
-- Editing this file is not required to use/compile the engine, as default values should be enough

-- Builds Nazara extern libraries (such as lua/STB)
BuildDependencies = true

-- Builds Nazara examples
BuildExamples = true

-- Setup additionnals install directories, separated by a semi-colon ; (library binaries will be copied there)
--InstallDir = "/usr/local/lib64"

-- Excludes client-only modules/tools/examples
ServerMode = false

-- Builds modules as one united library (useless on POSIX systems)
UniteModules = false
