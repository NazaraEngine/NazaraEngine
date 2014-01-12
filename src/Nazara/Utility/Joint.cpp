// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/Debug.hpp>

NzJoint::NzJoint(NzSkeleton* skeleton) :
m_skeleton(skeleton)
{
}

NzJoint::NzJoint(const NzJoint& joint) :
NzNode(joint),
m_inverseBindMatrix(joint.m_inverseBindMatrix),
m_name(joint.m_name),
m_skeleton(joint.m_skeleton)
{
}

NzMatrix4f NzJoint::GetInverseBindMatrix() const
{
	return m_inverseBindMatrix;
}

NzString NzJoint::GetName() const
{
	return m_name;
}

NzSkeleton* NzJoint::GetSkeleton()
{
	return m_skeleton;
}

const NzSkeleton* NzJoint::GetSkeleton() const
{
	return m_skeleton;
}

void NzJoint::SetInverseBindMatrix(const NzMatrix4f& matrix)
{
	m_inverseBindMatrix = matrix;
}

void NzJoint::SetName(const NzString& name)
{
	m_name = name;

	m_skeleton->InvalidateJointMap();
}
