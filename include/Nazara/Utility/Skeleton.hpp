// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKELETON_HPP
#define NAZARA_SKELETON_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Config.hpp>
#include <string>

namespace Nz
{
	class Joint;
	class Skeleton;

	using SkeletonLibrary = ObjectLibrary<Skeleton>;

	struct SkeletonImpl;

	class NAZARA_UTILITY_API Skeleton
	{
		friend Joint;

		public:
			Skeleton();
			Skeleton(const Skeleton& skeleton);
			Skeleton(Skeleton&&) noexcept;
			~Skeleton();

			bool Create(std::size_t jointCount);
			void Destroy();

			const Boxf& GetAABB() const;
			Joint* GetJoint(const std::string& jointName);
			Joint* GetJoint(std::size_t index);
			const Joint* GetJoint(const std::string& jointName) const;
			const Joint* GetJoint(std::size_t index) const;
			Joint* GetJoints();
			const Joint* GetJoints() const;
			std::size_t GetJointCount() const;
			std::size_t GetJointIndex(const std::string& jointName) const;

			void Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation);
			void Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation, const std::size_t* indices, std::size_t indiceCount);

			bool IsValid() const;

			Skeleton& operator=(const Skeleton& skeleton);
			Skeleton& operator=(Skeleton&&) noexcept;

			// Signals:
			NazaraSignal(OnSkeletonJointsInvalidated, const Skeleton* /*skeleton*/);

		private:
			void InvalidateJoints();
			void InvalidateJointMap();
			void UpdateJointMap() const;

			std::unique_ptr<SkeletonImpl> m_impl;
	};
}

#include <Nazara/Utility/Skeleton.inl>

#endif // NAZARA_SKELETON_HPP
