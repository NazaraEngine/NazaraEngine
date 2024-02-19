// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ANIMATION_HPP
#define NAZARA_CORE_ANIMATION_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <NazaraUtils/Signal.hpp>
#include <string>

namespace Nz
{
	class Skeleton;

	struct NAZARA_CORE_API AnimationParams : ResourceParameters
	{
		// Last frame to load (maximum)
		std::size_t endFrame = 0xFFFFFFFF;
		// First frame to load
		std::size_t startFrame = 0;
		// Reference skeleton
		const Skeleton* skeleton = nullptr;

		// Transform joints by these transformations
		Vector3f jointOffset = Vector3f::Zero();

		Quaternionf jointRotation = Quaternionf::Identity();

		Vector3f jointScale = Vector3f::Unit();

		bool IsValid() const;
	};

	class Animation;
	struct Sequence;
	struct SequenceJoint;

	using AnimationLibrary = ObjectLibrary<Animation>;
	using AnimationLoader = ResourceLoader<Animation, AnimationParams>;
	using AnimationManager = ResourceManager<Animation, AnimationParams>;

	struct AnimationImpl;

	class NAZARA_CORE_API Animation : public Resource
	{
		public:
			using Params = AnimationParams;

			Animation();
			Animation(const Animation&) = delete;
			Animation(Animation&&) noexcept;
			~Animation();

			bool AddSequence(Sequence sequence);
			void AnimateSkeleton(Skeleton* targetSkeleton, std::size_t frameA, std::size_t frameB, float interpolation) const;

			bool CreateSkeletal(std::size_t frameCount, std::size_t jointCount);
			void Destroy();

			std::size_t GetFrameCount() const;
			std::size_t GetJointCount() const;
			Sequence* GetSequence(std::string_view sequenceName);
			Sequence* GetSequence(std::size_t index);
			const Sequence* GetSequence(std::string_view sequenceName) const;
			const Sequence* GetSequence(std::size_t index) const;
			std::size_t GetSequenceCount() const;
			std::size_t GetSequenceIndex(std::string_view sequenceName) const;
			SequenceJoint* GetSequenceJoints(std::size_t frameIndex = 0);
			const SequenceJoint* GetSequenceJoints(std::size_t frameIndex = 0) const;
			AnimationType GetType() const;

			bool HasSequence(std::string_view sequenceName) const;
			bool HasSequence(std::size_t index = 0) const;

			bool IsValid() const;

			void RemoveSequence(std::string_view sequenceName);
			void RemoveSequence(std::size_t index);

			Animation& operator=(const Animation&) = delete;
			Animation& operator=(Animation&&) noexcept;

			static std::shared_ptr<Animation> LoadFromFile(const std::filesystem::path& filePath, const AnimationParams& params = AnimationParams());
			static std::shared_ptr<Animation> LoadFromMemory(const void* data, std::size_t size, const AnimationParams& params = AnimationParams());
			static std::shared_ptr<Animation> LoadFromStream(Stream& stream, const AnimationParams& params = AnimationParams());

		private:
			std::unique_ptr<AnimationImpl> m_impl;
	};
}

#include <Nazara/Core/Animation.inl>

#endif // NAZARA_CORE_ANIMATION_HPP
