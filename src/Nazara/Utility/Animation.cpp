// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <vector>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	struct AnimationImpl
	{
		std::unordered_map<String, unsigned int> sequenceMap;
		std::vector<Sequence> sequences;
		std::vector<SequenceJoint> sequenceJoints; // Uniquement pour les animations squelettiques
		AnimationType type;
		bool loopPointInterpolation = false;
		unsigned int frameCount;
		unsigned int jointCount;  // Uniquement pour les animations squelettiques
	};

	bool AnimationParams::IsValid() const
	{
		if (startFrame > endFrame)
		{
			NazaraError("Start frame index must be smaller than end frame index");
			return false;
		}

		return true;
	}

	Animation::~Animation()
	{
		OnAnimationRelease(this);

		Destroy();
	}

	bool Animation::AddSequence(const Sequence& sequence)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return false;
		}

		if (sequence.frameCount == 0)
		{
			NazaraError("Sequence frame count must be over zero");
			return false;
		}
		#endif

		if (m_impl->type == AnimationType_Skeletal)
		{
			unsigned int endFrame = sequence.firstFrame + sequence.frameCount - 1;
			if (endFrame >= m_impl->frameCount)
			{
				m_impl->frameCount = endFrame+1;
				m_impl->sequenceJoints.resize(m_impl->frameCount*m_impl->jointCount);
			}
		}
		#if NAZARA_UTILITY_SAFE
		else
		{
			unsigned int endFrame = sequence.firstFrame + sequence.frameCount - 1;
			if (endFrame >= m_impl->frameCount)
			{
				NazaraError("Sequence end frame is over animation end frame");
				return false;
			}
		}
		#endif

		if (!sequence.name.IsEmpty())
		{
			#if NAZARA_UTILITY_SAFE
			auto it = m_impl->sequenceMap.find(sequence.name);
			if (it != m_impl->sequenceMap.end())
			{
				NazaraError("Sequence name \"" + sequence.name + "\" is already used");
				return false;
			}
			#endif

			m_impl->sequenceMap[sequence.name] = m_impl->sequences.size();
		}

		m_impl->sequences.push_back(sequence);

		return true;
	}

	void Animation::AnimateSkeleton(Skeleton* targetSkeleton, unsigned int frameA, unsigned int frameB, float interpolation) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return;
		}

		if (m_impl->type != AnimationType_Skeletal)
		{
			NazaraError("Animation is not skeletal");
			return;
		}

		if (!targetSkeleton || !targetSkeleton->IsValid())
		{
			NazaraError("Target skeleton is invalid");
			return;
		}

		if (targetSkeleton->GetJointCount() != m_impl->jointCount)
		{
			NazaraError("Target skeleton joint count must match animation joint count");
			return;
		}

		if (frameA >= m_impl->frameCount)
		{
			NazaraError("Frame A is out of range (" + String::Number(frameA) + " >= " + String::Number(m_impl->frameCount) + ')');
			return;
		}

		if (frameB >= m_impl->frameCount)
		{
			NazaraError("Frame B is out of range (" + String::Number(frameB) + " >= " + String::Number(m_impl->frameCount) + ')');
			return;
		}
		#endif

		#ifdef NAZARA_DEBUG
		if (interpolation < 0.f || interpolation > 1.f)
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return;
		}
		#endif

		for (unsigned int i = 0; i < m_impl->jointCount; ++i)
		{
			Joint* joint = targetSkeleton->GetJoint(i);

			SequenceJoint& sequenceJointA = m_impl->sequenceJoints[frameA*m_impl->jointCount + i];
			SequenceJoint& sequenceJointB = m_impl->sequenceJoints[frameB*m_impl->jointCount + i];

			joint->SetPosition(Vector3f::Lerp(sequenceJointA.position, sequenceJointB.position, interpolation));
			joint->SetRotation(Quaternionf::Slerp(sequenceJointA.rotation, sequenceJointB.rotation, interpolation));
			joint->SetScale(Vector3f::Lerp(sequenceJointA.scale, sequenceJointB.scale, interpolation));
		}
	}

	bool Animation::CreateSkeletal(unsigned int frameCount, unsigned int jointCount)
	{
		Destroy();

		#if NAZARA_UTILITY_SAFE
		if (frameCount == 0)
		{
			NazaraError("Frame count must be over zero");
			return false;
		}
		#endif

		m_impl = new AnimationImpl;
		m_impl->frameCount = frameCount;
		m_impl->jointCount = jointCount;
		m_impl->sequenceJoints.resize(frameCount*jointCount);
		m_impl->type = AnimationType_Skeletal;

		return true;
	}

	void Animation::Destroy()
	{
		if (m_impl)
		{
			OnAnimationDestroy(this);

			delete m_impl;
			m_impl = nullptr;
		}
	}

	void Animation::EnableLoopPointInterpolation(bool loopPointInterpolation)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return;
		}
		#endif

		m_impl->loopPointInterpolation = loopPointInterpolation;
	}

	unsigned int Animation::GetFrameCount() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return false;
		}
		#endif

		return m_impl->frameCount;
	}

	unsigned int Animation::GetJointCount() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return 0;
		}

		if (m_impl->type != AnimationType_Skeletal)
		{
			NazaraError("Animation is not skeletal");
			return 0;
		}
		#endif

		return m_impl->jointCount;
	}

	Sequence* Animation::GetSequence(const String& sequenceName)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return nullptr;
		}
		#endif

		auto it = m_impl->sequenceMap.find(sequenceName);

		#if NAZARA_UTILITY_SAFE
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("Sequence not found");
			return nullptr;
		}
		#endif

		return &m_impl->sequences[it->second];
	}

	Sequence* Animation::GetSequence(unsigned int index)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return nullptr;
		}

		if (index >= m_impl->sequences.size())
		{
			NazaraError("Sequence index out of range (" + String::Number(index) + " >= " + String::Number(m_impl->sequences.size()) + ')');
			return nullptr;
		}
		#endif

		return &m_impl->sequences[index];
	}

	const Sequence* Animation::GetSequence(const String& sequenceName) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return nullptr;
		}
		#endif

		auto it = m_impl->sequenceMap.find(sequenceName);

		#if NAZARA_UTILITY_SAFE
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("Sequence not found");
			return nullptr;
		}
		#endif

		return &m_impl->sequences[it->second];
	}

	const Sequence* Animation::GetSequence(unsigned int index) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return nullptr;
		}

		if (index >= m_impl->sequences.size())
		{
			NazaraError("Sequence index out of range (" + String::Number(index) + " >= " + String::Number(m_impl->sequences.size()) + ')');
			return nullptr;
		}
		#endif

		return &m_impl->sequences[index];
	}

	unsigned int Animation::GetSequenceCount() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return 0;
		}
		#endif

		return m_impl->sequences.size();
	}

	int Animation::GetSequenceIndex(const String& sequenceName) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return -1;
		}
		#endif

		auto it = m_impl->sequenceMap.find(sequenceName);

		#if NAZARA_UTILITY_SAFE
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("Sequence not found");
			return -1;
		}
		#endif

		return it->second;
	}

	SequenceJoint* Animation::GetSequenceJoints(unsigned int frameIndex)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return nullptr;
		}

		if (m_impl->type != AnimationType_Skeletal)
		{
			NazaraError("Animation is not skeletal");
			return nullptr;
		}
		#endif

		return &m_impl->sequenceJoints[frameIndex*m_impl->jointCount];
	}

	const SequenceJoint* Animation::GetSequenceJoints(unsigned int frameIndex) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return nullptr;
		}

		if (m_impl->type != AnimationType_Skeletal)
		{
			NazaraError("Animation is not skeletal");
			return nullptr;
		}
		#endif

		return &m_impl->sequenceJoints[frameIndex*m_impl->jointCount];
	}

	AnimationType Animation::GetType() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return AnimationType_Static; // Ce qui est une valeur invalide pour Animation
		}
		#endif

		return m_impl->type;
	}

	bool Animation::HasSequence(const String& sequenceName) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return false;
		}
		#endif

		return m_impl->sequenceMap.find(sequenceName) != m_impl->sequenceMap.end();
	}

	bool Animation::HasSequence(unsigned int index) const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return false;
		}
		#endif

		return index >= m_impl->sequences.size();
	}

	bool Animation::IsLoopPointInterpolationEnabled() const
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return false;
		}
		#endif

		return m_impl->loopPointInterpolation;
	}

	bool Animation::IsValid() const
	{
		return m_impl != nullptr;
	}

	bool Animation::LoadFromFile(const String& filePath, const AnimationParams& params)
	{
		return AnimationLoader::LoadFromFile(this, filePath, params);
	}

	bool Animation::LoadFromMemory(const void* data, std::size_t size, const AnimationParams& params)
	{
		return AnimationLoader::LoadFromMemory(this, data, size, params);
	}

	bool Animation::LoadFromStream(Stream& stream, const AnimationParams& params)
	{
		return AnimationLoader::LoadFromStream(this, stream, params);
	}

	void Animation::RemoveSequence(const String& identifier)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return;
		}

		auto it = m_impl->sequenceMap.find(identifier);
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("Sequence not found");
			return;
		}

		int index = it->second;
		#else
		int index = m_impl->sequenceMap[identifier];
		#endif

		auto it2 = m_impl->sequences.begin();
		std::advance(it2, index);

		m_impl->sequences.erase(it2);
	}

	void Animation::RemoveSequence(unsigned int index)
	{
		#if NAZARA_UTILITY_SAFE
		if (!m_impl)
		{
			NazaraError("Animation not created");
			return;
		}

		if (index >= m_impl->sequences.size())
		{
			NazaraError("Sequence index out of range (" + String::Number(index) + " >= " + String::Number(m_impl->sequences.size()) + ')');
			return;
		}
		#endif

		auto it = m_impl->sequences.begin();
		std::advance(it, index);

		m_impl->sequences.erase(it);
	}

	bool Animation::Initialize()
	{
		if (!AnimationLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		if (!AnimationManager::Initialize())
		{
			NazaraError("Failed to initialise manager");
			return false;
		}

		return true;
	}

	void Animation::Uninitialize()
	{
		AnimationManager::Uninitialize();
		AnimationLibrary::Uninitialize();
	}

	AnimationLibrary::LibraryMap Animation::s_library;
	AnimationLoader::LoaderList Animation::s_loaders;
	AnimationManager::ManagerMap Animation::s_managerMap;
	AnimationManager::ManagerParams Animation::s_managerParameters;
}
