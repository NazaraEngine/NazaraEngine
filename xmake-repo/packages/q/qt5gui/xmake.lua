package("qt5gui")

    set_homepage("https://www.qt.io")
    set_description("Qt is the faster, smarter way to create innovative devices, modern UIs & applications for multiple screens. Cross-platform software development at its best.")
    set_license("LGPL-3")

    add_deps("qt5base", "qt5core", "qt5core")

    on_fetch(function (package)
        local base = package:dep("qt5base")
        local qt = base:data("qtdir")

        return {
            links = table.wrap("Qt5Gui" .. (package:is_debug() and "d" or "")),
            linkdirs = table.wrap(qt.libdir),
            includedirs = table.wrap(qt.includedir)
        }
    end)
