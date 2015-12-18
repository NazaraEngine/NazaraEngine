// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_ENTITYOWNER_HPP
#define NDK_ENTITYOWNER_HPP

#include <NDK/EntityHandle.hpp>

namespace Ndk
{
	class EntityOwner : public EntityHandle
	{
		public:
			EntityOwner() = default;
			explicit EntityOwner(Entity* entity);
			EntityOwner(const EntityOwner& handle);
			EntityOwner(EntityOwner&& handle);
			~EntityOwner();

			void Reset(Entity* entity = nullptr);
			void Reset(const EntityOwner& handle);
			void Reset(EntityOwner&& handle);

			EntityOwner& operator=(Entity* entity);
			EntityOwner& operator=(const EntityOwner& handle);
			EntityOwner& operator=(EntityOwner&& handle);
	};
}

#include <NDK/EntityOwner.inl>

#endif // NDK_ENTITYOwner_HPP
