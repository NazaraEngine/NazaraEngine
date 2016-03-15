// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <NDK/Components/ListenerComponent.hpp>

namespace Ndk
{
	bool ListenerComponent::Serialize(Nz::SerializationContext& context) const
	{
		bool isActive = IsActive();
		if (!Nz::Serialize(context, isActive))
			return false;

		return true;
	}

	bool ListenerComponent::Unserialize(Nz::SerializationContext& context)
	{
		bool isActive;
		if (!Nz::Unserialize(context, &isActive))
			return false;

		SetActive(isActive);

		return true;
	}

	ComponentIndex ListenerComponent::componentIndex;
}
