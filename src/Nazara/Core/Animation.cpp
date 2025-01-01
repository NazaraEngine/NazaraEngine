// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Animation.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Joint.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <unordered_map>
#include <vector>

namespace Nz
{
	struct AnimationImpl
	{
		std::unordered_map<std::string, std::size_t, StringHash<>, std::equal_to<>> sequenceMap;
		std::vector<Animation::Sequence> sequences;
		std::vector<Animation::SequenceJoint> sequenceJoints; // Uniquement pour les animations squelettiques
		std::size_t frameCount;
		std::size_t jointCount;  // Uniquement pour les animations squelettiques
		AnimationType type;
	};

	bool AnimationParams::IsValid() const
	{
		if (startFrame > endFrame)
		{
			NazaraError("start frame index must be smaller than end frame index");
			return false;
		}

		if (!skeleton)
		{
			NazaraError("you must set a valid skeleton to load an animation");
			return false;
		}

		return true;
	}

	Animation::Animation() = default;
	Animation::Animation(Animation&&) noexcept = default;
	Animation::~Animation() = default;

	bool Animation::AddSequence(Sequence sequence)
	{
		NazaraAssertMsg(m_impl, "animation not created");
		NazaraAssertMsg(sequence.frameCount > 0, "sequence frame count must be over zero");

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
			NazaraAssertMsg(m_impl->sequenceMap.contains(sequence.name), "sequence name \"%s\" is already in use", sequence.name.c_str());
			m_impl->sequenceMap[sequence.name] = static_cast<std::size_t>(m_impl->sequences.size());
		}

		m_impl->sequences.emplace_back(std::move(sequence));

