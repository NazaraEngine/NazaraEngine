#pragma once

#ifndef TET_INITIALIZEGAME_HPP
#define TET_INITIALIZEGAME_HPP

namespace Ndk { class World; }
namespace Nz { class RenderWindow; }

namespace Tet {
	namespace Utils {
		inline bool InitializeGame(Nz::RenderWindow& window, Ndk::World& world);
	}
}

#include "Utils/InitializeGame.inl"

#endif // TET_INITIALIZEGAME_HPP