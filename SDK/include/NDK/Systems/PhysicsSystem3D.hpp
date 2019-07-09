// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SYSTEMS_PHYSICSSYSTEM3D_HPP
#define NDK_SYSTEMS_PHYSICSSYSTEM3D_HPP

#include <Nazara/Physics3D/PhysWorld3D.hpp>
#include <NDK/EntityList.hpp>
#include <NDK/System.hpp>
#include <memory>

namespace Ndk
{
	class NDK_API PhysicsSystem3D : public System<PhysicsSystem3D>
	{
		public:
			PhysicsSystem3D();
			~PhysicsSystem3D() = default;

			Nz::PhysWorld3D& GetWorld();
			const Nz::PhysWorld3D& GetWorld() const;

			static SystemIndex systemIndex;

		private:
			void CreatePhysWorld() const;
			void OnEntityValidation(Entity* entity, bool justAdded) override;
			void OnUpdate(float elapsedTime) override;

			EntityList m_dynamicObjects;
			EntityList m_staticObjects;
			mutable std::unique_ptr<Nz::PhysWorld3D> m_world; ///TODO: std::optional (Should I make a Nz::Optional class?)
	};
}

#include <NDK/Systems/PhysicsSystem3D.inl>

#endif // NDK_SYSTEMS_PHYSICSSYSTEM3D_HPP
