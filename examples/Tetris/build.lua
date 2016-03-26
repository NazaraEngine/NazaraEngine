EXAMPLE.Name = "Tetris"

EXAMPLE.Console = true

EXAMPLE.Files = {
    "main.cpp",
    "include/Components/BlockComponent.hpp",
    "include/Components/BlockComponent.inl",
    "include/Components/PartComponent.hpp",
    "include/Components/PartComponent.inl",
    "include/Constants.hpp",
    "include/States/MenuState.hpp",
    "include/States/GameState.hpp",
    "src/Components/BlockComponent.cpp",
    "src/Components/PartComponent.cpp",
    "src/States/MenuState.cpp",
    "src/States/GameState.cpp"
}

EXAMPLE.Includes = {
    "../examples/" .. EXAMPLE.Name .. "/include"
}

EXAMPLE.Libraries = {
    "NazaraCore",
    "NazaraGraphics",
    "NazaraRenderer",
    "NazaraUtility",
    "NazaraSDK"
}