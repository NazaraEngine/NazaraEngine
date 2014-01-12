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

		NzMatrix4f GetInverseBindMatrix() const;
		NzString GetName() const;
		NzSkeleton* GetSkeleton();
		const NzSkeleton* GetSkeleton() const;

		void SetInverseBindMatrix(const NzMatrix4f& matrix);
		void SetName(const NzString& name);

	private:
		NzMatrix4f m_inverseBindMatrix;
		NzString m_name;
		NzSkeleton* m_skeleton;
};

#endif // NAZARA_JOINT_HPP
