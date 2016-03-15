// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/VelocityComponent.hpp>

namespace Ndk
{
	bool VelocityComponent::Serialize(Nz::SerializationContext& context) const
	{
		if (!Nz::Serialize(context, linearVelocity))
			return false;

		return true;
	}

	bool VelocityComponent::Unserialize(Nz::SerializationContext& context)
	{
		if (!Nz::Unserialize(context, &linearVelocity))
			return false;

		return true;
	}

	ComponentIndex VelocityComponent::componentIndex;
}
