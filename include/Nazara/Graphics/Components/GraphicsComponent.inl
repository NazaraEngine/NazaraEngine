// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline GraphicsComponent::GraphicsComponent(bool initialyVisible) :
	m_isVisible(initialyVisible)
	{
		m_worldInstance = std::make_shared<WorldInstance>(); //< FIXME: Use pools
	}

	inline void GraphicsComponent::AttachRenderable(std::shared_ptr<InstancedRenderable> renderable, UInt32 renderMask)
	{
		auto& entry = m_renderables.emplace_back();
		entry.renderable = std::move(renderable);
		entry.renderMask = renderMask;

		OnRenderableAttached(this, m_renderables.back());
	}

	inline void GraphicsComponent::Clear()
	{
		for (const auto& renderable : m_renderables)
			OnRenderableDetach(this, renderable);

		m_renderables.clear();
	}

	inline void GraphicsComponent::DetachRenderable(const std::shared_ptr<InstancedRenderable>& renderable)
	{
		auto it = std::find_if(m_renderables.begin(), m_renderables.end(), [&](const auto& renderableEntry) { return renderableEntry.renderable == renderable; });
		if (it != m_renderables.end())
		{
			OnRenderableDetach(this, *it);

			m_renderables.erase(it);
		}
	}

	inline auto GraphicsComponent::GetRenderables() const -> const std::vector<Renderable>&
	{
		return m_renderables;
	}

	inline const WorldInstancePtr& GraphicsComponent::GetWorldInstance() const
	{
		return m_worldInstance;
	}

	inline void GraphicsComponent::Hide()
	{
		return Show(false);
	}

	inline bool GraphicsComponent::IsVisible() const
	{
		return m_isVisible;
	}

	inline void GraphicsComponent::Show(bool show)
	{
		if (m_isVisible != show)
		{
			OnVisibilityUpdate(this, show);
			m_isVisible = show;
		}
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
