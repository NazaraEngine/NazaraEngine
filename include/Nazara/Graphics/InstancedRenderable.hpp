// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_INSTANCEDRENDERABLE_HPP
#define NAZARA_GRAPHICS_INSTANCEDRENDERABLE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/RenderElementOwner.hpp>
#include <Nazara/Math/Box.hpp>
#include <NazaraUtils/Signal.hpp>
#include <memory>

namespace Nz
{
	class CommandBufferBuilder;
	class ElementRendererRegistry;
	class MaterialInstance;
	class RenderElement;
	class SkeletonInstance;
	class WorldInstance;

	class NAZARA_GRAPHICS_API InstancedRenderable
	{
		public:
			struct ElementData;

			inline InstancedRenderable();
			InstancedRenderable(const InstancedRenderable&) = delete;
			InstancedRenderable(InstancedRenderable&&) noexcept = default;
			virtual ~InstancedRenderable();

			virtual void BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const = 0;

			inline const Boxf& GetAABB() const;
			virtual const std::shared_ptr<MaterialInstance>& GetMaterial(std::size_t materialIndex) const = 0;
			virtual std::size_t GetMaterialCount() const = 0;
			inline int GetRenderLayer() const;

			inline void UpdateRenderLayer(int renderLayer);

			InstancedRenderable& operator=(const InstancedRenderable&) = delete;
			InstancedRenderable& operator=(InstancedRenderable&&) noexcept = default;

			NazaraSignal(OnAABBUpdate, InstancedRenderable* /*instancedRenderable*/, const Boxf& /*aabb*/);
			NazaraSignal(OnElementInvalidated, InstancedRenderable* /*instancedRenderable*/);
			NazaraSignal(OnMaterialInvalidated, InstancedRenderable* /*instancedRenderable*/, std::size_t /*materialIndex*/, const std::shared_ptr<MaterialInstance>& /*newMaterial*/);

			struct ElementData
			{
				const Recti* scissorBox;
				const SkeletonInstance* skeletonInstance;
				const WorldInstance* worldInstance;
			};

		protected:
			inline void UpdateAABB(Boxf aabb);

		private:
			Boxf m_aabb;
			int m_renderLayer;
	};
}

#include <Nazara/Graphics/InstancedRenderable.inl>

#endif // NAZARA_GRAPHICS_INSTANCEDRENDERABLE_HPP
