// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline BaseComponent::BaseComponent(ComponentId componentId) :
	m_componentId(componentId)
	{
	}

	inline ComponentId BaseComponent::GetId() const
	{
		return m_componentId;
	}
}
