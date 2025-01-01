// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_ANIMATIONBLENDER_HPP
#define NAZARA_CORE_ANIMATIONBLENDER_HPP

#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Core/Time.hpp>
#include <array>
#include <memory>
#include <vector>

namespace Nz
{
	class Animation;

	class NAZARA_CORE_API AnimationBlender
	{
		public:
			inline AnimationBlender(const Nz::Skeleton& referenceSkeleton);
			AnimationBlender(const AnimationBlender&) = delete;
			AnimationBlender(AnimationBlender&&) noexcept = default;
			~AnimationBlender() = default;

			void AddPoint(float value, std::shared_ptr<const Nz::Animation> animation, std::size_t sequenceIndex = 0, float speedFactor = 1.f);
			void AnimateSkeleton(Nz::Skeleton* skeleton) const;

			void UpdateAnimation(Nz::Time elapsedTime);
			inline void UpdateValue(float value);
			inline void UpdateValueIncrease(float increasePerSecond);

			AnimationBlender& operator=(const AnimationBlender&) = delete;
			AnimationBlender& operator=(AnimationBlender&&) noexcept = default;

		private:
			void RefreshPoints(float deltaTime);

			struct AnimationData
			{
				Nz::Skeleton skeleton;
				std::size_t pointIndex = 0;
			};

			struct Point
			{
				std::shared_ptr<const Nz::Animation> animation;
				std::size_t sequenceIndex;
				float speed;
				float value;
			};

			std::array<AnimationData, 2> m_animData;
			std::vector<Point> m_points;
			float m_animationProgress;
			float m_blendingFactor;
			float m_currentValue;
			float m_targetValue;
			float m_valueIncrease;
	};
}

#include <Nazara/Core/AnimationBlender.inl>

#endif // NAZARA_CORE_ANIMATIONBLENDER_HPP
