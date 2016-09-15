// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Canvas.hpp>

namespace Ndk
{
	inline Canvas::Canvas(WorldHandle world, Nz::EventHandler& eventHandler) :
	m_hoveredWidget(nullptr),
	m_world(std::move(world))
	{
		m_canvas = this;

		m_mouseMovedSlot.Connect(eventHandler.OnMouseMoved, this, &Canvas::OnMouseMoved);
	}

	inline const WorldHandle& Canvas::GetWorld() const
	{
		return m_world;
	}
}
