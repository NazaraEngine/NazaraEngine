// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <algorithm>

namespace Ndk
{
	inline void GraphicsComponent::AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const NzMatrix4f& transformMatrix) const
	{
		for (const Renderable& object : m_renderables)
			object.renderable->AddToRenderQueue(renderQueue, transformMatrix);
	}

	inline void GraphicsComponent::Attach(NzRenderableRef renderable)
	{
		m_renderables.resize(m_renderables.size() + 1);
		Renderable& r = m_renderables.back();
		r.renderable = std::move(renderable);
	}
}
