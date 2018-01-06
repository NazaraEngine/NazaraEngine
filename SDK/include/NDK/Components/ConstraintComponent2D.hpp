#pragma once

#ifndef NDK_COMPONENTS_CONSTRAINTCOMPONENT2D_HPP
#define NDK_COMPONENTS_CONSTRAINTCOMPONENT2D_HPP

#include <NDK/Component.hpp>
#include <Nazara/Physics2D/Constraint2D.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <vector>
#include <memory>

namespace Ndk
{
	class NDK_API ConstraintComponent2D : public Component<ConstraintComponent2D>
	{
		friend class CollisionComponent2D;
		friend class PhysicsComponent2D;

		public:
			ConstraintComponent2D() = default;
			ConstraintComponent2D(const ConstraintComponent2D& joint) = default;
			ConstraintComponent2D(ConstraintComponent2D&& joint) = default;

			template<typename T, typename... Args> inline Nz::ObjectRef<T> CreateConstraint(const Ndk::EntityHandle first, const Ndk::EntityHandle second, Args&&... args);
		
			static ComponentIndex componentIndex;

		private:

			std::vector<Nz::Constraint2DRef> m_constraints;
	};

}

#include <NDK/Components/ConstraintComponent2D.inl>

#endif// NDK_COMPONENTS_CONSTRAINTCOMPONENT2D_HPP
