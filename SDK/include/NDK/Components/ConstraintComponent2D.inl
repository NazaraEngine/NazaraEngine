#include <NDK/Components/ConstraintComponent2D.hpp>
#include <NDK/Components/PhysicsComponent2D.hpp>
#include <NDK/Components/CollisionComponent2D.hpp>

namespace Ndk
{
	template<typename T, typename ...Args>
	Nz::ObjectRef<T> ConstraintComponent2D::CreateConstraint(const Ndk::EntityHandle& first, const Ndk::EntityHandle& second, Args && ...args)
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

		Nz::ObjectRef<T> constraint = T::New(*firstBody, *secondBody, std::forward<Args>(args)...);
		m_constraints.push_back(constraint);

		return constraint;
	}
}
