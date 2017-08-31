// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKELETON_HPP
#define NAZARA_SKELETON_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <vector>

namespace Nz
{
	class Skeleton;

	using SkeletonConstRef = ObjectRef<const Skeleton>;
	using SkeletonLibrary = ObjectLibrary<Skeleton>;
	using SkeletonRef = ObjectRef<Skeleton>;

	struct SkeletonImpl;

	class NAZARA_UTILITY_API Skeleton : public RefCounted
	{
		friend Joint;
		friend SkeletonLibrary;
		friend class Utility;

		public:
			Skeleton() = default;
			Skeleton(const Skeleton& skeleton);
			~Skeleton();

			bool Create(UInt32 jointCount);
			void Destroy();

			const Boxf& GetAABB() const;
			Joint* GetJoint(const String& jointName);
			Joint* GetJoint(UInt32 index);
			const Joint* GetJoint(const String& jointName) const;
			const Joint* GetJoint(UInt32 index) const;
			Joint* GetJoints();
			const Joint* GetJoints() const;
			UInt32 GetJointCount() const;
			int GetJointIndex(const String& jointName) const;

			void Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation);
			void Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation, UInt32* indices, UInt32 indiceCount);

			bool IsValid() const;

			Skeleton& operator=(const Skeleton& skeleton);

			template<typename... Args> static SkeletonRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnSkeletonDestroy, const Skeleton* /*skeleton*/);
			NazaraSignal(OnSkeletonJointsInvalidated, const Skeleton* /*skeleton*/);
			NazaraSignal(OnSkeletonRelease, const Skeleton* /*skeleton*/);

		private:
			void InvalidateJoints();
			void InvalidateJointMap();
			void UpdateJointMap() const;

			static bool Initialize();
			static void Uninitialize();

			SkeletonImpl* m_impl = nullptr;

			static SkeletonLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Utility/Skeleton.inl>

#endif // NAZARA_SKELETON_HPP
