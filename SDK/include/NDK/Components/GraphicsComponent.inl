// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline void GraphicsComponent::AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const NzMatrix4f& transformMatrix) const
	{
		for (const Renderable& object : m_renderables)
			object.renderable->AddToRenderQueue(renderQueue, transformMatrix);
	}
}
