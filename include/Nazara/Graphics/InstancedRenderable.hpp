// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INSTANCEDRENDERABLE_HPP
#define NAZARA_INSTANCEDRENDERABLE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <memory>

namespace Nz
{
	class CommandBufferBuilder;
	class MaterialPass;
	class WorldInstance;

	class NAZARA_GRAPHICS_API InstancedRenderable
	{
		public:
			InstancedRenderable() = default;
			InstancedRenderable(const InstancedRenderable&) = delete;
			InstancedRenderable(InstancedRenderable&&) noexcept = default;
			~InstancedRenderable();

			virtual void Draw(CommandBufferBuilder& commandBuffer) const = 0;

			virtual const std::shared_ptr<MaterialPass>& GetMaterial(std::size_t i) const = 0;
			virtual std::size_t GetMaterialCount() const = 0;

			InstancedRenderable& operator=(const InstancedRenderable&) = delete;
			InstancedRenderable& operator=(InstancedRenderable&&) noexcept = default;

			NazaraSignal(OnMaterialInvalidated, InstancedRenderable* /*instancedRenderable*/, std::size_t /*materialIndex*/, const std::shared_ptr<MaterialPass>& /*newMaterial*/);
	};
}

#include <Nazara/Graphics/InstancedRenderable.inl>

#endif
