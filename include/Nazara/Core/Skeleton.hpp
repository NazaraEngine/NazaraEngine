// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_SKELETON_HPP
#define NAZARA_CORE_SKELETON_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Math/Box.hpp>
#include <NazaraUtils/Signal.hpp>
#include <string>

namespace Nz
{
	class Joint;
	class Skeleton;

	using SkeletonLibrary = ObjectLibrary<Skeleton>;

	struct SkeletonImpl;

	class NAZARA_CORE_API Skeleton
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
			Joint* GetJoint(std::string_view jointName);
			Joint* GetJoint(std::size_t index);
			const Joint* GetJoint(std::string_view jointName) const;
			const Joint* GetJoint(std::size_t index) const;
			Joint* GetJoints();
			const Joint* GetJoints() const;
			std::size_t GetJointCount() const;
			std::size_t GetJointIndex(std::string_view jointName) const;
			Joint* GetRootJoint();
			const Joint* GetRootJoint() const;

			void Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation);
			void Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation, const std::size_t* indices, std::size_t indiceCount);

			bool IsValid() const;

			Skeleton& operator=(const Skeleton& skeleton);
			Skeleton& operator=(Skeleton&&) noexcept;

			// Signals:
			NazaraSignal(OnSkeletonJointsInvalidated, const Skeleton* /*skeleton*/);

			static constexpr std::size_t InvalidJointIndex = std::numeric_limits<std::size_t>::max();

		private:
			void InvalidateJoints();
			void InvalidateJointMap();
			void UpdateJointMap() const;

			std::unique_ptr<SkeletonImpl> m_impl;
	};
}

#include <Nazara/Core/Skeleton.inl>

#endif // NAZARA_CORE_SKELETON_HPP
