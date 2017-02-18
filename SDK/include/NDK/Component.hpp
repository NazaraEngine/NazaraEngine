// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENT_HPP
#define NDK_COMPONENT_HPP

#include <NDK/BaseComponent.hpp>

namespace Ndk
{
	template<typename ComponentType>
	class Component : public BaseComponent
	{
		public:
			Component();
			virtual ~Component();

			std::unique_ptr<BaseComponent> Clone() const override;

			static ComponentIndex RegisterComponent(ComponentId id);

			template<unsigned int N>
			static ComponentIndex RegisterComponent(const char (&name)[N]);
	};
}

#include <NDK/Component.inl>

#endif // NDK_COMPONENT_HPP
