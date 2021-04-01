// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_ENTITYLIST_HPP
#define NDK_ENTITYLIST_HPP

#include <Nazara/Core/Bitset.hpp>
#include <NDK/Prerequisites.hpp>
#include <NDK/Entity.hpp>

namespace Ndk
{
	class NDK_API EntityList
	{
		friend Entity;
		friend World;

		public:
			class iterator;
			friend iterator;
			using size_type = std::size_t;

			inline EntityList();
			EntityList(const EntityList& entityList);
			EntityList(EntityList&& entityList) noexcept;
			~EntityList();

			void Clear();

			inline bool Has(const Entity* entity) const;
			inline bool Has(EntityId entity) const;

			void Insert(Entity* entity);

			inline void Remove(Entity* entity);
			inline void Reserve(std::size_t entityCount);

			// STL API
			inline iterator begin() const;
			inline bool empty() const;
			inline iterator end() const;
			inline size_type size() const;

			EntityList& operator=(const EntityList& entityList);
			EntityList& operator=(EntityList&& entityList) noexcept;

		private:
			inline std::size_t FindNext(std::size_t currentId) const;
			inline World* GetWorld() const;
			inline void NotifyEntityDestruction(const Entity* entity);
			inline void SetWorld(World* world);

			Nz::Bitset<Nz::UInt64> m_entityBits;
			World* m_world;
	};

	class NDK_API EntityList::iterator
	{
		friend EntityList;

		public:
			inline iterator(const iterator& it);

			const EntityHandle& operator*() const;

			inline iterator& operator=(const iterator& it);
			inline iterator& operator++();
			inline iterator operator++(int);

			friend inline bool operator==(const iterator& lhs, const iterator& rhs);
			friend inline bool operator!=(const iterator& lhs, const iterator& rhs);

			friend inline void swap(iterator& lhs, iterator& rhs);

			using difference_type = std::ptrdiff_t;
			using iterator_category = std::forward_iterator_tag;
			using pointer = EntityHandle*;
			using reference = EntityHandle&;
			using value_type = EntityHandle;

		private:
			inline iterator(const EntityList* world, std::size_t nextId);

			std::size_t m_nextEntityId;
			const EntityList* m_list;
	};
}

#include <NDK/EntityList.inl>

#endif // NDK_ENTITYLIST_HPP
