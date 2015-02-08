// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_ENTITY_HPP
#define NDK_ENTITY_HPP

#include <NDK/Prerequesites.hpp>

namespace Ndk
{
	class World;

	class NDK_API Entity
	{
		friend World;

		public:
			class Id;

			Entity();
			Entity(const Entity&) = default;
			~Entity() = default;

			void Kill();

			Id GetId() const;
			World* GetWorld() const;

			bool IsValid() const;

			Entity& operator=(const Entity&) = default;

			bool operator==(const Entity& other) const;
			bool operator!=(const Entity& other) const;

			// Identifiant
			struct Id
			{
				struct Part
				{
					nzUInt32 counter, index;
				};

				union
				{
					Part part;
					nzUInt64 value;
				};

				bool operator==(const Id& other) const;
				bool operator!=(const Id& other) const;
			};

		private:
			Entity(Id id, World* world);

			Id m_id;
			World* m_world;
	};
}

#include <NDK/Entity.inl>

#endif // NDK_ENTITY_HPP
