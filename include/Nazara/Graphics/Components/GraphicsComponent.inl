// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline GraphicsComponent::GraphicsComponent(bool initiallyVisible) :
	m_scissorBox(-1, -1, -1, -1),
	m_isVisible(initiallyVisible)
	{
		m_worldInstance = std::make_shared<WorldInstance>(); //< FIXME: Use pools
	}

	inline GraphicsComponent::GraphicsComponent(std::shared_ptr<InstancedRenderable> renderable, UInt32 renderMask, bool initiallyVisible) :
	GraphicsComponent(initiallyVisible)
	{
		AttachRenderable(std::move(renderable), renderMask);
	}

	inline void GraphicsComponent::AttachRenderable(std::shared_ptr<InstancedRenderable> renderable, UInt32 renderMask)
	{
		for (std::size_t i = 0; i < m_renderables.size(); ++i)
		{
			auto& entry = m_renderables[i];
			if (entry.renderable)
				continue;

			entry.renderable = std::move(renderable);
			entry.renderMask = renderMask;

			OnRenderableAttached(this, i);
			break;
		}
	}

	inline void GraphicsComponent::Clear()
	{
		for (std::size_t i = 0; i < m_renderables.size(); ++i)
		{
			auto& entry = m_renderables[i];
			if (!entry.renderable)
				continue;

			OnRenderableDetach(this, i);
			entry.renderable.reset();
		}
	}

	inline void GraphicsComponent::DetachRenderable(const std::shared_ptr<InstancedRenderable>& renderable)
	{
		auto it = std::find_if(m_renderables.begin(), m_renderables.end(), [&](const auto& renderableEntry) { return renderableEntry.renderable == renderable; });
		if (it != m_renderables.end())
		{
			OnRenderableDetach(this, std::distance(m_renderables.begin(), it));

			it->renderable.reset();
		}
	}

	inline Boxf GraphicsComponent::GetAABB() const
	{
		Boxf aabb = Boxf::Invalid();
		for (std::size_t i = 0; i < m_renderables.size(); ++i)
		{
			auto& entry = m_renderables[i];
			if (!entry.renderable)
				continue;

			if (i > 0)
				aabb.ExtendTo(m_renderables[i].renderable->GetAABB());
			else
				aabb = m_renderables[i].renderable->GetAABB();
		}

		return aabb;
	}

	inline auto GraphicsComponent::GetRenderableEntry(std::size_t renderableIndex) const -> const Renderable&
	{
		assert(renderableIndex < m_renderables.size());
		return m_renderables[renderableIndex];
	}

	inline auto GraphicsComponent::GetRenderables() const -> const std::array<Renderable, MaxRenderableCount>&
	{
		return m_renderables;
	}

	inline const Recti& GraphicsComponent::GetScissorBox() const
	{
		return m_scissorBox;
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

	inline void GraphicsComponent::UpdateScissorBox(const Recti& scissorBox)
	{
		if (m_scissorBox != scissorBox)
		{
			OnScissorBoxUpdate(this, scissorBox);
			m_scissorBox = scissorBox;
		}
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
