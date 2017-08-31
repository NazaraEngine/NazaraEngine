// Copyright (C) 2017 Jérôme Leclercq
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
		std::unordered_map<String, UInt32> sequenceMap;
		std::vector<Sequence> sequences;
		std::vector<SequenceJoint> sequenceJoints; // Uniquement pour les animations squelettiques
		AnimationType type;
		bool loopPointInterpolation = false;
		UInt32 frameCount;
		UInt32 jointCount;  // Uniquement pour les animations squelettiques
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
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(sequence.frameCount > 0, "Sequence frame count must be over zero");

		if (m_impl->type == AnimationType_Skeletal)
		{
			UInt32 endFrame = sequence.firstFrame + sequence.frameCount - 1;
			if (endFrame >= m_impl->frameCount)
			{
				m_impl->frameCount = endFrame+1;
				m_impl->sequenceJoints.resize(m_impl->frameCount*m_impl->jointCount);
			}
		}

		if (!sequence.name.IsEmpty())
		{
			#if NAZARA_UTILITY_SAFE
			auto it = m_impl->sequenceMap.find(sequence.name);
			if (it != m_impl->sequenceMap.end())
			{
				NazaraError("Sequence name \"" + sequence.name + "\" is already in use");
				return false;
			}
			#endif

			m_impl->sequenceMap[sequence.name] = static_cast<UInt32>(m_impl->sequences.size());
		}

		m_impl->sequences.push_back(sequence);

		return true;
	}

	void Animation::AnimateSkeleton(Skeleton* targetSkeleton, UInt32 frameA, UInt32 frameB, float interpolation) const
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(m_impl->type == AnimationType_Skeletal, "Animation is not skeletal");
		NazaraAssert(targetSkeleton && targetSkeleton->IsValid(), "Invalid skeleton");
		NazaraAssert(targetSkeleton->GetJointCount() == m_impl->jointCount, "Skeleton joint does not match animation joint count");
		NazaraAssert(frameA < m_impl->frameCount, "FrameA is out of range");
		NazaraAssert(frameB < m_impl->frameCount, "FrameB is out of range");

		for (UInt32 i = 0; i < m_impl->jointCount; ++i)
		{
			Joint* joint = targetSkeleton->GetJoint(i);

			SequenceJoint& sequenceJointA = m_impl->sequenceJoints[frameA*m_impl->jointCount + i];
			SequenceJoint& sequenceJointB = m_impl->sequenceJoints[frameB*m_impl->jointCount + i];

			joint->SetPosition(Vector3f::Lerp(sequenceJointA.position, sequenceJointB.position, interpolation));
			joint->SetRotation(Quaternionf::Slerp(sequenceJointA.rotation, sequenceJointB.rotation, interpolation));
			joint->SetScale(Vector3f::Lerp(sequenceJointA.scale, sequenceJointB.scale, interpolation));
		}
	}

	bool Animation::CreateSkeletal(UInt32 frameCount, UInt32 jointCount)
	{
		NazaraAssert(frameCount > 0, "Frame count must be over zero");
		NazaraAssert(jointCount > 0, "Frame count must be over zero");

		Destroy();

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
		NazaraAssert(m_impl, "Animation not created");

		m_impl->loopPointInterpolation = loopPointInterpolation;
	}

	UInt32 Animation::GetFrameCount() const
	{
		NazaraAssert(m_impl, "Animation not created");

		return m_impl->frameCount;
	}

	UInt32 Animation::GetJointCount() const
	{
		NazaraAssert(m_impl, "Animation not created");

		return m_impl->jointCount;
	}

	Sequence* Animation::GetSequence(const String& sequenceName)
	{
		NazaraAssert(m_impl, "Animation not created");

		auto it = m_impl->sequenceMap.find(sequenceName);
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("Sequence not found");
			return nullptr;
		}

		return &m_impl->sequences[it->second];
	}

	Sequence* Animation::GetSequence(UInt32 index)
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(index < m_impl->sequences.size(), "Sequence index out of range");

		return &m_impl->sequences[index];
	}

	const Sequence* Animation::GetSequence(const String& sequenceName) const
	{
		NazaraAssert(m_impl, "Animation not created");

		auto it = m_impl->sequenceMap.find(sequenceName);
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("Sequence not found");
			return nullptr;
		}

		return &m_impl->sequences[it->second];
	}

	const Sequence* Animation::GetSequence(UInt32 index) const
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(index < m_impl->sequences.size(), "Sequence index out of range");

		return &m_impl->sequences[index];
	}

	UInt32 Animation::GetSequenceCount() const
	{
		NazaraAssert(m_impl, "Animation not created");

		return static_cast<UInt32>(m_impl->sequences.size());
	}

	UInt32 Animation::GetSequenceIndex(const String& sequenceName) const
	{
		NazaraAssert(m_impl, "Animation not created");

		auto it = m_impl->sequenceMap.find(sequenceName);
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("Sequence not found");
			return 0xFFFFFFFF;
		}

		return it->second;
	}

	SequenceJoint* Animation::GetSequenceJoints(UInt32 frameIndex)
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(m_impl->type == AnimationType_Skeletal, "Animation is not skeletal");

		return &m_impl->sequenceJoints[frameIndex*m_impl->jointCount];
	}

	const SequenceJoint* Animation::GetSequenceJoints(UInt32 frameIndex) const
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(m_impl->type == AnimationType_Skeletal, "Animation is not skeletal");

		return &m_impl->sequenceJoints[frameIndex*m_impl->jointCount];
	}

	AnimationType Animation::GetType() const
	{
		NazaraAssert(m_impl, "Animation not created");

		return m_impl->type;
	}

	bool Animation::HasSequence(const String& sequenceName) const
	{
		NazaraAssert(m_impl, "Animation not created");

		return m_impl->sequenceMap.find(sequenceName) != m_impl->sequenceMap.end();
	}

	bool Animation::HasSequence(UInt32 index) const
	{
		NazaraAssert(m_impl, "Animation not created");

		return index >= m_impl->sequences.size();
	}

	bool Animation::IsLoopPointInterpolationEnabled() const
	{
		NazaraAssert(m_impl, "Animation not created");

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
		NazaraAssert(m_impl, "Animation not created");

		auto it = m_impl->sequenceMap.find(identifier);
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("Sequence not found");
			return;
		}

		auto sequenceIt = m_impl->sequences.begin();
		std::advance(sequenceIt, it->second);

		m_impl->sequences.erase(sequenceIt);
	}

	void Animation::RemoveSequence(UInt32 index)
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(index < m_impl->sequences.size(), "Sequence index out of range");

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
