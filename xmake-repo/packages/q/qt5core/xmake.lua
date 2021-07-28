package("qt5core")

    set_homepage("https://www.qt.io")
    set_description("Qt is the faster, smarter way to create innovative devices, modern UIs & applications for multiple screens. Cross-platform software development at its best.")
    set_license("LGPL-3")

    add_deps("qt5base", {public=true})

    on_fetch(function (package)
        local base = package:dep("qt5base")
        local qt = base:data("qtdir")

        return {
            qtdir = qt,
            links = table.wrap("Qt5Core" .. (package:is_debug() and "d" or "")),
            linkdirs = table.wrap(qt.libdir),
            includedirs = table.wrap(qt.includedir)
        }
    end)
