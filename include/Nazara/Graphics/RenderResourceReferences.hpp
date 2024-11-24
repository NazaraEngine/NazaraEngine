// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERRESOURCEREFERENCES_HPP
#define NAZARA_GRAPHICS_RENDERRESOURCEREFERENCES_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Thirdparty/ankerl/unordered_dense.h>
#include <memory>

namespace Nz
{
	class RenderBuffer;
	class Texture;
	class TextureSampler;

	struct RenderResourceReferences
	{
		// Required for unordered_dense::set::try_emplace to not instantiate the key
		struct TransparentHash
		{
			using is_transparent = void;

			template<typename Key>
			std::size_t operator()(const Key& key) const
			{
				return std::hash<Key>{}(key);
			}
		};

		inline void Clear();

		ankerl::unordered_dense::set<std::shared_ptr<RenderBuffer>, TransparentHash, std::equal_to<>> renderBuffers;
		ankerl::unordered_dense::set<std::shared_ptr<Texture>, TransparentHash, std::equal_to<>> textures;
		ankerl::unordered_dense::set<std::shared_ptr<TextureSampler>, TransparentHash, std::equal_to<>> samplers;
	};
}

#include <Nazara/Graphics/RenderResourceReferences.inl>

#endif // NAZARA_GRAPHICS_RENDERRESOURCEREFERENCES_HPP
