// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_COMPONENTS_LISTENERCOMPONENT_HPP
#define NDK_COMPONENTS_LISTENERCOMPONENT_HPP

#include <NDK/Component.hpp>

namespace Ndk
{
	class ListenerComponent;

	using ListenerComponentHandle = Nz::ObjectHandle<ListenerComponent>;

	class NDK_API ListenerComponent : public Component<ListenerComponent>
	{
		public:
			inline ListenerComponent();
			~ListenerComponent() = default;

			inline bool IsActive() const;
			inline void SetActive(bool active = true);

			static ComponentIndex componentIndex;

		private:
			bool m_isActive;
	};
}

#include <NDK/Components/ListenerComponent.inl>

#endif // NDK_COMPONENTS_LISTENERCOMPONENT_HPP
#endif // NDK_SERVER