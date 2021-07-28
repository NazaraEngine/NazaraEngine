package("nodeeditor")

    set_homepage("https://github.com/paceholder/nodeeditor")
    set_description("Qt Node Editor. Dataflow programming framework")
    set_license("BSD-3")

    set_urls("https://github.com/paceholder/nodeeditor/archive/refs/tags/$(version).tar.gz",
             "https://github.com/paceholder/nodeeditor.git")
    add_versions("2.1.3", "4e3194a04ac4a2a2bf4bc8eb6cc27d5cc154923143c1ecf579ce7f0115a90585")
    add_patches("2.1.3", path.join(os.scriptdir(), "patches", "2.1.3", "fix_qt.patch"), "3192c66fe711ad4bbfba96348601655396bc32465e2807f5be252cde6e2a3d59")

    add_deps("cmake", "qt5core", "qt5gui", "qt5widgets")

    on_load(function (package)
        if package:config("shared") then
            package:add("defines", "NODE_EDITOR_SHARED")
        else
            package:add("defines", "NODE_EDITOR_STATIC")
        end
    end)

    on_install("windows", "linux", "mingw", "macosx", function (package)
        local qt = package:dep("qt5core"):fetch().qtdir

        local configs = {"-DBUILD_EXAMPLES=OFF", "-DBUILD_TESTING=OFF"}
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        if qt then
            table.insert(configs, "-DQt5_DIR=" .. path.join(qt.libdir, "cmake", "Qt5"))
        end
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test() {
                QtNodes::FlowScene scene(std::make_shared<QtNodes::DataModelRegistry>());
                QtNodes::FlowView view(&scene);
            }
        ]]}, {configs = {languages = "c++11"}, includes = {"nodes/FlowScene", "nodes/FlowView"}}))
    end)
