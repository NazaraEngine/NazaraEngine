#include <Nazara/Audio.hpp>
#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Lua.hpp>
#include <Nazara/Network.hpp>
#include <Nazara/Noise.hpp>
#include <Nazara/Physics2D.hpp>
#include <Nazara/Physics3D.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <NDK/Application.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	// This "example" has only one purpose: Giving an empty project for you to test whatever you want
	// If you wish to have multiple test projects, you only have to copy/paste this directory and change the name in the build.lua
	Ndk::Application application(argc, argv);

	// Do what you want here
	Nz::LuaInstance lua;
	std::cout << lua.Execute("return {key = 42}") << std::endl;
	std::cout << lua.DumpStack() << std::endl;

	return EXIT_SUCCESS;
}
