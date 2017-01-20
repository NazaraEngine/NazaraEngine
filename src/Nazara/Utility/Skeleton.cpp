// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Skeleton.hpp>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	struct SkeletonImpl
	{
		std::unordered_map<String, UInt32> jointMap;
		std::vector<Joint> joints;
		Boxf aabb;
		bool aabbUpdated = false;
		bool jointMapUpdated = false;
	};

	Skeleton::Skeleton(const Skeleton& skeleton) :
	RefCounted(),
	m_impl(nullptr)
	{
		operator=(skeleton);
	}

	Skeleton::~Skeleton()
	{
		OnSkeletonRelease(this);

		Destroy();
	}

	bool Skeleton::Create(UInt32 jointCount)
	{
		#if NAZARA_UTILITY_SAFE
		if (jointCount == 0)
		{
			NazaraError("Joint count must be over zero");
			return false;
		}
		#endif

		m_impl = new SkeletonImpl;
		m_impl->joints.resize(jointCount, Joint(this));

		return true;
	}

	void Skeleton::Destroy()
	{
		if (m_impl)
		{
			OnSkeletonDestroy(this);

			delete m_impl;
			m_impl = nullptr;
		}
	}

	const Boxf& Skeleton::GetAABB() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Skeleton not created");

			static Boxf dummy;
			return dummy;
		}
		#endif

		if (!m_impl->aabbUpdated)
		{
			UInt32 jointCount = m_impl->joints.size();
			if (jointCount > 0)
			{
				Vector3f pos = m_impl->joints[0].GetPosition();
				m_impl->aabb.Set(pos.x, pos.y, pos.z, 0.f, 0.f, 0.f);
				for (UInt32 i = 1; i < jointCount; ++i)
					m_impl->aabb.ExtendTo(m_impl->joints[i].GetPosition());
			}
			else
				m_impl->aabb.MakeZero();

			m_impl->aabbUpdated = true;
		}

		return m_impl->aabb;
	}

	Joint* Skeleton::GetJoint(const String& jointName)
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

		InvalidateJoints();

		return &m_impl->joints[it->second];
	}

	Joint* Skeleton::GetJoint(UInt32 index)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Skeleton not created");
			return nullptr;
		}

		if (index >= m_impl->joints.size())
		{
			NazaraError("Joint index out of range (" + String::Number(index) + " >= " + String::Number(m_impl->joints.size()) + ')');
			return nullptr;
		}
		#endif

		InvalidateJoints();

		return &m_impl->joints[index];
	}

	const Joint* Skeleton::GetJoint(const String& jointName) const
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

	const Joint* Skeleton::GetJoint(UInt32 index) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Skeleton not created");
			return nullptr;
		}

		if (index >= m_impl->joints.size())
		{
			NazaraError("Joint index out of range (" + String::Number(index) + " >= " + String::Number(m_impl->joints.size()) + ')');
			return nullptr;
		}
		#endif

		return &m_impl->joints[index];
	}

	Joint* Skeleton::GetJoints()
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

	const Joint* Skeleton::GetJoints() const
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

	UInt32 Skeleton::GetJointCount() const
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

	int Skeleton::GetJointIndex(const String& jointName) const
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

	void Skeleton::Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation)
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

		Joint* jointsA = &skeletonA.m_impl->joints[0];
		Joint* jointsB = &skeletonB.m_impl->joints[0];
		for (std::size_t i = 0; i < m_impl->joints.size(); ++i)
			m_impl->joints[i].Interpolate(jointsA[i], jointsB[i], interpolation, CoordSys_Local);

		InvalidateJoints();
	}

	void Skeleton::Interpolate(const Skeleton& skeletonA, const Skeleton& skeletonB, float interpolation, UInt32* indices, UInt32 indiceCount)
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

		const Joint* jointsA = &skeletonA.m_impl->joints[0];
		const Joint* jointsB = &skeletonB.m_impl->joints[0];
		for (UInt32 i = 0; i < indiceCount; ++i)
		{
			UInt32 index = indices[i];

			#if NAZARA_UTILITY_SAFE
			if (index >= m_impl->joints.size())
			{
				NazaraError("Index #" + String::Number(i) + " out of range (" + String::Number(index) + " >= " + String::Number(m_impl->joints.size()) + ')');
				return;
			}
			#endif

			m_impl->joints[index].Interpolate(jointsA[index], jointsB[index], interpolation, CoordSys_Local);
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
			m_impl = new SkeletonImpl;
			m_impl->jointMap = skeleton.m_impl->jointMap;
			m_impl->jointMapUpdated = skeleton.m_impl->jointMapUpdated;
			m_impl->joints = skeleton.m_impl->joints;

			// Code crade mais son optimisation demanderait de stocker jointCount*sizeof(UInt32) en plus
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

	void Skeleton::InvalidateJoints()
	{
		m_impl->aabbUpdated = false;

		OnSkeletonJointsInvalidated(this);
	}

	void Skeleton::InvalidateJointMap()
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

	void Skeleton::UpdateJointMap() const
	{
		#ifdef NAZARA_DEBUG
		if (!m_impl)
		{
			NazaraError("Invalid skeleton");
			return;
		}
		#endif

		m_impl->jointMap.clear();
		for (std::size_t i = 0; i < m_impl->joints.size(); ++i)
		{
			String name = m_impl->joints[i].GetName();
			if (!name.IsEmpty())
			{
				#if NAZARA_UTILITY_SAFE
				auto it = m_impl->jointMap.find(name);
				if (it != m_impl->jointMap.end())
				{
					NazaraWarning("Joint name \"" + name + "\" is already present in joint map for joint #" + String::Number(it->second));
					continue;
				}
				#endif

				m_impl->jointMap[name] = i;
			}
		}

		m_impl->jointMapUpdated = true;
	}

	bool Skeleton::Initialize()
	{
		if (!SkeletonLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	void Skeleton::Uninitialize()
	{
		SkeletonLibrary::Uninitialize();
	}

	SkeletonLibrary::LibraryMap Skeleton::s_library;
}
