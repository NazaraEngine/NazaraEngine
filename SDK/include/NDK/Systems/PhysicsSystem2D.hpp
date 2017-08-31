// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SYSTEMS_PHYSICSSYSTEM2D_HPP
#define NDK_SYSTEMS_PHYSICSSYSTEM2D_HPP

#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <NDK/EntityList.hpp>
#include <NDK/System.hpp>
#include <memory>

namespace Ndk
{
	class NDK_API PhysicsSystem2D : public System<PhysicsSystem2D>
	{
		public:
			PhysicsSystem2D();
			PhysicsSystem2D(const PhysicsSystem2D& system);
			~PhysicsSystem2D() = default;

			Nz::PhysWorld2D& GetWorld();
			const Nz::PhysWorld2D& GetWorld() const;

			static SystemIndex systemIndex;

		private:
			void CreatePhysWorld() const;
			void OnEntityValidation(Entity* entity, bool justAdded) override;
			void OnUpdate(float elapsedTime) override;

			EntityList m_dynamicObjects;
			EntityList m_staticObjects;
			mutable std::unique_ptr<Nz::PhysWorld2D> m_world; ///TODO: std::optional (Should I make a Nz::Optional class?)
	};
}

#include <NDK/Systems/PhysicsSystem2D.inl>

#endif // NDK_SYSTEMS_PHYSICSSYSTEM2D_HPP
