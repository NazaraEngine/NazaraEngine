// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_JOINT_HPP
#define NAZARA_UTILITY_JOINT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/Node.hpp>
#include <string>

namespace Nz
{
	class Skeleton;

	class NAZARA_UTILITY_API Joint : public Node
	{
		public:
			inline Joint(Skeleton* skeleton);
			inline Joint(const Joint& joint);
			inline Joint(Joint&&) noexcept;
			~Joint() = default;

			void EnsureSkinningMatrixUpdate() const;

			const Matrix4f& GetInverseBindMatrix() const;
			const std::string& GetName() const;
			Skeleton* GetSkeleton();
			const Skeleton* GetSkeleton() const;
			const Matrix4f& GetSkinningMatrix() const;

			void SetInverseBindMatrix(const Matrix4f& matrix);
			void SetName(std::string name);

			inline Joint& operator=(const Joint& joint);
			inline Joint& operator=(Joint&& joint) noexcept;

		private:
			void InvalidateNode(Invalidation invalidation) override;
			void UpdateSkinningMatrix() const;

			Matrix4f m_inverseBindMatrix;
			mutable Matrix4f m_skinningMatrix;
			std::string m_name;
			Skeleton* m_skeleton;
			mutable bool m_skinningMatrixUpdated;
	};
}

#include <Nazara/Utility/Joint.inl>

#endif // NAZARA_UTILITY_JOINT_HPP
