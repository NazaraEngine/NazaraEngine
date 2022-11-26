// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_COMPONENTS_LIGHTCOMPONENT_HPP
#define NAZARA_GRAPHICS_COMPONENTS_LIGHTCOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <entt/entt.hpp>
#include <array>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_GRAPHICS_API LightComponent
	{
		public:
			struct LightEntry;
			static constexpr std::size_t MaxLightCount = 8;

			inline LightComponent(bool initialyVisible = true);
			LightComponent(const LightComponent&) = default;
			LightComponent(LightComponent&&) = default;
			~LightComponent() = default;

			template<typename T, typename... Args> T& AddLight(UInt32 renderMask, Args&&... args);

			inline void Clear();

			inline const LightEntry& GetLightEntry(std::size_t lightIndex) const;
			inline const std::array<LightEntry, MaxLightCount>& GetLights() const;

			inline void Hide();

			inline bool IsVisible() const;

			inline void RemoveLight(std::size_t lightIndex);
			inline void RemoveLight(const Light& renderable);

			inline void Show(bool show = true);

			LightComponent& operator=(const LightComponent&) = default;
			LightComponent& operator=(LightComponent&&) = default;

			NazaraSignal(OnLightAttached, LightComponent* /*graphicsComponent*/, std::size_t /*lightIndex*/);
			NazaraSignal(OnLightDetach, LightComponent* /*graphicsComponent*/, std::size_t /*lightIndex*/);
			NazaraSignal(OnVisibilityUpdate, LightComponent* /*graphicsComponent*/, bool /*newVisibilityState*/);

			struct LightEntry
			{
				std::unique_ptr<Light> light;
				UInt32 renderMask;
			};

		private:
			std::array<LightEntry, MaxLightCount> m_lightEntries;
			bool m_isVisible;
	};
}

#include <Nazara/Graphics/Components/LightComponent.inl>

#endif // NAZARA_GRAPHICS_COMPONENTS_LIGHTCOMPONENT_HPP
