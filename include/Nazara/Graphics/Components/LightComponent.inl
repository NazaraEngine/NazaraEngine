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
		auto& entry = m_lightEntries.emplace_back();
		entry.light = std::move(light);
		entry.renderMask = renderMask;

		OnLightAttached(this, m_lightEntries.back());
	}

	inline void LightComponent::Clear()
	{
		for (const auto& lightEntry : m_lightEntries)
			OnLightDetach(this, lightEntry);

		m_lightEntries.clear();
	}

	inline void LightComponent::DetachLight(const std::shared_ptr<Light>& light)
	{
		auto it = std::find_if(m_lightEntries.begin(), m_lightEntries.end(), [&](const auto& lightEntry) { return lightEntry.light == light; });
		if (it != m_lightEntries.end())
		{
			OnLightDetach(this, *it);

			m_lightEntries.erase(it);
		}
	}

	inline auto LightComponent::GetLights() const -> const std::vector<LightEntry>&
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
