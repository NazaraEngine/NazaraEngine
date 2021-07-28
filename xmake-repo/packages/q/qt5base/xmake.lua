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

    -- TODO: on_install using aqtinstall

    on_test(function (package)
        local qt = package:data("qtdir")
        os.vrun(path.join(qt.bindir, "moc", " -v"))
        os.vrun(path.join(qt.bindir, "rcc", " -v"))
        os.vrun(path.join(qt.bindir, "uic", " -v"))
    end)
