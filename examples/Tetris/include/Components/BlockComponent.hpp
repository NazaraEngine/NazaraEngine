#pragma once

#ifndef TET_BLOCKCOMPONENT_HPP
#define TET_BLOCKCOMPONENT_HPP

#include <Nazara/Renderer/Texture.hpp>
#include <NDK/Component.hpp>

namespace Nz { class Color; }

namespace Tet {
	class BlockComponent : public Ndk::Component<BlockComponent> {
	public:
		BlockComponent() = default;
		inline BlockComponent(const Nz::Color& blockColor);
		inline BlockComponent(const Nz::TextureRef& blockTexture);
		~BlockComponent() = default;

		Nz::TextureRef texture;

		inline const Nz::TextureRef& GenerateTextureFromColor(const Nz::Color& blockColor);

		inline BlockComponent& operator=(const Nz::Color& blockColor);
		inline BlockComponent& operator=(const Nz::TextureRef& blockTexture);
        
        static Ndk::ComponentIndex componentIndex;
	};
}

#include "Components/BlockComponent.inl"

#endif // TET_BLOCKCOMPONENT_HPP