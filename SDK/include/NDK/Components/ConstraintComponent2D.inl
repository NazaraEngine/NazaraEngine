#include <NDK/Components/ConstraintComponent2D.hpp>

namespace Ndk
{
	template<typename T, typename ...Args>
	inline Nz::ObjectRef<T> ConstraintComponent2D::CreateConstraint(const Ndk::EntityHandle first, const Ndk::EntityHandle second, Args && ...args)
	{
		NazaraAssert((first->HasComponent<Ndk::PhysicsComponent2D>() || first->HasComponent<Ndk::CollisionComponent2D>()) && (second->HasComponent<Ndk::PhysicsComponent2D>() && second->HasComponent<Ndk::CollisionComponent2D>()), "These Entity must have	a CollisionComponent2D or a PhysicsComponent2D");

		if (first->HasComponent<Ndk::PhysicsComponent2D>() && second->HasComponent<Ndk::PhysicsComponent2D>())
		{
			Nz::ObjectRef<T> constraint = T::New(first->GetComponent<Ndk::PhysicsComponent2D>().GetRigidBody(), second->GetComponent<Ndk::PhysicsComponent2D>().GetRigidBody(), std::forward<Args>(args)...);
			AddJoint(constraint);
			return constraint;
		}
		else if (first->HasComponent<Ndk::CollisionComponent2D>() && second->HasComponent<Ndk::PhysicsComponent2D>())
		{
			Nz::ObjectRef<T> constraint = T::New(first->GetComponent<Ndk::CollisionComponent2D>().GetStaticBody(), second->GetComponent<Ndk::PhysicsComponent2D>().GetRigidBody(), std::forward<Args>(args)...);
			AddJoint(constraint);
			return constraint;
		}
		else if (first->HasComponent<Ndk::PhysicsComponent2D>() && second->HasComponent<Ndk::CollisionComponent2D>())
		{
			Nz::ObjectRef<T> constraint = T::New(first->GetComponent<Ndk::PhysicsComponent2D>().GetRigidBody(), second->GetComponent<Ndk::CollisionComponent2D>().GetStaticBody(), std::forward<Args>(args)...);
			AddJoint(constraint);
			return constraint;
		}
		else if (first->HasComponent<Ndk::CollisionComponent2D>() && second->HasComponent<Ndk::CollisionComponent2D>())
		{
			Nz::ObjectRef<T> constraint = T::New(first->GetComponent<Ndk::CollisionComponent2D>().GetStaticBody(), second->GetComponent<Ndk::CollisionComponent2D>().GetStaticBody(), std::forward<Args>(args)...);
			AddJoint(constraint);
			return constraint;
		}
	}

	inline void ConstraintComponent2D::AddJoint(const Nz::Constraint2DRef& joint)
	{
		m_constraints.push_back(joint);
	}
}
