// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_ENTITYLIST_HPP
#define NDK_ENTITYLIST_HPP

#include <Nazara/Core/Bitset.hpp>
#include <NDK/Prerequesites.hpp>
#include <NDK/EntityHandle.hpp>

namespace Ndk
{
	class NDK_API EntityList
	{
		public:
			using Container = std::vector<EntityHandle>;

			EntityList() = default;
			~EntityList() = default;

			void Clear();

			bool Has(const Entity* entity);
			bool Has(EntityId entity);

			void Insert(Entity* entity);

			void Remove(Entity* entity);

			// Interface STD
			Container::iterator begin();
			Container::const_iterator begin() const;

			Container::const_iterator cbegin() const;
			Container::const_iterator cend() const;
			Container::const_reverse_iterator crbegin() const;
			Container::const_reverse_iterator crend() const;

			bool empty() const;

			Container::iterator end();
			Container::const_iterator end() const;

			Container::reverse_iterator rbegin();
			Container::const_reverse_iterator rbegin() const;

			Container::reverse_iterator rend();
			Container::const_reverse_iterator rend() const;

			Container::size_type size() const;

		private:
			std::vector<EntityHandle> m_entities;
			NzBitset<nzUInt64> m_entityBits;
	};
}

#include <NDK/EntityList.inl>

#endif // NDK_ENTITYLIST_HPP
