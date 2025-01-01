// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Skeleton.hpp>

namespace Nz
{
	inline Joint::Joint(Skeleton* skeleton) :
	m_skeleton(skeleton),
	m_skinningMatrixUpdated(false)
	{
	}

	inline Joint::Joint(const Joint& joint) :
	Node(joint),
	m_inverseBindMatrix(joint.m_inverseBindMatrix),
	m_name(joint.m_name),
	m_skeleton(joint.m_skeleton),
	m_skinningMatrixUpdated(false)
	{
	}

	inline Joint::Joint(Joint&& joint) noexcept :
	Node(std::move(joint)),
	m_inverseBindMatrix(joint.m_inverseBindMatrix),
	m_name(joint.m_name),
	m_skeleton(joint.m_skeleton),
	m_skinningMatrixUpdated(false)
	{
	}

	inline void Joint::EnsureSkinningMatrixUpdate() const
	{
		if (!m_skinningMatrixUpdated)
			UpdateSkinningMatrix();
	}

	inline const Matrix4f& Joint::GetInverseBindMatrix() const
	{
		return m_inverseBindMatrix;
	}

	inline const std::string& Joint::GetName() const
	{
		return m_name;
	}

	inline Skeleton* Joint::GetSkeleton()
	{
		return m_skeleton;
	}

	inline const Skeleton* Joint::GetSkeleton() const
	{
		return m_skeleton;
	}

	inline const Matrix4f& Joint::GetSkinningMatrix() const
	{
		EnsureSkinningMatrixUpdate();
		return m_skinningMatrix;
	}

	inline void Joint::SetInverseBindMatrix(const Matrix4f& matrix)
	{
		m_inverseBindMatrix = matrix;
		m_skinningMatrixUpdated = false;
	}

	inline void Joint::SetName(std::string name)
	{
		m_name = std::move(name);

		m_skeleton->InvalidateJointMap();
	}

	inline Joint& Joint::operator=(const Joint& joint)
	{
		Node::operator=(joint);

		m_inverseBindMatrix = joint.m_inverseBindMatrix;
		m_name = joint.m_name;
		m_skeleton = joint.m_skeleton;
		m_skinningMatrixUpdated = false;

		return *this;
	}

	inline Joint& Joint::operator=(Joint&& joint) noexcept
	{
		Node::operator=(std::move(joint));

		m_inverseBindMatrix = joint.m_inverseBindMatrix;
		m_name = joint.m_name;
		m_skeleton = joint.m_skeleton;
		m_skinningMatrixUpdated = false;

		return *this;
	}

	inline void Joint::UpdateSkinningMatrix() const
	{
		EnsureTransformMatrixUpdate();

		m_skinningMatrix = Matrix4f::ConcatenateTransform(m_inverseBindMatrix, m_transformMatrix);
		m_skinningMatrixUpdated = true;
	}
}
