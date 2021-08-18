// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	struct SkeletonImpl
	{
		std::unordered_map<std::string, std::size_t> jointMap;
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

	bool Skeleton::Create(std::size_t jointCount)
	{
		NazaraAssert(jointCount > 0, "joint count must be over zero");

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
		NazaraAssert(m_impl, "skeleton must have been created");

		if (!m_impl->aabbUpdated)
		{
			std::size_t jointCount = m_impl->joints.size();
			if (jointCount > 0)
			{
				Vector3f pos = m_impl->joints[0].GetPosition();
				m_impl->aabb.Set(pos.x, pos.y, pos.z, 0.f, 0.f, 0.f);
				for (std::size_t i = 1; i < jointCount; ++i)
					m_impl->aabb.ExtendTo(m_impl->joints[i].GetPosition());
			}
			else
				m_impl->aabb.MakeZero();

			m_impl->aabbUpdated = true;
		}

		return m_impl->aabb;
	}

	Joint* Skeleton::GetJoint(const std::string& jointName)
	{
		NazaraAssert(m_impl, "skeleton must have been created");

		if (!m_impl->jointMapUpdated)
			UpdateJointMap();

		auto it = m_impl->jointMap.find(jointName);
		NazaraAssert(it != m_impl->jointMap.end(), "joint not found");

		InvalidateJoints();
		return &m_impl->joints[it->second];
	}

	Joint* Skeleton::GetJoint(std::size_t index)
	{
		NazaraAssert(m_impl, "skeleton must have been created");
		NazaraAssert(index < m_impl->joints.size(), "joint index out of range");

		InvalidateJoints();
		return &m_impl->joints[index];
	}

	const Joint* Skeleton::GetJoint(const std::string& jointName) const
	{
		NazaraAssert(m_impl, "skeleton must have been created");

		if (!m_impl->jointMapUpdated)
			UpdateJointMap();

		auto it = m_impl->jointMap.find(jointName);
		NazaraAssert(it != m_impl->jointMap.end(), "joint not found");

		return &m_impl->joints[it->second];
	}

	const Joint* Skeleton::GetJoint(std::size_t index) const
	{
		NazaraAssert(m_impl, "skeleton must have been created");
		NazaraAssert(index < m_impl->joints.size(), "joint index out of range");

		return &m_impl->joints[index];
	}

	Joint* Skeleton::GetJoints()
	{
		NazaraAssert(m_impl, "skeleton must have been created");

		InvalidateJoints();
		return &m_impl->joints[0];
	}

	const Joint* Skeleton::GetJoints() const
	{
		NazaraAssert(m_impl, "skeleton must have been created");
		return &m_impl->joints[0];
	}

	std::size_t Skeleton::GetJointCount() const
	{
		NazaraAssert(m_impl, "skeleton must have been created");
		return static_cast<std::size_t>(m_impl->joints.size());
	}

	std::size_t Skeleton::GetJointIndex(const std::string& jointName) const
	{
		NazaraAssert(m_impl, "skeleton must have been created");

		if (!m_impl->jointMapUpdated)
			UpdateJointMap();

		auto it = m_impl->jointMap.find(jointName);
		NazaraAssert(it != m_impl->jointMap.end(), "joint not found");

		return it->second;
	}

	void Skeleton::Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation)
	{
		NazaraAssert(m_impl, "skeleton must have been created");
		NazaraAssert(skeletonA.IsValid(), "first skeleton is invalid");
		NazaraAssert(skeletonB.IsValid(), "second skeleton is invalid");
		NazaraAssert(skeletonA.GetJointCount() == skeletonB.GetJointCount() && m_impl->joints.size() == skeletonA.GetJointCount(), "both skeletons must have the same number of joints");

		const Joint* jointsA = &skeletonA.m_impl->joints[0];
		const Joint* jointsB = &skeletonB.m_impl->joints[0];
		for (std::size_t i = 0; i < m_impl->joints.size(); ++i)
			m_impl->joints[i].Interpolate(jointsA[i], jointsB[i], interpolation, CoordSys::Local);

		InvalidateJoints();
	}

	void Skeleton::Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation, const std::size_t* indices, std::size_t indiceCount)
	{
		NazaraAssert(m_impl, "skeleton must have been created");
		NazaraAssert(skeletonA.IsValid(), "first skeleton is invalid");
		NazaraAssert(skeletonB.IsValid(), "second skeleton is invalid");
		NazaraAssert(skeletonA.GetJointCount() == skeletonB.GetJointCount() && m_impl->joints.size() == skeletonA.GetJointCount(), "both skeletons must have the same number of joints");

		const Joint* jointsA = &skeletonA.m_impl->joints[0];
		const Joint* jointsB = &skeletonB.m_impl->joints[0];
		for (std::size_t i = 0; i < indiceCount; ++i)
		{
			std::size_t index = indices[i];
			NazaraAssert(index < m_impl->joints.size(), "joint index out of range");

			m_impl->joints[index].Interpolate(jointsA[index], jointsB[index], interpolation, CoordSys::Local);
		}

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

			// Code crade mais son optimisation demanderait de stocker jointCount*sizeof(std::size_t) en plus
			// Ce qui, pour juste une copie qui ne se fera que rarement, ne vaut pas le coup
			// L'éternel trade-off mémoire/calculs ..
			std::size_t jointCount = skeleton.m_impl->joints.size();
			for (std::size_t i = 0; i < jointCount; ++i)
			{
				const Node* parent = skeleton.m_impl->joints[i].GetParent();
				if (parent)
				{
					for (std::size_t j = 0; j < i; ++j) // Le parent se trouve forcément avant nous
					{
						if (parent == &skeleton.m_impl->joints[j]) // A-t-on trouvé le parent ?
						{
							m_impl->joints[i].SetParent(m_impl->joints[j]); // Oui, tout ça pour ça
							break;
						}
					}
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
		NazaraAssert(m_impl, "skeleton must have been created");
		m_impl->jointMapUpdated = false;
	}

	void Skeleton::UpdateJointMap() const
	{
		NazaraAssert(m_impl, "skeleton must have been created");

		m_impl->jointMap.clear();
		for (std::size_t i = 0; i < m_impl->joints.size(); ++i)
		{
			const std::string& name = m_impl->joints[i].GetName();
			if (!name.empty())
			{
				NazaraAssert(m_impl->jointMap.find(name) == m_impl->jointMap.end(), "Joint name \"" + name + "\" is already present in joint map");

				m_impl->jointMap[name] = static_cast<std::size_t>(i);
			}
		}

		m_impl->jointMapUpdated = true;
	}
}
