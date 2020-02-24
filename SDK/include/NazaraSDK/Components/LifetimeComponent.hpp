// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_COMPONENTS_LIFETIMECOMPONENT_HPP
#define NDK_COMPONENTS_LIFETIMECOMPONENT_HPP

#include <NazaraSDK/Component.hpp>

namespace Ndk
{
	class LifetimeComponent;

	using LifetimeComponentHandle = Nz::ObjectHandle<LifetimeComponent>;

	class NDK_API LifetimeComponent : public Component<LifetimeComponent>
	{
		friend class LifetimeSystem;

		public:
			inline LifetimeComponent(float lifetime);
			LifetimeComponent(const LifetimeComponent&) = default;
			~LifetimeComponent() = default;

			inline float GetRemainingTime() const;

			static ComponentIndex componentIndex;

		private:
			inline bool UpdateLifetime(float elapsedTime);

			float m_lifetime;
	};
}

#include <NazaraSDK/Components/LifetimeComponent.inl>

#endif // NDK_COMPONENTS_LIFETIMECOMPONENT_HPP
