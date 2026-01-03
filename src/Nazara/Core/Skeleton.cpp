// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Core/Joint.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <unordered_map>

namespace Nz
{
	struct SkeletonImpl
	{
		std::unordered_map<std::string, std::size_t, StringHash<>, std::equal_to<>> jointMap;
		std::vector<Joint> joints;
		Boxf aabb;
		bool aabbUpdated = false;
		bool jointMapUpdated = false;
	};

	Skeleton::Skeleton() = default;

	Skeleton::Skeleton(const Skeleton& skeleton)
	{
		operator=(skeleton);
	}

	Skeleton::Skeleton(Skeleton&&) noexcept = default;
	Skeleton::~Skeleton() = default;

	void Skeleton::CopyPose(const Skeleton& skeleton)
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");
		NazaraAssertMsg(skeleton.IsValid(), "skeleton is invalid");
		NazaraAssertMsg(m_impl->joints.size() == skeleton.GetJointCount(), "both skeletons must have the same number of joints");

		Joint* dstJoints = &m_impl->joints[0];
		const Joint* srcJoints = &skeleton.m_impl->joints[0];
		for (std::size_t i = 0; i < m_impl->joints.size(); ++i)
			dstJoints[i].CopyLocalTransform(srcJoints[i], Node::Invalidation::DontInvalidate);

