// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOINT_HPP
#define NAZARA_JOINT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/Node.hpp>

namespace Nz
{
	class Skeleton;

	class NAZARA_UTILITY_API Joint : public Node
	{
		public:
			Joint(Skeleton* skeleton);
			Joint(const Joint& joint);
			~Joint() = default;

			void EnsureSkinningMatrixUpdate() const;

			const Matrix4f& GetInverseBindMatrix() const;
			String GetName() const;
			Skeleton* GetSkeleton();
			const Skeleton* GetSkeleton() const;
			const Matrix4f& GetSkinningMatrix() const;

			void SetInverseBindMatrix(const Matrix4f& matrix);
			void SetName(const String& name);

		private:
			void InvalidateNode() override;
			void UpdateSkinningMatrix() const;

			Matrix4f m_inverseBindMatrix;
			mutable Matrix4f m_skinningMatrix;
			String m_name;
			Skeleton* m_skeleton;
			mutable bool m_skinningMatrixUpdated;
	};
}

#endif // NAZARA_JOINT_HPP
