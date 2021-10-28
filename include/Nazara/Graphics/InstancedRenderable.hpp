// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_INSTANCEDRENDERABLE_HPP
#define NAZARA_GRAPHICS_INSTANCEDRENDERABLE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/Box.hpp>
#include <memory>

namespace Nz
{
	class CommandBufferBuilder;
	class Material;
	class RenderElement;
	class WorldInstance;

	class NAZARA_GRAPHICS_API InstancedRenderable
	{
		public:
			inline InstancedRenderable(const Boxf& aabb);
			InstancedRenderable(const InstancedRenderable&) = delete;
			InstancedRenderable(InstancedRenderable&&) noexcept = default;
			~InstancedRenderable();

			virtual void BuildElement(std::size_t passIndex, const WorldInstance& worldInstance, std::vector<std::unique_ptr<RenderElement>>& elements) const = 0;

			inline const Boxf& GetAABB() const;
			virtual const std::shared_ptr<Material>& GetMaterial(std::size_t i) const = 0;
			virtual std::size_t GetMaterialCount() const = 0;

			InstancedRenderable& operator=(const InstancedRenderable&) = delete;
			InstancedRenderable& operator=(InstancedRenderable&&) noexcept = default;

			NazaraSignal(OnMaterialInvalidated, InstancedRenderable* /*instancedRenderable*/, std::size_t /*materialIndex*/, const std::shared_ptr<Material>& /*newMaterial*/);

		private:
			Boxf m_aabb;
	};
}

#include <Nazara/Graphics/InstancedRenderable.inl>

#endif // NAZARA_GRAPHICS_INSTANCEDRENDERABLE_HPP
