// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_BASECOMPONENT_HPP
#define NDK_BASECOMPONENT_HPP

#include <NDK/Prerequesites.hpp>
#include <functional>
#include <unordered_map>
#include <vector>

namespace Ndk
{
	class NDK_API BaseComponent
	{
		public:
			using Factory = std::function<BaseComponent*()>;

			BaseComponent(ComponentIndex componentIndex);
			virtual ~BaseComponent();

			virtual BaseComponent* Clone() const = 0;

			ComponentIndex GetIndex() const;

			template<typename ComponentType, unsigned int N>
			static ComponentIndex Register(const char (&name)[N]);

			static ComponentIndex Register(ComponentId id, Factory factoryFunc);

		protected:
			ComponentIndex m_componentIndex;

		private:
			struct ComponentEntry
			{
				ComponentId id;
				Factory factory;
			};

			static std::vector<ComponentEntry> s_entries;
			static std::unordered_map<ComponentId, ComponentIndex> s_idToIndex;
	};
}

#include <NDK/BaseComponent.inl>

#endif // NDK_BASECOMPONENT_HPP
