// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/AxisAlignedBox.hpp>
#include <map>
#include <Nazara/Utility/Debug.hpp>

struct NzSkeletonImpl
{
	std::map<NzString, unsigned int> jointMap;
	std::vector<NzJoint> joints;
	NzAxisAlignedBox aabb;
	bool jointMapUpdated = false;
};

NzSkeleton::NzSkeleton(const NzSkeleton& skeleton) :
m_impl(nullptr)
{
	operator=(skeleton);
}

NzSkeleton::~NzSkeleton()
{
	Destroy();
}

bool NzSkeleton::Create(unsigned int jointCount)
{
	#if NAZARA_UTILITY_SAFE
	if (jointCount == 0)
	{
		NazaraError("Joint count must be over 0");
		return false;
	}
	#endif

	m_impl = new NzSkeletonImpl;
	m_impl->joints.resize(jointCount, NzJoint(this));

	return true;
}

void NzSkeleton::Destroy()
{
	if (m_impl)
	{
		delete m_impl;
		m_impl = nullptr;
	}
}

const NzAxisAlignedBox& NzSkeleton::GetAABB() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return NzAxisAlignedBox::Null;
	}
	#endif

	if (m_impl->aabb.IsNull())
	{
		for (unsigned int i = 0; i < m_impl->joints.size(); ++i)
			m_impl->aabb.ExtendTo(m_impl->joints[i].GetPosition());
	}

	return m_impl->aabb;
}

NzJoint* NzSkeleton::GetJoint(const NzString& jointName)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return nullptr;
	}
	#endif

	if (!m_impl->jointMapUpdated)
		UpdateJointMap();

	auto it = m_impl->jointMap.find(jointName);

	#if NAZARA_UTILITY_SAFE
	if (it == m_impl->jointMap.end())
	{
		NazaraError("Joint not found");
		return nullptr;
	}
	#endif

	// Invalidation de l'AABB
	m_impl->aabb.SetNull();

	return &m_impl->joints[it->second];
}

NzJoint* NzSkeleton::GetJoint(unsigned int index)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return nullptr;
	}

	if (index >= m_impl->joints.size())
	{
		NazaraError("Joint index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->joints.size()) + ')');
		return nullptr;
	}
	#endif

	// Invalidation de l'AABB
	m_impl->aabb.SetNull();

	return &m_impl->joints[index];
}

const NzJoint* NzSkeleton::GetJoint(const NzString& jointName) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return nullptr;
	}
	#endif

	if (!m_impl->jointMapUpdated)
		UpdateJointMap();

	auto it = m_impl->jointMap.find(jointName);

	#if NAZARA_UTILITY_SAFE
	if (it == m_impl->jointMap.end())
	{
		NazaraError("Joint not found");
		return nullptr;
	}
	#endif

	return &m_impl->joints[it->second];
}

const NzJoint* NzSkeleton::GetJoint(unsigned int index) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return nullptr;
	}

	if (index >= m_impl->joints.size())
	{
		NazaraError("Joint index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->joints.size()) + ')');
		return nullptr;
	}
	#endif

	return &m_impl->joints[index];
}

NzJoint* NzSkeleton::GetJoints()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return nullptr;
	}
	#endif

	return &m_impl->joints[0];
}

const NzJoint* NzSkeleton::GetJoints() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return nullptr;
	}
	#endif

	return &m_impl->joints[0];
}

unsigned int NzSkeleton::GetJointCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return 0;
	}
	#endif

	return m_impl->joints.size();
}

int NzSkeleton::GetJointIndex(const NzString& jointName) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return -1;
	}
	#endif

	if (!m_impl->jointMapUpdated)
		UpdateJointMap();

	auto it = m_impl->jointMap.find(jointName);

	#if NAZARA_UTILITY_SAFE
	if (it == m_impl->jointMap.end())
	{
		NazaraError("Joint not found");
		return -1;
	}
	#endif

	return it->second;
}