		GetRootJoint()->Invalidate();
		InvalidateJoints();
	}

	bool Skeleton::Create(std::size_t jointCount)
	{
		NazaraAssertMsg(jointCount > 0, "joint count must be over zero");

		m_impl = std::make_unique<SkeletonImpl>();
		m_impl->joints.reserve(jointCount);
		for (std::size_t i = 0; i < jointCount; ++i)
			m_impl->joints.emplace_back(this);

		return true;
	}

	void Skeleton::Destroy()
	{
		m_impl.reset();
	}

	const Boxf& Skeleton::GetAABB() const
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");

		if (!m_impl->aabbUpdated)
		{
			std::size_t jointCount = m_impl->joints.size();
			if (jointCount > 0)
			{
				Vector3f pos = m_impl->joints[0].GetPosition();
				m_impl->aabb = Boxf(pos, Vector3f::Zero());
				for (std::size_t i = 1; i < jointCount; ++i)
					m_impl->aabb.ExtendTo(m_impl->joints[i].GetPosition());
			}
			else
				m_impl->aabb = Boxf::Zero();

			m_impl->aabbUpdated = true;
		}

		return m_impl->aabb;
	}

	Joint* Skeleton::GetJoint(std::string_view jointName)
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");

		if (!m_impl->jointMapUpdated)
			UpdateJointMap();

		auto it = m_impl->jointMap.find(jointName);
		NazaraAssertMsg(it != m_impl->jointMap.end(), "joint not found");

		InvalidateJoints();
		return &m_impl->joints[it->second];
	}

	Joint* Skeleton::GetJoint(std::size_t index)
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");
		NazaraAssertMsg(index < m_impl->joints.size(), "joint index out of range");

		InvalidateJoints();
		return &m_impl->joints[index];
	}

	const Joint* Skeleton::GetJoint(std::string_view jointName) const
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");

		if (!m_impl->jointMapUpdated)
			UpdateJointMap();

		auto it = m_impl->jointMap.find(jointName);
		NazaraAssertMsg(it != m_impl->jointMap.end(), "joint not found");

		return &m_impl->joints[it->second];
	}

	const Joint* Skeleton::GetJoint(std::size_t index) const
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");
		NazaraAssertMsg(index < m_impl->joints.size(), "joint index out of range");

		return &m_impl->joints[index];
	}

	Joint* Skeleton::GetJoints()
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");

		InvalidateJoints();
		return &m_impl->joints[0];
	}

	const Joint* Skeleton::GetJoints() const
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");
		return &m_impl->joints[0];
	}

	std::size_t Skeleton::GetJointCount() const
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");
		return static_cast<std::size_t>(m_impl->joints.size());
	}

	std::size_t Skeleton::GetJointIndex(std::string_view jointName) const
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");

		if (!m_impl->jointMapUpdated)
			UpdateJointMap();

		auto it = m_impl->jointMap.find(jointName);
		if (it == m_impl->jointMap.end())
			return InvalidJointIndex;

		return it->second;
	}

	Joint* Skeleton::GetRootJoint()
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");

		assert(!m_impl->joints.empty());
		return &m_impl->joints.front();
	}

	const Joint* Skeleton::GetRootJoint() const
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");

		assert(!m_impl->joints.empty());
		return &m_impl->joints.front();
	}

	void Skeleton::Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation)
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");
		NazaraAssertMsg(skeletonA.IsValid(), "first skeleton is invalid");
		NazaraAssertMsg(skeletonB.IsValid(), "second skeleton is invalid");
		NazaraAssertMsg(skeletonA.GetJointCount() == skeletonB.GetJointCount() && m_impl->joints.size() == skeletonA.GetJointCount(), "both skeletons must have the same number of joints");

		const Joint* jointsA = &skeletonA.m_impl->joints[0];
		const Joint* jointsB = &skeletonB.m_impl->joints[0];
		for (std::size_t i = 0; i < m_impl->joints.size(); ++i)
			m_impl->joints[i].Interpolate(jointsA[i], jointsB[i], interpolation, Node::Invalidation::DontInvalidate);

		GetRootJoint()->Invalidate();
		InvalidateJoints();
	}

	void Skeleton::Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation, const std::size_t* indices, std::size_t indiceCount)
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");
		NazaraAssertMsg(skeletonA.IsValid(), "first skeleton is invalid");
		NazaraAssertMsg(skeletonB.IsValid(), "second skeleton is invalid");
		NazaraAssertMsg(skeletonA.GetJointCount() == skeletonB.GetJointCount() && m_impl->joints.size() == skeletonA.GetJointCount(), "both skeletons must have the same number of joints");

		const Joint* jointsA = &skeletonA.m_impl->joints[0];
		const Joint* jointsB = &skeletonB.m_impl->joints[0];
		for (std::size_t i = 0; i < indiceCount; ++i)
		{
			std::size_t index = indices[i];
			NazaraAssertMsg(index < m_impl->joints.size(), "joint index out of range");

			m_impl->joints[index].Interpolate(jointsA[index], jointsB[index], interpolation, Node::Invalidation::DontInvalidate);
		}

		GetRootJoint()->Invalidate();
		InvalidateJoints();
	}

	bool Skeleton::IsValid() const
	{
		return m_impl != nullptr;
	}

	Skeleton& Skeleton::operator=(const Skeleton& skeleton)
	{
		if (this == &skeleton)
			return *this;

		Destroy();

		if (skeleton.m_impl)
		{
			m_impl = std::make_unique<SkeletonImpl>();
			m_impl->jointMap = skeleton.m_impl->jointMap;
			m_impl->jointMapUpdated = skeleton.m_impl->jointMapUpdated;
			m_impl->joints = skeleton.m_impl->joints;

			// Restore parent hierarchy
			const Joint* firstJoint = skeleton.m_impl->joints.data();

			std::size_t jointCount = skeleton.m_impl->joints.size();
			for (std::size_t i = 0; i < jointCount; ++i)
			{
				const Joint* parent = SafeCast<const Joint*>(skeleton.m_impl->joints[i].GetParent());
				if (parent)
				{
					std::size_t parentIndex = SafeCast<std::size_t>(parent - firstJoint);
					m_impl->joints[i].SetParent(m_impl->joints[parentIndex]);
				}
			}
		}

		return *this;
	}

	Skeleton& Skeleton::operator=(Skeleton&&) noexcept = default;

	void Skeleton::InvalidateJoints()
	{
		m_impl->aabbUpdated = false;

		OnSkeletonJointsInvalidated(this);
	}

	void Skeleton::InvalidateJointMap()
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");
		m_impl->jointMapUpdated = false;
	}

	void Skeleton::UpdateJointMap() const
	{
		NazaraAssertMsg(m_impl, "skeleton must have been created");

		m_impl->jointMap.clear();
		for (std::size_t i = 0; i < m_impl->joints.size(); ++i)
		{
			const std::string& name = m_impl->joints[i].GetName();
			if (!name.empty())
			{
				NazaraAssertMsg(!m_impl->jointMap.contains(name), "joint name \"%s\" is already present in joint map", name.c_str());
				m_impl->jointMap.emplace(name, i);
			}
		}

		m_impl->jointMapUpdated = true;
	}
}
