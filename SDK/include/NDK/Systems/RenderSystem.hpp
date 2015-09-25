// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SYSTEMS_RENDERSYSTEM_HPP
#define NDK_SYSTEMS_RENDERSYSTEM_HPP

#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <NDK/EntityList.hpp>
#include <NDK/System.hpp>
#include <unordered_map>
#include <vector>

namespace Ndk
{
	class GraphicsComponent;

	class NDK_API RenderSystem : public System<RenderSystem>
	{
		public:
			RenderSystem();
			inline RenderSystem(const RenderSystem& renderSystem);
			~RenderSystem() = default;

			inline const Nz::BackgroundRef& GetDefaultBackground() const;
			inline const Nz::Matrix4f& GetCoordinateSystemMatrix() const;
			inline Nz::Vector3f GetGlobalForward() const;
			inline Nz::Vector3f GetGlobalRight() const;
			inline Nz::Vector3f GetGlobalUp() const;

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

			EntityList m_cameras;
			EntityList m_drawables;
			EntityList m_lights;
			Nz::BackgroundRef m_background;
			Nz::ForwardRenderTechnique m_renderTechnique;
			Nz::Matrix4f m_coordinateSystemMatrix;
			bool m_coordinateSystemInvalidated;
	};
}

#include <NDK/Systems/RenderSystem.inl>

#endif // NDK_SYSTEMS_RENDERSYSTEM_HPP
