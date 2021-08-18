// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Utility/Sequence.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <vector>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	struct AnimationImpl
	{
		std::unordered_map<std::string, std::size_t> sequenceMap;
		std::vector<Sequence> sequences;
		std::vector<SequenceJoint> sequenceJoints; // Uniquement pour les animations squelettiques
		AnimationType type;
		bool loopPointInterpolation = false;
		std::size_t frameCount;
		std::size_t jointCount;  // Uniquement pour les animations squelettiques
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

	Animation::Animation() = default;
	Animation::Animation(Animation&&) noexcept = default;
	Animation::~Animation() = default;

	bool Animation::AddSequence(const Sequence& sequence)
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(sequence.frameCount > 0, "Sequence frame count must be over zero");

		if (m_impl->type == AnimationType::Skeletal)
		{
			std::size_t endFrame = sequence.firstFrame + sequence.frameCount - 1;
			if (endFrame >= m_impl->frameCount)
			{
				m_impl->frameCount = endFrame+1;
				m_impl->sequenceJoints.resize(m_impl->frameCount*m_impl->jointCount);
			}
		}

		if (!sequence.name.empty())
		{
			#if NAZARA_UTILITY_SAFE
			auto it = m_impl->sequenceMap.find(sequence.name);
			if (it != m_impl->sequenceMap.end())
			{
				NazaraError("Sequence name \"" + sequence.name + "\" is already in use");
				return false;
			}
			#endif

			m_impl->sequenceMap[sequence.name] = static_cast<std::size_t>(m_impl->sequences.size());
		}

		m_impl->sequences.push_back(sequence);

		return true;
	}

	void Animation::AnimateSkeleton(Skeleton* targetSkeleton, std::size_t frameA, std::size_t frameB, float interpolation) const
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(m_impl->type == AnimationType::Skeletal, "Animation is not skeletal");
		NazaraAssert(targetSkeleton && targetSkeleton->IsValid(), "Invalid skeleton");
		NazaraAssert(targetSkeleton->GetJointCount() == m_impl->jointCount, "Skeleton joint does not match animation joint count");
		NazaraAssert(frameA < m_impl->frameCount, "FrameA is out of range");
		NazaraAssert(frameB < m_impl->frameCount, "FrameB is out of range");

		for (std::size_t i = 0; i < m_impl->jointCount; ++i)
		{
			Joint* joint = targetSkeleton->GetJoint(i);

			const SequenceJoint& sequenceJointA = m_impl->sequenceJoints[frameA*m_impl->jointCount + i];
			const SequenceJoint& sequenceJointB = m_impl->sequenceJoints[frameB*m_impl->jointCount + i];

			joint->SetPosition(Vector3f::Lerp(sequenceJointA.position, sequenceJointB.position, interpolation));
			joint->SetRotation(Quaternionf::Slerp(sequenceJointA.rotation, sequenceJointB.rotation, interpolation));
			joint->SetScale(Vector3f::Lerp(sequenceJointA.scale, sequenceJointB.scale, interpolation));
		}
	}

	bool Animation::CreateSkeletal(std::size_t frameCount, std::size_t jointCount)
	{
		NazaraAssert(frameCount > 0, "Frame count must be over zero");
		NazaraAssert(jointCount > 0, "Frame count must be over zero");

		Destroy();

		m_impl = std::make_unique<AnimationImpl>();
		m_impl->frameCount = frameCount;
		m_impl->jointCount = jointCount;
		m_impl->sequenceJoints.resize(frameCount*jointCount);
		m_impl->type = AnimationType::Skeletal;

		return true;
	}

	void Animation::Destroy()
	{
		m_impl.reset();
	}

	void Animation::EnableLoopPointInterpolation(bool loopPointInterpolation)
	{
		NazaraAssert(m_impl, "Animation not created");

		m_impl->loopPointInterpolation = loopPointInterpolation;
	}

	std::size_t Animation::GetFrameCount() const
	{
		NazaraAssert(m_impl, "Animation not created");

		return m_impl->frameCount;
	}

	std::size_t Animation::GetJointCount() const
	{
		NazaraAssert(m_impl, "Animation not created");

		return m_impl->jointCount;
	}

	Sequence* Animation::GetSequence(const std::string& sequenceName)
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

	Sequence* Animation::GetSequence(std::size_t index)
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(index < m_impl->sequences.size(), "Sequence index out of range");

		return &m_impl->sequences[index];
	}

	const Sequence* Animation::GetSequence(const std::string& sequenceName) const
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

	const Sequence* Animation::GetSequence(std::size_t index) const
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(index < m_impl->sequences.size(), "Sequence index out of range");

		return &m_impl->sequences[index];
	}

	std::size_t Animation::GetSequenceCount() const
	{
		NazaraAssert(m_impl, "Animation not created");

		return static_cast<std::size_t>(m_impl->sequences.size());
	}

	std::size_t Animation::GetSequenceIndex(const std::string& sequenceName) const
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

	SequenceJoint* Animation::GetSequenceJoints(std::size_t frameIndex)
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(m_impl->type == AnimationType::Skeletal, "Animation is not skeletal");

		return &m_impl->sequenceJoints[frameIndex*m_impl->jointCount];
	}

	const SequenceJoint* Animation::GetSequenceJoints(std::size_t frameIndex) const
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(m_impl->type == AnimationType::Skeletal, "Animation is not skeletal");

		return &m_impl->sequenceJoints[frameIndex*m_impl->jointCount];
	}

	AnimationType Animation::GetType() const
	{
		NazaraAssert(m_impl, "Animation not created");

		return m_impl->type;
	}

	bool Animation::HasSequence(const std::string& sequenceName) const
	{
		NazaraAssert(m_impl, "Animation not created");

		return m_impl->sequenceMap.find(sequenceName) != m_impl->sequenceMap.end();
	}

	bool Animation::HasSequence(std::size_t index) const
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

	void Animation::RemoveSequence(const std::string& identifier)
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

	void Animation::RemoveSequence(std::size_t index)
	{
		NazaraAssert(m_impl, "Animation not created");
		NazaraAssert(index < m_impl->sequences.size(), "Sequence index out of range");

		auto it = m_impl->sequences.begin();
		std::advance(it, index);

		m_impl->sequences.erase(it);
	}

	Animation& Animation::operator=(Animation&&) noexcept = default;

	std::shared_ptr<Animation> Animation::LoadFromFile(const std::filesystem::path& filePath, const AnimationParams& params)
	{
		Utility* utility = Utility::Instance();
		NazaraAssert(utility, "Utility module has not been initialized");

		return utility->GetAnimationLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<Animation> Animation::LoadFromMemory(const void* data, std::size_t size, const AnimationParams& params)
	{
		Utility* utility = Utility::Instance();
		NazaraAssert(utility, "Utility module has not been initialized");

		return utility->GetAnimationLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<Animation> Animation::LoadFromStream(Stream& stream, const AnimationParams& params)
	{
		Utility* utility = Utility::Instance();
		NazaraAssert(utility, "Utility module has not been initialized");

		return utility->GetAnimationLoader().LoadFromStream(stream, params);
	}
}
