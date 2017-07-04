// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	Joint::Joint(Skeleton* skeleton) :
	m_skeleton(skeleton),
	m_skinningMatrixUpdated(false)
	{
	}

	Joint::Joint(const Joint& joint) :
	Node(joint),
	m_inverseBindMatrix(joint.m_inverseBindMatrix),
	m_name(joint.m_name),
	m_skeleton(joint.m_skeleton),
	m_skinningMatrixUpdated(false)
	{
	}

	void Joint::EnsureSkinningMatrixUpdate() const
	{
		if (!m_skinningMatrixUpdated)
			UpdateSkinningMatrix();
	}

	const Matrix4f& Joint::GetInverseBindMatrix() const
	{
		return m_inverseBindMatrix;
	}

	String Joint::GetName() const
	{
		return m_name;
	}

	Skeleton* Joint::GetSkeleton()
	{
		return m_skeleton;
	}

	const Skeleton* Joint::GetSkeleton() const
	{
		return m_skeleton;
	}

	const Matrix4f& Joint::GetSkinningMatrix() const
	{
		if (!m_skinningMatrixUpdated)
			UpdateSkinningMatrix();

		return m_skinningMatrix;
	}

	void Joint::SetInverseBindMatrix(const Matrix4f& matrix)
	{
		m_inverseBindMatrix = matrix;
		m_skinningMatrixUpdated = false;
	}

	void Joint::SetName(const String& name)
	{
		m_name = name;

		m_skeleton->InvalidateJointMap();
	}

	void Joint::InvalidateNode()
	{
		Node::InvalidateNode();

		m_skinningMatrixUpdated = false;
	}

	void Joint::UpdateSkinningMatrix() const
	{
		if (!m_transformMatrixUpdated)
			UpdateTransformMatrix();

		m_skinningMatrix.Set(m_inverseBindMatrix);
		m_skinningMatrix.ConcatenateAffine(m_transformMatrix);
		m_skinningMatrixUpdated = true;
	}
}
