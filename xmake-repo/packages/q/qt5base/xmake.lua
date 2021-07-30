package("qt5base")

    set_kind("binary")
    set_homepage("https://www.qt.io")
    set_description("Qt is the faster, smarter way to create innovative devices, modern UIs & applications for multiple screens. Cross-platform software development at its best.")
    set_license("LGPL-3")

    on_load(function (package)
        -- I think find_qt could be moved here
        import("detect.sdks.find_qt")
        local qt = find_qt()
        if qt then
            package:data_set("qtdir", qt)
        else
            package:add("deps", "python >=3.6", "7z")
        end
    end)

    on_fetch(function (package)
        local qt = package:data("qtdir")
        if not qt then
            return
        end

        return {
            qtdir = qt,
        }
    end)

    on_install(function (package)
        os.vrunv("python", {"-m", "pip", "install", "-U", "pip"})
        os.vrunv("python", {"-m", "pip", "install", "aqtinstall"})

        local installdir = package:installdir()
        local version = "5.15.2"
        local host = package:is_plat("windows") and "windows" or "linux"
        local target = "desktop"
        local arch = package:is_plat("windows") and "win64_msvc2019_64" or nil

        os.vrunv("python", {"-m", "aqt", "install", "--outputdir", installdir, version, host, target, arch})

        -- move files to root
        local installeddir = path.join(installdir, version, package:is_plat("windows") and "msvc2019_64" or "gcc_64", "*")
        os.mv(installeddir, installdir)
        os.rmdir(path.join(installdir, version)

        package:data_set("qtdir", {
            bindir = path.join(installdir, "bin"),
            includedir = path.join(installdir, "include"),
            libdir = path.join(installdir, "lib")
        })
    end)

    on_test(function (package)
        local qt = package:data("qtdir")
        os.vrun(path.join(qt.bindir, "moc") .. " -v")
        os.vrun(path.join(qt.bindir, "rcc") .. " -v")
        os.vrun(path.join(qt.bindir, "uic") .. " -v")
    end)
