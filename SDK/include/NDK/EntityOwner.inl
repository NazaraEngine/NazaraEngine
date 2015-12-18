// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/StringStream.hpp>
#include <functional>
#include <limits>

namespace Ndk
{
	inline EntityOwner::EntityOwner(Entity* entity) :
	EntityOwner()
	{
		Reset(entity);
	}

	inline EntityOwner::EntityOwner(const EntityOwner& handle) :
	EntityHandle(handle)
	{
	}

	inline EntityOwner::EntityOwner(EntityOwner&& handle) :
	EntityHandle(std::move(handle))
	{
	}

	inline EntityOwner::~EntityOwner()
	{
		Reset(nullptr);
	}

	inline void EntityOwner::Reset(Entity* entity)
	{
		if (m_entity)
			m_entity->Kill();

		EntityHandle::Reset(entity);
	}

	inline void EntityOwner::Reset(const EntityOwner& handle)
	{
		Reset(handle.GetEntity());
	}

	inline void EntityOwner::Reset(EntityOwner&& handle)
	{
		Reset(handle.GetEntity());
	}

	inline EntityOwner& EntityOwner::operator=(Entity* entity)
	{
		Reset(entity);

		return *this;
	}

	inline EntityOwner& EntityOwner::operator=(const EntityOwner& handle)
	{
		Reset(handle);

		return *this;
	}

	inline EntityOwner& EntityOwner::operator=(EntityOwner&& handle)
	{
		Reset(std::move(handle));

		return *this;
	}
}

namespace std
{
	template<>
	struct hash<Ndk::EntityOwner> : public hash<Ndk::EntityHandle>
	{ 
	};
}
