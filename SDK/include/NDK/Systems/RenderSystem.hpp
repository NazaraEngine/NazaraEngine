// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_SYSTEMS_RENDERSYSTEM_HPP
#define NDK_SYSTEMS_RENDERSYSTEM_HPP

#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/CullingList.hpp>
#include <Nazara/Graphics/DepthRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Renderer/RenderTexture.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/EntityList.hpp>
#include <NDK/System.hpp>
#include <unordered_map>
#include <vector>

namespace Ndk
{
	class NDK_API RenderSystem : public System<RenderSystem>
	{
		public:
			RenderSystem();
			inline RenderSystem(const RenderSystem& renderSystem);
			~RenderSystem() = default;

			template<typename T> T& ChangeRenderTechnique();
			inline Nz::AbstractRenderTechnique& ChangeRenderTechnique(std::unique_ptr<Nz::AbstractRenderTechnique>&& renderTechnique);

			inline const Nz::BackgroundRef& GetDefaultBackground() const;
			inline const Nz::Matrix4f& GetCoordinateSystemMatrix() const;
			inline Nz::Vector3f GetGlobalForward() const;
			inline Nz::Vector3f GetGlobalRight() const;
			inline Nz::Vector3f GetGlobalUp() const;
			inline Nz::AbstractRenderTechnique& GetRenderTechnique() const;

			inline void SetDefaultBackground(Nz::BackgroundRef background);
			inline void SetGlobalForward(const Nz::Vector3f& direction);
			inline void SetGlobalRight(const Nz::Vector3f& direction);
			inline void SetGlobalUp(const Nz::Vector3f& direction);

			static SystemIndex systemIndex;

		private:
			inline void InvalidateCoordinateSystem();

			void OnEntityRemoved(Entity* entity) override;
			void OnEntityValidation(Entity* entity, bool justAdded) override;
			void OnUpdate(float elapsedTime) override;

			void UpdateDirectionalShadowMaps(const Nz::AbstractViewer& viewer);
			void UpdatePointSpotShadowMaps();

			std::unique_ptr<Nz::AbstractRenderTechnique> m_renderTechnique;
			std::vector<GraphicsComponentCullingList::VolumeEntry> m_volumeEntries;
			std::vector<EntityHandle> m_cameras;
			EntityList m_drawables;
			EntityList m_directionalLights;
			EntityList m_lights;
			EntityList m_pointSpotLights;
			EntityList m_particleGroups;
			GraphicsComponentCullingList m_drawableCulling;
			Nz::BackgroundRef m_background;
			Nz::DepthRenderTechnique m_shadowTechnique;
			Nz::Matrix4f m_coordinateSystemMatrix;
			Nz::RenderTexture m_shadowRT;
			bool m_coordinateSystemInvalidated;
			bool m_forceRenderQueueInvalidation;
	};
}

#include <NDK/Systems/RenderSystem.inl>

#endif // NDK_SYSTEMS_RENDERSYSTEM_HPP
#endif // NDK_SERVER
