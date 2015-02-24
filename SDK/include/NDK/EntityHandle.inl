// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <functional>

namespace Ndk
{
	inline EntityHandle::EntityHandle() :
	m_entity(nullptr)
	{
	}

	inline EntityHandle::EntityHandle(Entity* entity) :
	EntityHandle()
	{
		Reset(entity);
	}

	inline EntityHandle::EntityHandle(const EntityHandle& handle) :
	EntityHandle()
	{
		Reset(handle);
	}

	inline EntityHandle::EntityHandle(EntityHandle&& handle) :
	EntityHandle()
	{
		Reset(handle);
	}

	inline EntityHandle::~EntityHandle()
	{
		Reset(nullptr);
	}

	inline Entity* EntityHandle::GetEntity() const
	{
		return m_entity;
	}

	inline bool EntityHandle::IsValid() const
	{
		return m_entity != nullptr;
	}

	inline void EntityHandle::Reset(Entity* entity)
	{
		// Si nous avions déjà une entité, nous devons l'informer que nous ne pointons plus sur elle
		if (m_entity)
			m_entity->UnregisterHandle(this);

		m_entity = entity;
		if (m_entity)
			// On informe la nouvelle entité que nous pointons sur elle
			m_entity->RegisterHandle(this);
	}

	inline void EntityHandle::Reset(const EntityHandle& handle)
	{
		Reset(handle.GetEntity());
	}

	inline void EntityHandle::Reset(EntityHandle&& handle)
	{
		Reset(handle.GetEntity());
	}

	inline EntityHandle& EntityHandle::Swap(EntityHandle& handle)
	{
		std::swap(m_entity, handle.m_entity);
	}

	inline EntityHandle::operator bool() const
	{
		return IsValid();
	}

	inline EntityHandle::operator Entity*() const
	{
		return m_entity;
	}

	inline Entity* EntityHandle::operator->() const
	{
		return m_entity;
	}

	inline EntityHandle& EntityHandle::operator=(Entity* entity)
	{
		Reset(entity);
	}

	inline EntityHandle& EntityHandle::operator=(const EntityHandle& handle)
	{
		Reset(handle);
	}

	inline EntityHandle& EntityHandle::operator=(EntityHandle&& handle)
	{
		Reset(handle);
	}

	inline void EntityHandle::OnEntityDestroyed()
	{
		// Un raccourci, un appel à Reset nous enlèverait de la liste des handles que nous ne pouvons pas modifier
		// maintenant car elle est actuellement parcourue
		m_entity = nullptr;
	}

	inline void EntityHandle::OnEntityMoved(Entity* newEntity)
	{
		// L'entité a été déplacée (peut arriver lors d'un changement de taille du conteneur du monde)
		// nous mettons à jour notre pointeur
		m_entity = newEntity;
	}

	inline std::ostream& operator<<(std::ostream& out, const EntityHandle& handle)
	{
		out << "EntityHandle(";
		if (handle.IsValid())
			out << "Entity(" << handle->GetId() << ")";
		else
			out << "Null entity";

		out << ')';

		return out;
	}

	inline bool operator==(const EntityHandle& lhs, const EntityHandle& rhs)
	{
		return lhs.m_entity == rhs.m_entity;
	}

	inline bool operator!=(const EntityHandle& lhs, const EntityHandle& rhs)
	{
		return !(lhs == rhs);
	}

	inline bool operator<(const EntityHandle& lhs, const EntityHandle& rhs)
	{
		return lhs.m_entity < rhs.m_entity;
	}

	inline bool operator<=(const EntityHandle& lhs, const EntityHandle& rhs)
	{
		return !(lhs > rhs);
	}

	inline bool operator>(const EntityHandle& lhs, const EntityHandle& rhs)
	{
		return rhs < lhs;
	}

	inline bool operator>=(const EntityHandle& lhs, const EntityHandle& rhs)
	{
		return !(lhs < rhs);
	}
}

namespace std
{
    template<>
    struct hash<Ndk::EntityHandle>
    {
		size_t operator()(const Ndk::EntityHandle& handle) const
		{
			return hash<Ndk::Entity*>()(handle.GetEntity());
		}
    };

    inline void swap(Ndk::EntityHandle& lhs, Ndk::EntityHandle& rhs)
	{
		lhs.Swap(rhs);
	}
}
