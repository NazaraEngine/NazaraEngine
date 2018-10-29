// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/EntityOwner.hpp>
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
	* \brief Release the ownership of the entity without killing it
	*/
	inline void EntityOwner::Release()
	{
		EntityHandle::Reset(nullptr);
	}

	/*!
	* \brief Resets the ownership of the entity, previous is killed
	*
	* \param entity Entity to own
	*/
	inline void EntityOwner::Reset(Entity* entity)
	{
		if (IsValid())
			GetObject()->Kill();

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
		handle.Release();
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

	/*!
	* \brief Steals ownership of a EntityOwner
	*
	* \param handle Handle to the new entity to own, or an invalid handle
	*/
	inline EntityOwner& EntityOwner::operator=(EntityOwner&& handle) noexcept
	{
		Reset(); //< Kill previously owned entity, if any

		EntityHandle::operator=(std::move(handle));

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
