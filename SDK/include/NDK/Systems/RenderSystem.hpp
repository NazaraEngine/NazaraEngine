// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SYSTEMS_RENDERSYSTEM_HPP
#define NDK_SYSTEMS_RENDERSYSTEM_HPP

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
			~RenderSystem() = default;

			void Update(float elapsedTime);

			static SystemIndex systemIndex;

		private:
			void OnEntityRemoved(Entity* entity) override;
			void OnEntityValidation(Entity* entity, bool justAdded) override;

			EntityList m_cameras;
			EntityList m_drawables;
	};
}

#include <NDK/Systems/RenderSystem.inl>

#endif // NDK_SYSTEMS_RENDERSYSTEM_HPP
