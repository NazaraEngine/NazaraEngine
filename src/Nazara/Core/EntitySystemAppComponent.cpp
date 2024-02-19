// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/EntitySystemAppComponent.hpp>

namespace Nz
{
	void EntitySystemAppComponent::Update(Time elapsedTime)
	{
		for (auto& worldPtr : m_worlds)
			worldPtr->Update(elapsedTime);
	}
}
