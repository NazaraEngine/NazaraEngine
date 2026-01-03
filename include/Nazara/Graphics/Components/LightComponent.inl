// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <stdexcept>

namespace Nz
{
	inline LightComponent::LightComponent(bool initialyVisible) :
	m_isVisible(initialyVisible)
	{
	}

	template<typename T, typename... Args>
	T& LightComponent::AddLight(UInt32 renderMask, Args&&... args)
	{
		static_assert(std::is_base_of_v<Light, T>, "Type must inherit Light");

		for (std::size_t i = 0; i < m_lightEntries.size(); ++i)
		{
			auto& entry = m_lightEntries[i];
			if (entry.light)
				continue;

			entry.light = std::make_unique<T>(std::forward<Args>(args)...);
			entry.renderMask = renderMask;

			OnLightAttached(this, i);
			return static_cast<T&>(*entry.light);
		}

		throw std::runtime_error("too many lights attached to the same entity");
	}

	inline void LightComponent::Clear()
	{
		for (std::size_t i = 0; i < m_lightEntries.size(); ++i)
		{
			auto& entry = m_lightEntries[i];
			if (!entry.light)
				continue;

			OnLightDetach(this, i);
			entry.light.reset();
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

	inline void LightComponent::RemoveLight(std::size_t lightIndex)
	{
		assert(lightIndex < m_lightEntries.size());
		auto& entry = m_lightEntries[lightIndex];
		if (!entry.light)
			return;

		OnLightDetach(this, lightIndex);
		entry.light.reset();
	}

	inline void LightComponent::RemoveLight(const Light& light)
	{
		auto it = std::find_if(m_lightEntries.begin(), m_lightEntries.end(), [&](const auto& lightEntry) { return lightEntry.light.get() == &light; });
		if (it != m_lightEntries.end())
			RemoveLight(std::distance(m_lightEntries.begin(), it));
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
