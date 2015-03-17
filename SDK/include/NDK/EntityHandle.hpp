// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_ENTITYHANDLE_HPP
#define NDK_ENTITYHANDLE_HPP

#include <NDK/Entity.hpp>
#include <ostream>

namespace Ndk
{
	class EntityHandle
	{
		friend Entity;

		public:
			EntityHandle();
			explicit EntityHandle(Entity* entity);
			EntityHandle(const EntityHandle& handle);
			EntityHandle(EntityHandle&& handle);
			~EntityHandle();

			Entity* GetEntity() const;

			bool IsValid() const;

			void Reset(Entity* entity = nullptr);
			void Reset(const EntityHandle& handle);
			void Reset(EntityHandle&& handle);

			EntityHandle& Swap(EntityHandle& handle);

			operator bool() const;
			operator Entity*() const;
			Entity* operator->() const;

			EntityHandle& operator=(Entity* entity);
			EntityHandle& operator=(const EntityHandle& handle);
			EntityHandle& operator=(EntityHandle&& handle);

			friend std::ostream& operator<<(std::ostream& out, const EntityHandle& handle);

			friend bool operator==(const EntityHandle& lhs, const EntityHandle& rhs);
			friend bool operator==(const Entity& lhs, const EntityHandle& rhs);
			friend bool operator==(const EntityHandle& lhs, const Entity& rhs);

			friend bool operator!=(const EntityHandle& lhs, const EntityHandle& rhs);
			friend bool operator!=(const Entity& lhs, const EntityHandle& rhs);
			friend bool operator!=(const EntityHandle& lhs, const Entity& rhs);

			friend bool operator<(const EntityHandle& lhs, const EntityHandle& rhs);
			friend bool operator<(const Entity& lhs, const EntityHandle& rhs);
			friend bool operator<(const EntityHandle& lhs, const Entity& rhs);

			friend bool operator<=(const EntityHandle& lhs, const EntityHandle& rhs);
			friend bool operator<=(const Entity& lhs, const EntityHandle& rhs);
			friend bool operator<=(const EntityHandle& lhs, const Entity& rhs);

			friend bool operator>(const EntityHandle& lhs, const EntityHandle& rhs);
			friend bool operator>(const Entity& lhs, const EntityHandle& rhs);
			friend bool operator>(const EntityHandle& lhs, const Entity& rhs);

			friend bool operator>=(const EntityHandle& lhs, const EntityHandle& rhs);
			friend bool operator>=(const Entity& lhs, const EntityHandle& rhs);
			friend bool operator>=(const EntityHandle& lhs, const Entity& rhs);

		private:
			void OnEntityDestroyed();
			void OnEntityMoved(Entity* newEntity);

			Entity* m_entity;
	};
}

namespace std
{
	void swap(Ndk::EntityHandle& lhs, Ndk::EntityHandle& rhs);
}

#include <NDK/EntityHandle.inl>

#endif // NDK_ENTITYHANDLE_HPP
