// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline InstancedRenderable::InstancedRenderable() :
	m_aabb(Boxf::Zero())
	{
	}

	inline const Boxf& InstancedRenderable::GetAABB() const
	{
		return m_aabb;
	}

	inline void InstancedRenderable::UpdateAABB(Boxf aabb)
	{
		OnAABBUpdate(this, aabb);
		m_aabb = aabb;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
