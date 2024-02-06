#include <Nazara/Audio.hpp>
#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Network.hpp>
#include <Nazara/ChipmunkPhysics2D.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	// This "example" has only one purpose: Giving an empty project for you to test whatever you want
	// If you wish to have multiple test projects, you only have to copy/paste this directory and change the name in the xmake.lua
	Nz::Application<Nz::Audio, Nz::Core, Nz::Graphics, Nz::Network, Nz::ChipmunkPhysics2D, Nz::Renderer, Nz::Utility> app(argc, argv);

	return EXIT_SUCCESS;
}
