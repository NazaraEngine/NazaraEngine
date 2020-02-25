// Copyright (C) 2019 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_COMPONENTS_CONSTRAINTCOMPONENT2D_HPP
#define NDK_COMPONENTS_CONSTRAINTCOMPONENT2D_HPP

#include <Nazara/Physics2D/Constraint2D.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraSDK/Component.hpp>
#include <NazaraSDK/Entity.hpp>
#include <memory>
#include <vector>

namespace Ndk
{
	class ConstraintComponent2D;

	using ConstraintComponent2DHandle = Nz::ObjectHandle<ConstraintComponent2D>;

	class NDK_API ConstraintComponent2D : public Component<ConstraintComponent2D>
	{
		public:
			ConstraintComponent2D() = default;
			ConstraintComponent2D(const ConstraintComponent2D& joint);
			ConstraintComponent2D(ConstraintComponent2D&& joint) = default;

			template<typename T, typename... Args> T* CreateConstraint(const Ndk::EntityHandle& first, const Ndk::EntityHandle& second, Args&&... args);
			bool RemoveConstraint(Nz::Constraint2D* constraint);

			static ComponentIndex componentIndex;

		private:
			struct ConstraintData
			{
				std::unique_ptr<Nz::Constraint2D> constraint;

				NazaraSlot(Ndk::Entity, OnEntityDestruction, onBodyADestruction);
				NazaraSlot(Ndk::Entity, OnEntityDestruction, onBodyBDestruction);
			};

			std::vector<ConstraintData> m_constraints;
	};

}

#include <NazaraSDK/Components/ConstraintComponent2D.inl>

#endif// NDK_COMPONENTS_CONSTRAINTCOMPONENT2D_HPP
