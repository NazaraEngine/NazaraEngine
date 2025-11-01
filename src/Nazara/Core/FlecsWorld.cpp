// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/FlecsWorld.hpp>

namespace Nz
{
	FlecsWorld::~FlecsWorld()
	{
		m_systemGraph.Clear();
	}

	void FlecsWorld::Update(Time elapsedTime)
	{
		m_world.progress(1.0f);
		m_systemGraph.Update(elapsedTime);
	}
}
