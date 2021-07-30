package("qt5core")

    set_homepage("https://www.qt.io")
    set_description("Qt is the faster, smarter way to create innovative devices, modern UIs & applications for multiple screens. Cross-platform software development at its best.")
    set_license("LGPL-3")

    on_load(function (package)
        package:add("deps", "qt5base", {debug = package:is_debug()})
    end)

    on_fetch(function (package)
        local base = package:dep("qt5base")
        local qt = base:data("qtdir")
        if not qt then
            return
        end

        return {
            qtdir = qt,
            links = table.wrap("Qt5Core" .. (package:is_plat("windows") and package:is_debug() and "d" or "")),
            linkdirs = table.wrap(qt.libdir),
            includedirs = table.wrap(qt.includedir)
        }
    end)

    on_install(function (package)
        local base = package:dep("qt5base")
        local qt = base:data("qtdir")
        assert(qt, "qt5base is required")
    end)

    on_test(function (package)
    
    end)
