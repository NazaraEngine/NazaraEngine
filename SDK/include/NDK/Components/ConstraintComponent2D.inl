#include <NDK/Components/ConstraintComponent2D.hpp>
#include <NDK/Components/PhysicsComponent2D.hpp>
#include <NDK/Components/CollisionComponent2D.hpp>

namespace Ndk
{
	template<typename T, typename ...Args>
	inline Nz::ObjectRef<T> ConstraintComponent2D::CreateConstraint(const Ndk::EntityHandle first, const Ndk::EntityHandle second, Args && ...args)
	{
		auto QueryBody = [](const Ndk::EntityHandle& entity) -> Nz::RigidBody2D*
		{
			if (entity->HasComponent<Ndk::PhysicsComponent2D>())
				return entity->GetComponent<Ndk::PhysicsComponent2D>().GetRigidBody();
			else if (entity->HasComponent<Ndk::CollisionComponent2D>())
				return entity->GetComponent<Ndk::CollisionComponent2D>().GetStaticBody();
			return nullptr;
		};

		Nz::RigidBody2D* body_first{ QueryBody(first) }, body_second{ QueryBody(second) };

		NazaraAssert(body_first && body_second, "RigidBodies of CollisionComponent2D or PhysicsComponent2D must be valid");

		Nz::ObjectRef<T> constraint = T::New(*body_first, *body_second, std::forward<Args>(args)...);

		m_constraints.push_back(constraint);

		return constraint;
	}
}
