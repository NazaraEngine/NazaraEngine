// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline ListenerComponent::ListenerComponent() :
	m_isActive(true)
	{
	}

	inline bool ListenerComponent::IsActive() const
	{
		return m_isActive;
	}

	inline void ListenerComponent::SetActive(bool active)
	{
		m_isActive = active;
	}
}
