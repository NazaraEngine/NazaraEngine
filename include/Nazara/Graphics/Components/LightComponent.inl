// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Components/LightComponent.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline LightComponent::LightComponent(bool initialyVisible) :
	m_isVisible(initialyVisible)
	{
	}

	inline void LightComponent::AttachLight(std::shared_ptr<Light> light, UInt32 renderMask)
	{
		for (std::size_t i = 0; i < m_lightEntries.size(); ++i)
		{
			auto& entry = m_lightEntries[i];
			if (entry.light)
				continue;

			entry.light = std::move(light);
			entry.renderMask = renderMask;

			OnLightAttached(this, i);
			break;
		}
	}

	inline void LightComponent::Clear()
	{
		for (std::size_t i = 0; i < m_lightEntries.size(); ++i)
		{
			auto& entry = m_lightEntries[i];
			if (entry.light)
				continue;

			OnLightDetach(this, i);
			entry.light.reset();
		}
	}

	inline void LightComponent::DetachLight(const std::shared_ptr<Light>& light)
	{
		auto it = std::find_if(m_lightEntries.begin(), m_lightEntries.end(), [&](const auto& lightEntry) { return lightEntry.light == light; });
		if (it != m_lightEntries.end())
		{
			OnLightDetach(this, std::distance(m_lightEntries.begin(), it));

			it->light.reset();
		}
	}

	inline auto LightComponent::GetLightEntry(std::size_t lightIndex) const -> const LightEntry&
	{
		assert(lightIndex < m_lightEntries.size());
		return m_lightEntries[lightIndex];
	}

	inline auto LightComponent::GetLights() const -> const std::array<LightEntry, MaxLightCount>&
	{
		return m_lightEntries;
	}

	inline void LightComponent::Hide()
	{
		return Show(false);
	}

	inline bool LightComponent::IsVisible() const
	{
		return m_isVisible;
	}

	inline void LightComponent::Show(bool show)
	{
		if (m_isVisible != show)
		{
			OnVisibilityUpdate(this, show);
			m_isVisible = show;
		}
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
