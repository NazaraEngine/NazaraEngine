// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_COMPONENTS_LIGHTCOMPONENT_HPP
#define NAZARA_GRAPHICS_COMPONENTS_LIGHTCOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_GRAPHICS_API LightComponent
	{
		public:
			struct LightEntry;

			inline LightComponent(bool initialyVisible = true);
			LightComponent(const LightComponent&) = default;
			LightComponent(LightComponent&&) = default;
			~LightComponent() = default;

			inline void AttachLight(std::shared_ptr<Light> renderable, UInt32 renderMask);

			inline void Clear();

			inline void DetachLight(const std::shared_ptr<Light>& renderable);

			inline const std::vector<LightEntry>& GetLights() const;

			inline void Hide();

			inline bool IsVisible() const;

			inline void Show(bool show = true);

			LightComponent& operator=(const LightComponent&) = default;
			LightComponent& operator=(LightComponent&&) = default;

			NazaraSignal(OnLightAttached, LightComponent* /*graphicsComponent*/, const LightEntry& /*lightEntry*/);
			NazaraSignal(OnLightDetach, LightComponent* /*graphicsComponent*/, const LightEntry& /*lightEntry*/);
			NazaraSignal(OnVisibilityUpdate, LightComponent* /*graphicsComponent*/, bool /*newVisibilityState*/);

			struct LightEntry
			{
				std::shared_ptr<Light> light;
				UInt32 renderMask;
			};

		private:
			std::vector<LightEntry> m_lightEntries;
			bool m_isVisible;
	};
}

#include <Nazara/Graphics/Components/LightComponent.inl>

#endif // NAZARA_GRAPHICS_COMPONENTS_LIGHTCOMPONENT_HPP
