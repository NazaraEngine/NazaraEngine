// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENT_HPP
#define NDK_COMPONENT_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/BaseComponent.hpp>

namespace Ndk
{
	template<typename ComponentType>
	class Component : public BaseComponent
	{
		public:
			Component();
			virtual ~Component();

			BaseComponent* Clone() const override;
	};

	template<typename ComponentType> constexpr nzUInt32 GetComponentId();
}

#include <NDK/Component.inl>

#endif // NDK_COMPONENT_HPP
