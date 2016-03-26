#pragma once

#ifndef TET_PARTCOMPONENT_HPP
#define TET_PARTCOMPONENT_HPP

#include <NDK/Component.hpp>
#include <NDK/EntityHandle.hpp>
#include <array>
#include <vector>

namespace Tet {
	class PartComponent : public Ndk::Component<PartComponent> {
	public:
		template<std::size_t x, std::size_t y>
		using PartForm = std::array<bool, x*y>;

		inline PartComponent(const PartForm<3, 2>& partForm = {0,0,0,0,0,0}, const std::vector<Ndk::EntityHandle>& partBlocks = {});
		~PartComponent() = default;

		PartForm<3, 2> form;
		std::vector<Ndk::EntityHandle> blocks;

        static Ndk::ComponentIndex componentIndex;
	};
}

#include "Components/PartComponent.inl"

#endif // TET_PARTCOMPONENT_HPP