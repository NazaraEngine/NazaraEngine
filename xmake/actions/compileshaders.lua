task("compile-shaders")

    set_menu({
        -- Settings menu usage
        usage = "xmake compile-shaders [options]",
        description = "Compile engine shaders"
    })

    on_run(function ()
        import("core.project.task")

        print("Compiling NZSLC...")
        task.run("build", {target = "NazaraShaderCompiler"})

        print("Compiling shaders...")
        for _, filepath in pairs(os.files("src/Nazara/*/Resources/**.nzsl")) do
            print(" - Compiling " .. filepath)
            local argv = {"--compile", "--partial", "--header-file", path.join("..", "..", filepath) }
            task.run("run", {target = "NazaraShaderCompiler", arguments = argv})
        end
    end)