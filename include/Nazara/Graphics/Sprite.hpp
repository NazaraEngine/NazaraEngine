// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPRITE_HPP
#define NAZARA_SPRITE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <array>

namespace Nz
{
	class NAZARA_GRAPHICS_API Sprite : public InstancedRenderable
	{
		public:
			Sprite(std::shared_ptr<Material> material);
			Sprite(const Sprite&) = delete;
			Sprite(Sprite&&) noexcept = default;
			~Sprite() = default;

			void BuildElement(std::size_t passIndex, const WorldInstance& worldInstance, std::vector<std::unique_ptr<RenderElement>>& elements) const override;

			const std::shared_ptr<Material>& GetMaterial(std::size_t i) const;
			std::size_t GetMaterialCount() const;

			inline void SetMaterial(std::shared_ptr<Material> material);

			Sprite& operator=(const Sprite&) = delete;
			Sprite& operator=(Sprite&&) noexcept = default;

		private:
			std::array<VertexStruct_XYZ_Color_UV, 4> m_vertices;
			std::shared_ptr<Material> m_material;
	};
}

#include <Nazara/Graphics/Sprite.inl>

#endif
