// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Components/DebugComponent.hpp>
#include <NazaraSDK/Components/GraphicsComponent.hpp>

namespace Ndk
{
	void DebugComponent::DetachDebugRenderables(GraphicsComponent& gfxComponent)
	{
		for (auto& renderable : m_debugRenderables)
		{
			if (renderable)
			{
				gfxComponent.Detach(renderable);
				renderable.Reset();
			}
		}
	}

	void DebugComponent::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<GraphicsComponent>(component))
			DetachDebugRenderables(static_cast<GraphicsComponent&>(component));
	}

	void DebugComponent::OnDetached()
	{
		if (m_entity->HasComponent<GraphicsComponent>())
			DetachDebugRenderables(m_entity->GetComponent<GraphicsComponent>());
	}

	ComponentIndex DebugComponent::componentIndex;
}
