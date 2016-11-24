// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_ENTITYLIST_HPP
#define NDK_ENTITYLIST_HPP

#include <Nazara/Core/Bitset.hpp>
#include <NDK/Prerequesites.hpp>
#include <NDK/Entity.hpp>

namespace Ndk
{
	class NDK_API EntityList
	{
		public:
			using Container = std::vector<EntityHandle>;

			EntityList() = default;
			~EntityList() = default;

			inline void Clear();

			inline bool Has(const Entity* entity);
			inline bool Has(EntityId entity);

			inline void Insert(Entity* entity);

			inline void Remove(Entity* entity);

			// STL API
			inline Container::iterator begin();
			inline Container::const_iterator begin() const;

			inline Container::const_iterator cbegin() const;
			inline Container::const_iterator cend() const;
			inline Container::const_reverse_iterator crbegin() const;
			inline Container::const_reverse_iterator crend() const;

			inline bool empty() const;

			inline Container::iterator end();
			inline Container::const_iterator end() const;

			inline Container::reverse_iterator rbegin();
			inline Container::const_reverse_iterator rbegin() const;

			inline Container::reverse_iterator rend();
			inline Container::const_reverse_iterator rend() const;

			inline Container::size_type size() const;

		private:
			std::vector<EntityHandle> m_entities;
			Nz::Bitset<Nz::UInt64> m_entityBits;
	};
}

#include <NDK/EntityList.inl>

#endif // NDK_ENTITYLIST_HPP
