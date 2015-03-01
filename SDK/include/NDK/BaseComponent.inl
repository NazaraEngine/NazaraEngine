// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline BaseComponent::BaseComponent(nzUInt32 componentId) :
	m_componentId(componentId)
	{
	}

	inline nzUInt32 BaseComponent::GetId() const
	{
		return m_componentId;
	}

	inline nzUInt32 BaseComponent::GetNextId()
	{
		return s_nextId++;
	}
}
