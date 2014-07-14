// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKELETON_HPP
#define NAZARA_SKELETON_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <vector>

struct NzSkeletonImpl;

class NAZARA_API NzSkeleton : public NzRefCounted
{
	friend NzJoint;

	public:
		NzSkeleton() = default;
		NzSkeleton(const NzSkeleton& skeleton);
		~NzSkeleton();

		bool Create(unsigned int jointCount);
		void Destroy();

		const NzBoxf& GetAABB() const;
		NzJoint* GetJoint(const NzString& jointName);
		NzJoint* GetJoint(unsigned int index);
		const NzJoint* GetJoint(const NzString& jointName) const;
		const NzJoint* GetJoint(unsigned int index) const;
		NzJoint* GetJoints();
		const NzJoint* GetJoints() const;
		unsigned int GetJointCount() const;
		int GetJointIndex(const NzString& jointName) const;

		void Interpolate(const NzSkeleton& skeletonA, const NzSkeleton& skeletonB, float interpolation);
		void Interpolate(const NzSkeleton& skeletonA, const NzSkeleton& skeletonB, float interpolation, unsigned int* indices, unsigned int indiceCount);

		bool IsValid() const;

		NzSkeleton& operator=(const NzSkeleton& skeleton);

	private:
		void InvalidateJoints();
		void InvalidateJointMap();
		void UpdateJointMap() const;

		NzSkeletonImpl* m_impl = nullptr;
};

#endif // NAZARA_SKELETON_HPP