		return true;
	}

	void Animation::AnimateSkeleton(Skeleton* targetSkeleton, std::size_t frameA, std::size_t frameB, float interpolation) const
	{
		NazaraAssertMsg(m_impl, "Animation not created");
		NazaraAssertMsg(m_impl->type == AnimationType::Skeletal, "Animation is not skeletal");
		NazaraAssertMsg(targetSkeleton && targetSkeleton->IsValid(), "invalid skeleton");
		NazaraAssertMsg(targetSkeleton->GetJointCount() == m_impl->jointCount, "skeleton joint does not match animation joint count (%zu != %zu)", targetSkeleton->GetJointCount(), m_impl->jointCount);
		NazaraAssertMsg(frameA < m_impl->frameCount, "Frame A is out of range (%zu >= %zu)", frameA, m_impl->frameCount);
		NazaraAssertMsg(frameB < m_impl->frameCount, "Frame B is out of range (%zu >= %zu)", frameB, m_impl->frameCount);

		Joint* joints = targetSkeleton->GetJoints();
		for (std::size_t i = 0; i < m_impl->jointCount; ++i)
		{
			const SequenceJoint& sequenceJointA = m_impl->sequenceJoints[frameA * m_impl->jointCount + i];
			const SequenceJoint& sequenceJointB = m_impl->sequenceJoints[frameB * m_impl->jointCount + i];

			Joint& joint = joints[i];
			joint.SetPosition(Vector3f::Lerp(sequenceJointA.position, sequenceJointB.position, interpolation), Node::Invalidation::DontInvalidate);
			joint.SetRotation(Quaternionf::Slerp(sequenceJointA.rotation, sequenceJointB.rotation, interpolation), Node::Invalidation::DontInvalidate);
			joint.SetScale(Vector3f::Lerp(sequenceJointA.scale, sequenceJointB.scale, interpolation), Node::Invalidation::DontInvalidate);
		}

		targetSkeleton->GetRootJoint()->Invalidate();
	}

	bool Animation::CreateSkeletal(std::size_t frameCount, std::size_t jointCount)
	{
		NazaraAssertMsg(frameCount > 0, "frame count must be over zero");
		NazaraAssertMsg(jointCount > 0, "frame count must be over zero");

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

	std::size_t Animation::GetFrameCount() const
	{
		NazaraAssertMsg(m_impl, "Animation not created");

		return m_impl->frameCount;
	}

	std::size_t Animation::GetJointCount() const
	{
		NazaraAssertMsg(m_impl, "Animation not created");

		return m_impl->jointCount;
	}

	auto Animation::GetSequence(std::string_view sequenceName) -> Sequence*
	{
		NazaraAssertMsg(m_impl, "Animation not created");

		auto it = m_impl->sequenceMap.find(sequenceName);
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("sequence not found");
			return nullptr;
		}

		return &m_impl->sequences[it->second];
	}

	auto Animation::GetSequence(std::size_t index) -> Sequence*
	{
		NazaraAssertMsg(m_impl, "Animation not created");
		NazaraAssertMsg(index < m_impl->sequences.size(), "Sequence index out of range");

		return &m_impl->sequences[index];
	}

	auto Animation::GetSequence(std::string_view sequenceName) const -> const Sequence*
	{
		NazaraAssertMsg(m_impl, "Animation not created");

		auto it = m_impl->sequenceMap.find(sequenceName);
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("sequence not found");
			return nullptr;
		}

		return &m_impl->sequences[it->second];
	}

	auto Animation::GetSequence(std::size_t index) const -> const Sequence*
	{
		NazaraAssertMsg(m_impl, "Animation not created");
		NazaraAssertMsg(index < m_impl->sequences.size(), "Sequence index out of range");

		return &m_impl->sequences[index];
	}

	std::size_t Animation::GetSequenceCount() const
	{
		NazaraAssertMsg(m_impl, "Animation not created");

		return static_cast<std::size_t>(m_impl->sequences.size());
	}

	std::size_t Animation::GetSequenceIndex(std::string_view sequenceName) const
	{
		NazaraAssertMsg(m_impl, "Animation not created");

		auto it = m_impl->sequenceMap.find(sequenceName);
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("sequence not found");
			return 0xFFFFFFFF;
		}

		return it->second;
	}

	auto Animation::GetSequenceJoints(std::size_t frameIndex) -> SequenceJoint*
	{
		NazaraAssertMsg(m_impl, "Animation not created");
		NazaraAssertMsg(m_impl->type == AnimationType::Skeletal, "Animation is not skeletal");

		return &m_impl->sequenceJoints[frameIndex * m_impl->jointCount];
	}

	auto Animation::GetSequenceJoints(std::size_t frameIndex) const -> const SequenceJoint*
	{
		NazaraAssertMsg(m_impl, "Animation not created");
		NazaraAssertMsg(m_impl->type == AnimationType::Skeletal, "Animation is not skeletal");

		return &m_impl->sequenceJoints[frameIndex * m_impl->jointCount];
	}

	AnimationType Animation::GetType() const
	{
		NazaraAssertMsg(m_impl, "Animation not created");

		return m_impl->type;
	}

	bool Animation::HasSequence(std::string_view sequenceName) const
	{
		NazaraAssertMsg(m_impl, "Animation not created");

		return m_impl->sequenceMap.contains(sequenceName);
	}

	bool Animation::HasSequence(std::size_t index) const
	{
		NazaraAssertMsg(m_impl, "Animation not created");

		return index >= m_impl->sequences.size();
	}

	bool Animation::IsValid() const
	{
		return m_impl != nullptr;
	}

	void Animation::RemoveSequence(std::string_view identifier)
	{
		NazaraAssertMsg(m_impl, "Animation not created");

		auto it = m_impl->sequenceMap.find(identifier);
		if (it == m_impl->sequenceMap.end())
		{
			NazaraError("sequence not found");
			return;
		}

		RemoveSequence(it->second);
		m_impl->sequenceMap.erase(it);
	}

	void Animation::RemoveSequence(std::size_t index)
	{
		NazaraAssertMsg(m_impl, "Animation not created");
		NazaraAssertMsg(index < m_impl->sequences.size(), "Sequence index out of range");

		m_impl->sequences.erase(m_impl->sequences.begin() + index);

		// Shift indices
		for (auto& it : m_impl->sequenceMap)
		{
			if (it.second > index)
				it.second--;
		}
	}

	Animation& Animation::operator=(Animation&&) noexcept = default;

	std::shared_ptr<Animation> Animation::LoadFromFile(const std::filesystem::path& filePath, const AnimationParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssertMsg(core, "Core module has not been initialized");

		return core->GetAnimationLoader().LoadFromFile(filePath, params);
	}

	std::shared_ptr<Animation> Animation::LoadFromMemory(const void* data, std::size_t size, const AnimationParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssertMsg(core, "Core module has not been initialized");

		return core->GetAnimationLoader().LoadFromMemory(data, size, params);
	}

	std::shared_ptr<Animation> Animation::LoadFromStream(Stream& stream, const AnimationParams& params)
	{
		Core* core = Core::Instance();
		NazaraAssertMsg(core, "Core module has not been initialized");

		return core->GetAnimationLoader().LoadFromStream(stream, params);
	}
}
