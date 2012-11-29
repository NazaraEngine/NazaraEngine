// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <map>
#include <vector>
#include <Nazara/Utility/Debug.hpp>

struct NzAnimationImpl
{
	std::map<NzString, unsigned int> sequenceMap;
	std::vector<NzSequence> sequences;
	std::vector<NzSequenceJoint> sequenceJoints; // Uniquement pour les animations squelettiques
	nzAnimationType type;
	unsigned int frameCount;
	unsigned int jointCount;  // Uniquement pour les animations squelettiques
};

bool NzAnimationParams::IsValid() const
{
	if (startFrame > endFrame)
	{
		NazaraError("Start frame index must be smaller than end frame index");
		return false;
	}

	return true;
}

NzAnimation::~NzAnimation()
{
	Destroy();
}

bool NzAnimation::AddSequence(const NzSequence& sequence)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return false;
	}

	if (sequence.frameCount == 0)
	{
		NazaraError("Sequence frame count must be over 0");
		return false;
	}
	#endif

	if (m_impl->type == nzAnimationType_Skeletal)
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

void NzAnimation::AnimateSkeleton(NzSkeleton* targetSkeleton, unsigned int frameA, unsigned int frameB, float interpolation) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return;
	}

	if (m_impl->type != nzAnimationType_Skeletal)
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
		NazaraError("Frame A is out of range (" + NzString::Number(frameA) + " >= " + NzString::Number(m_impl->frameCount) + ')');
		return;
	}

	if (frameB >= m_impl->frameCount)
	{
		NazaraError("Frame B is out of range (" + NzString::Number(frameB) + " >= " + NzString::Number(m_impl->frameCount) + ')');
		return;
	}
	#endif

	#ifdef NAZARA_DEBUG
	if (interpolation < 0.f || interpolation > 1.f)
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return;
	}
	#endif

	for (unsigned int i = 0; i < m_impl->jointCount; ++i)
	{
		NzJoint* joint = targetSkeleton->GetJoint(i);

		NzSequenceJoint& sequenceJointA = m_impl->sequenceJoints[frameA*m_impl->jointCount + i];
		NzSequenceJoint& sequenceJointB = m_impl->sequenceJoints[frameB*m_impl->jointCount + i];

		joint->SetRotation(NzQuaternionf::Slerp(sequenceJointA.rotation, sequenceJointB.rotation, interpolation));
		joint->SetScale(NzVector3f::Lerp(sequenceJointA.scale, sequenceJointB.scale, interpolation));
		joint->SetTranslation(NzVector3f::Lerp(sequenceJointA.translation, sequenceJointB.translation, interpolation));
	}
}

bool NzAnimation::CreateKeyframe(unsigned int frameCount)
{
	Destroy();

	#if NAZARA_UTILITY_SAFE
	if (frameCount == 0)
	{
		NazaraError("Frame count must be over zero");
		return false;
	}
	#endif

	m_impl = new NzAnimationImpl;
	m_impl->frameCount = frameCount;
	m_impl->type = nzAnimationType_Keyframe;

	NotifyCreated();
	return true;
}

bool NzAnimation::CreateSkeletal(unsigned int frameCount, unsigned int jointCount)
{
	Destroy();

	#if NAZARA_UTILITY_SAFE
	if (frameCount == 0)
	{
		NazaraError("Frame count must be over zero");
		return false;
	}
	#endif

	m_impl = new NzAnimationImpl;
	m_impl->frameCount = frameCount;
	m_impl->jointCount = jointCount;
	m_impl->sequenceJoints.resize(frameCount*jointCount);
	m_impl->type = nzAnimationType_Skeletal;

	NotifyCreated();
	return true;
}

void NzAnimation::Destroy()
{
	if (m_impl)
	{
		NotifyDestroy();

		delete m_impl;
		m_impl = nullptr;
	}
}

unsigned int NzAnimation::GetFrameCount() const
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

unsigned int NzAnimation::GetJointCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return 0;
	}

	if (m_impl->type != nzAnimationType_Skeletal)
	{
		NazaraError("Animation is not skeletal");
		return 0;
	}
	#endif

	return m_impl->jointCount;
}

NzSequence* NzAnimation::GetSequence(const NzString& sequenceName)
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

NzSequence* NzAnimation::GetSequence(unsigned int index)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nullptr;
	}

	if (index >= m_impl->sequences.size())
	{
		NazaraError("Sequence index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->sequences.size()) + ')');
		return nullptr;
	}
	#endif

	return &m_impl->sequences[index];
}

const NzSequence* NzAnimation::GetSequence(const NzString& sequenceName) const
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

const NzSequence* NzAnimation::GetSequence(unsigned int index) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nullptr;
	}

	if (index >= m_impl->sequences.size())
	{
		NazaraError("Sequence index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->sequences.size()) + ')');
		return nullptr;
	}
	#endif

	return &m_impl->sequences[index];
}

unsigned int NzAnimation::GetSequenceCount() const
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

int NzAnimation::GetSequenceIndex(const NzString& sequenceName) const
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

NzSequenceJoint* NzAnimation::GetSequenceJoints(unsigned int frameIndex)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nullptr;
	}

	if (m_impl->type != nzAnimationType_Skeletal)
	{
		NazaraError("Animation is not skeletal");
		return nullptr;
	}
	#endif

	return &m_impl->sequenceJoints[frameIndex*m_impl->jointCount];
}

const NzSequenceJoint* NzAnimation::GetSequenceJoints(unsigned int frameIndex) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nullptr;
	}

	if (m_impl->type != nzAnimationType_Skeletal)
	{
		NazaraError("Animation is not skeletal");
		return nullptr;
	}
	#endif

	return &m_impl->sequenceJoints[frameIndex*m_impl->jointCount];
}

nzAnimationType NzAnimation::GetType() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return nzAnimationType_Static; // Ce qui est une valeur invalide pour NzAnimation
	}
	#endif

	return m_impl->type;
}

bool NzAnimation::HasSequence(const NzString& sequenceName) const
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

bool NzAnimation::HasSequence(unsigned int index) const
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

bool NzAnimation::IsValid() const
{
	return m_impl != nullptr;
}

bool NzAnimation::LoadFromFile(const NzString& filePath, const NzAnimationParams& params)
{
	return NzAnimationLoader::LoadFromFile(this, filePath, params);
}

bool NzAnimation::LoadFromMemory(const void* data, std::size_t size, const NzAnimationParams& params)
{
	return NzAnimationLoader::LoadFromMemory(this, data, size, params);
}

bool NzAnimation::LoadFromStream(NzInputStream& stream, const NzAnimationParams& params)
{
	return NzAnimationLoader::LoadFromStream(this, stream, params);
}

void NzAnimation::RemoveSequence(const NzString& identifier)
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

void NzAnimation::RemoveSequence(unsigned int index)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Animation not created");
		return;
	}

	if (index >= m_impl->sequences.size())
	{
		NazaraError("Sequence index out of range (" + NzString::Number(index) + " >= " + NzString::Number(m_impl->sequences.size()) + ')');
		return;
	}
	#endif

	auto it = m_impl->sequences.begin();
	std::advance(it, index);

	m_impl->sequences.erase(it);
}

NzAnimationLoader::LoaderList NzAnimation::s_loaders;
