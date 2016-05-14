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

	inline EntityOwner::~EntityOwner()
	{
		Reset(nullptr);
	}

	inline void EntityOwner::Reset(Entity* entity)
	{
		if (m_object)
			m_object->Kill();

		EntityHandle::Reset(entity);
	}

	inline void EntityOwner::Reset(EntityOwner&& handle)
	{
		Reset(handle.GetObject());
		handle.m_object = nullptr;
	}

	inline EntityOwner& EntityOwner::operator=(Entity* entity)
	{
		Reset(entity);

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
