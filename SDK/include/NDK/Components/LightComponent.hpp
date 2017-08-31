// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_COMPONENTS_LIGHTCOMPONENT_HPP
#define NDK_COMPONENTS_LIGHTCOMPONENT_HPP

#include <Nazara/Graphics/Light.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	class NDK_API LightComponent : public Component<LightComponent>, public Nz::Light
	{
		public:
			inline LightComponent(Nz::LightType lightType = Nz::LightType_Point);
			LightComponent(const LightComponent& light) = default;
			~LightComponent() = default;

			LightComponent& operator=(const LightComponent& light) = default;

			static ComponentIndex componentIndex;
	};
}

#include <NDK/Components/LightComponent.inl>

#endif // NDK_COMPONENTS_LIGHTCOMPONENT_HPP
#endif // NDK_SERVER
