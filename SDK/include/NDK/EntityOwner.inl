// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Core/StringStream.hpp>
#include <functional>
#include <limits>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::EntityOwner
	* \brief NDK class that represents the owner of the entity and so its lifetime
	*/

	/*!
	* \brief Constructs a EntityOwner object
	*
	* \param entity Entity to own
	*/

	inline EntityOwner::EntityOwner(Entity* entity) :
	EntityOwner()
	{
		Reset(entity);
	}

	/*!
	* \brief Destructs the object and calls Reset
	*
	* \see Reset
	*/

	inline EntityOwner::~EntityOwner()
	{
		Reset(nullptr);
	}

	/*!
	* \brief Resets the ownership of the entity, previous is killed
	*
	* \param entity Entity to own
	*/

	inline void EntityOwner::Reset(Entity* entity)
	{
		if (m_object)
			m_object->Kill();

		EntityHandle::Reset(entity);
	}

	/*!
	* \brief Resets the ownership of the entity by move semantic
	*
	* \param handle EntityOwner to move into this
	*/

	inline void EntityOwner::Reset(EntityOwner&& handle)
	{
		Reset(handle.GetObject());
		handle.m_object = nullptr;
	}

	/*!
	* \brief Resets the ownership of the entity to the affected one
	*
	* \param entity Entity to own
	*/

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
