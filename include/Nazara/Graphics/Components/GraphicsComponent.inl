// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline GraphicsComponent::GraphicsComponent()
	{
		m_worldInstance = std::make_shared<WorldInstance>(); //< FIXME: Use pools
	}

	inline void GraphicsComponent::AttachRenderable(std::shared_ptr<InstancedRenderable> renderable)
	{
		m_renderables.push_back(std::move(renderable));

		OnRenderableAttached(this, m_renderables.back());
	}

	inline void GraphicsComponent::DetachRenderable(const std::shared_ptr<InstancedRenderable>& renderable)
	{
		auto it = std::find(m_renderables.begin(), m_renderables.end(), renderable);
		if (it != m_renderables.end())
		{
			OnRenderableDetach(this, renderable);

			m_renderables.erase(it);
		}
	}

	inline const std::vector<std::shared_ptr<InstancedRenderable>>& GraphicsComponent::GetRenderables() const
	{
		return m_renderables;
	}

	inline const WorldInstancePtr& GraphicsComponent::GetWorldInstance() const
	{
		return m_worldInstance;
	}
}
