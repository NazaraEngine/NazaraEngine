// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ANIMATION_HPP
#define NAZARA_ANIMATION_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <string>

namespace Nz
{
	struct NAZARA_UTILITY_API AnimationParams : ResourceParameters
	{
		// La frame de fin à charger
		std::size_t endFrame = 0xFFFFFFFF;
		// La frame de début à charger
		std::size_t startFrame = 0;

		bool IsValid() const;
	};

	class Animation;
	struct Sequence;
	struct SequenceJoint;
	class Skeleton;

	using AnimationLibrary = ObjectLibrary<Animation>;
	using AnimationLoader = ResourceLoader<Animation, AnimationParams>;
	using AnimationManager = ResourceManager<Animation, AnimationParams>;

	struct AnimationImpl;

	class NAZARA_UTILITY_API Animation : public Resource
	{
		public:
			Animation();
			Animation(const Animation&) = delete;
			Animation(Animation&&) noexcept;
			~Animation();

			bool AddSequence(const Sequence& sequence);
			void AnimateSkeleton(Skeleton* targetSkeleton, std::size_t frameA, std::size_t frameB, float interpolation) const;

			bool CreateSkeletal(std::size_t frameCount, std::size_t jointCount);
			void Destroy();

			void EnableLoopPointInterpolation(bool loopPointInterpolation);

			std::size_t GetFrameCount() const;
			std::size_t GetJointCount() const;
			Sequence* GetSequence(const std::string& sequenceName);
			Sequence* GetSequence(std::size_t index);
			const Sequence* GetSequence(const std::string& sequenceName) const;
			const Sequence* GetSequence(std::size_t index) const;
			std::size_t GetSequenceCount() const;
			std::size_t GetSequenceIndex(const std::string& sequenceName) const;
			SequenceJoint* GetSequenceJoints(std::size_t frameIndex = 0);
			const SequenceJoint* GetSequenceJoints(std::size_t frameIndex = 0) const;
			AnimationType GetType() const;

			bool HasSequence(const std::string& sequenceName) const;
			bool HasSequence(std::size_t index = 0) const;

			bool IsLoopPointInterpolationEnabled() const;
			bool IsValid() const;

			void RemoveSequence(const std::string& sequenceName);
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

#include <Nazara/Utility/Animation.inl>

#endif // NAZARA_ANIMATION_HPP
