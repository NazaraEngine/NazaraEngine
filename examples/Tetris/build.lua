EXAMPLE.Name = "Tetris"

EXAMPLE.Console = true

EXAMPLE.Files = {
    "main.cpp",
    "include/States/MenuState.hpp",
    "include/States/GameState.hpp",
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