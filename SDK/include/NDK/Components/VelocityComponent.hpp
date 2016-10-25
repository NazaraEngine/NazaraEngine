// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_VELOCITYCOMPONENT_HPP
#define NDK_COMPONENTS_VELOCITYCOMPONENT_HPP

#include <Nazara/Math/Vector3.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	class VelocityComponent;

	using VelocityComponentHandle = Nz::ObjectHandle<VelocityComponent>;

	class NDK_API VelocityComponent : public Component<VelocityComponent>, public Nz::HandledObject<VelocityComponent>
	{
		public:
			VelocityComponent(const Nz::Vector3f& velocity = Nz::Vector3f::Zero());
			~VelocityComponent() = default;

			Nz::Vector3f linearVelocity;

			VelocityComponent& operator=(const Nz::Vector3f& vel);

			static ComponentIndex componentIndex;
	};
}

#include <NDK/Components/VelocityComponent.inl>

#endif // NDK_COMPONENTS_VELOCITYCOMPONENT_HPP
