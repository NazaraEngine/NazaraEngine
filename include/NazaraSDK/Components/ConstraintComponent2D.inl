// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Components/ConstraintComponent2D.hpp>
#include <NazaraSDK/Components/PhysicsComponent2D.hpp>
#include <NazaraSDK/Components/CollisionComponent2D.hpp>

namespace Ndk
{
	template<typename T, typename ...Args>
	T* ConstraintComponent2D::CreateConstraint(const Ndk::EntityHandle& first, const Ndk::EntityHandle& second, Args&& ...args)
	{
		auto FetchBody = [](const Ndk::EntityHandle& entity) -> Nz::RigidBody2D*
		{
			if (entity->HasComponent<Ndk::PhysicsComponent2D>())
				return entity->GetComponent<Ndk::PhysicsComponent2D>().GetRigidBody();
			else if (entity->HasComponent<Ndk::CollisionComponent2D>())
				return entity->GetComponent<Ndk::CollisionComponent2D>().GetStaticBody();

			return nullptr;
		};

		Nz::RigidBody2D* firstBody = FetchBody(first);
		NazaraAssert(firstBody, "First entity has no CollisionComponent2D nor PhysicsComponent2D component");

		Nz::RigidBody2D* secondBody = FetchBody(second);
		NazaraAssert(secondBody, "Second entity has no CollisionComponent2D nor PhysicsComponent2D component");

		m_constraints.emplace_back();
		auto& constraintData = m_constraints.back();
		constraintData.constraint = std::make_unique<T>(*firstBody, *secondBody, std::forward<Args>(args)...);

		constraintData.onBodyADestruction.Connect(first->OnEntityDestruction, [this, constraint = constraintData.constraint.get()](const Ndk::Entity* /*entity*/)
		{
			RemoveConstraint(constraint);
		});

		constraintData.onBodyBDestruction.Connect(second->OnEntityDestruction, [this, constraint = constraintData.constraint.get()](const Ndk::Entity* /*entity*/)
		{
			RemoveConstraint(constraint);
		});

		return static_cast<T*>(constraintData.constraint.get());
	}
}
