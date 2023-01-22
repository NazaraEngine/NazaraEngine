// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AppEntitySystemComponent.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	void AppEntitySystemComponent::Update(Time elapsedTime)
	{
		m_systemGraph.Update(elapsedTime);
	}
}
