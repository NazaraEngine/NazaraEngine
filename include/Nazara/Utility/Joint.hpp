// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOINT_HPP
#define NAZARA_JOINT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/Node.hpp>

class NzSkeleton;

class NAZARA_API NzJoint : public NzNode
{
	public:
		NzJoint(NzSkeleton* skeleton);
		NzJoint(const NzJoint& joint);
		~NzJoint() = default;

		void EnsureSkinningMatrixUpdate() const;

		const NzMatrix4f& GetInverseBindMatrix() const;
		NzString GetName() const;
		NzSkeleton* GetSkeleton();
		const NzSkeleton* GetSkeleton() const;
		const NzMatrix4f& GetSkinningMatrix() const;

		void SetInverseBindMatrix(const NzMatrix4f& matrix);
		void SetName(const NzString& name);

	private:
		void InvalidateNode();
		void UpdateSkinningMatrix() const;

		NzMatrix4f m_inverseBindMatrix;
		mutable NzMatrix4f m_skinningMatrix;
		NzString m_name;
		NzSkeleton* m_skeleton;
		mutable bool m_skinningMatrixUpdated;
};

#endif // NAZARA_JOINT_HPP
