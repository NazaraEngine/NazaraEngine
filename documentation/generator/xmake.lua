add_requires("cppast", "nlohmann_json")

target("docgen", function ()
    set_rundir("../")
    add_files("src/*.cpp")
    add_deps("NazaraCore")
    add_packages("cppast", "nazarautils", "nlohmann_json")
end)