void NzSkeleton::Interpolate(const NzSkeleton& skeletonA, const NzSkeleton& skeletonB, float interpolation)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return;
	}

	if (!skeletonA.IsValid())
	{
		NazaraError("Skeleton A is invalid");
		return;
	}

	if (!skeletonB.IsValid())
	{
		NazaraError("Skeleton B is invalid");
		return;
	}

	if (skeletonA.GetJointCount() != skeletonB.GetJointCount() || m_impl->joints.size() != skeletonA.GetJointCount())
	{
		NazaraError("Skeletons must have the same joint count");
		return;
	}
	#endif

	NzJoint* jointsA = &skeletonA.m_impl->joints[0];
	NzJoint* jointsB = &skeletonB.m_impl->joints[0];
	for (unsigned int i = 0; i < m_impl->joints.size(); ++i)
		m_impl->joints[i].Interpolate(jointsA[i], jointsB[i], interpolation);
}

void NzSkeleton::Interpolate(const NzSkeleton& skeletonA, const NzSkeleton& skeletonB, float interpolation, unsigned int* indices, unsigned int indiceCount)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeleton not created");
		return;
	}

	if (!skeletonA.IsValid())
	{
		NazaraError("Skeleton A is invalid");
		return;
	}

	if (!skeletonB.IsValid())
	{
		NazaraError("Skeleton B is invalid");
		return;
	}

	if (skeletonA.GetJointCount() != skeletonB.GetJointCount() || m_impl->joints.size() != skeletonA.GetJointCount())
	{
		NazaraError("Skeletons must have the same joint count");
		return;
	}
	#endif

	const NzJoint* jointsA = &skeletonA.m_impl->joints[0];
	const NzJoint* jointsB = &skeletonB.m_impl->joints[0];
	for (unsigned int i = 0; i < indiceCount; ++i)
	{
		unsigned int index = indices[i];

		#if NAZARA_UTILITY_SAFE
		if (index >= m_impl->joints.size())
		{
			NazaraError("Index #" + NzString::Number(i) + " out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->joints.size()) + ')');
			return;
		}
		#endif

		m_impl->joints[index].Interpolate(jointsA[index], jointsB[index], interpolation);
	}
}

bool NzSkeleton::IsValid() const
{
	return m_impl != nullptr;
}

NzSkeleton& NzSkeleton::operator=(const NzSkeleton& skeleton)
{
	Destroy();

	if (skeleton.m_impl)
	{
		m_impl = new NzSkeletonImpl;
		m_impl->jointMap = skeleton.m_impl->jointMap;
		m_impl->jointMapUpdated = skeleton.m_impl->jointMapUpdated;
		m_impl->joints = skeleton.m_impl->joints;

		// Code crade mais son optimisation demanderait de stocker jointCount*sizeof(unsigned int) en plus
		// Ce qui, pour juste une copie qui ne se fera que rarement, ne vaut pas le coup
		// L'éternel trade-off mémoire/calculs ..
		unsigned int jointCount = skeleton.m_impl->joints.size();
		for (unsigned int i = 0; i < jointCount; ++i)
		{
			const NzNode* parent = skeleton.m_impl->joints[i].GetParent();
			if (parent)
			{
				for (unsigned int j = 0; j < i; ++j) // Le parent se trouve forcément avant nous
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

void NzSkeleton::InvalidateJointMap()
{
	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Invalid skeleton");
		return;
	}
	#endif

	m_impl->jointMapUpdated = false;
}

void NzSkeleton::UpdateJointMap() const
{
	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Invalid skeleton");
		return;
	}
	#endif

	m_impl->jointMap.clear();
	for (unsigned int i = 0; i < m_impl->joints.size(); ++i)
	{
		NzString name = m_impl->joints[i].GetName();
		if (!name.IsEmpty())
		{
			#if NAZARA_UTILITY_SAFE
			auto it = m_impl->jointMap.find(name);
			if (it != m_impl->jointMap.end())
			{
				NazaraWarning("Joint name \"" + name + "\" is already present in joint map for joint #" + NzString::Number(it->second));
				continue;
			}
			#endif

			m_impl->jointMap[name] = i;
		}
	}

	m_impl->jointMapUpdated = true;
}
