#ifndef __STATECONTEXT_HPP__
#define __STATECONTEXT_HPP__

namespace Ndk
{
	class World;
}

namespace Nz
{
	class RenderWindow;
}

class StateContext
{
	public:
		StateContext(Nz::RenderWindow& window, Ndk::World& world);

		Nz::RenderWindow& window;
		Ndk::World& world;
};

#endif // __STATECONTEXT_HPP__
