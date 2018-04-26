// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Components/DebugComponent.hpp>

namespace Ndk
{
	inline DebugComponent::DebugComponent(DebugDrawFlags flags) :
	m_flags(flags)
	{
	}

	inline DebugComponent::DebugComponent(const DebugComponent& debug) :
	m_flags(debug.m_flags)
	{
	}

	inline void DebugComponent::Disable(DebugDrawFlags flags)
	{
		m_flags &= ~flags;

		if (m_entity)
			m_entity->Invalidate();
	}

	inline void DebugComponent::Enable(DebugDrawFlags flags)
	{
		m_flags |= flags;

		if (m_entity)
			m_entity->Invalidate();
	}

	inline DebugDrawFlags DebugComponent::GetFlags() const
	{
		return m_flags;
	}

	inline bool DebugComponent::IsEnabled(DebugDrawFlags flags) const
	{
		return (m_flags & flags) == flags;
	}

	inline DebugComponent& DebugComponent::operator=(const DebugComponent& debug)
	{
		m_flags = debug.m_flags;

		if (m_entity)
			m_entity->Invalidate();

		return *this;
	}

	inline const Nz::InstancedRenderableRef& DebugComponent::GetDebugRenderable(DebugDraw option) const
	{
		return m_debugRenderables[static_cast<std::size_t>(option)];
	}

	inline DebugDrawFlags DebugComponent::GetEnabledFlags() const
	{
		return m_enabledFlags;
	}

	inline void DebugComponent::UpdateDebugRenderable(DebugDraw option, Nz::InstancedRenderableRef renderable)
	{
		m_debugRenderables[static_cast<std::size_t>(option)] = std::move(renderable);
	}

	inline void DebugComponent::UpdateEnabledFlags(DebugDrawFlags flags)
	{
		m_enabledFlags = flags;
	}
}
