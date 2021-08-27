package("qt5widgets")

    set_homepage("https://www.qt.io")
    set_description("Qt is the faster, smarter way to create innovative devices, modern UIs & applications for multiple screens. Cross-platform software development at its best.")
    set_license("LGPL-3")

    on_load(function (package)
        package:add("deps", "qt5base", "qt5core", "qt5gui", {debug = package:is_debug()})
    end)

    on_fetch(function (package)
        local base = package:dep("qt5base")
        local qt = base:data("qtdir")
        if not qt then
            return
        end

        return {
            includedirs = {qt.includedir, path.join(qt.includedir, "QtWidgets")},
            links = table.wrap("Qt5Widgets" .. (package:is_plat("windows") and package:is_debug() and "d" or "")),
            linkdirs = table.wrap(qt.libdir)
        }
    end)

    on_install(function (package)
        local base = package:dep("qt5base")
        local qt = base:data("qtdir")
        assert(qt, "qt5base is required")
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            int test(int argc, char** argv) {
                QApplication app (argc, argv);

                QPushButton button ("Hello world !");
                button.show();

                return app.exec();
            }
        ]]}, {configs = {languages = "c++14", cxflags = not package:is_plat("windows") and "-fPIC" or nil}, includes = {"QApplication", "QPushButton"}}))
    end)
