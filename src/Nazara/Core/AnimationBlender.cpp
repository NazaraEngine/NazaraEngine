// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/AnimationBlender.hpp>
#include <Nazara/Core/Animation.hpp>
#include <algorithm>
#include <cmath>

namespace Nz
{
	void AnimationBlender::AddPoint(float value, std::shared_ptr<const Animation> animation, std::size_t sequenceIndex, float speedFactor)
	{
		const Animation::Sequence* sequence = animation->GetSequence(sequenceIndex);
		float animSpeed = speedFactor * float(sequence->frameRate) / sequence->frameCount;

		auto it = std::lower_bound(m_points.begin(), m_points.end(), value, [](const auto& point, float value)
		{
			return point.value < value;
		});

		m_points.insert(it, {
			.animation = std::move(animation),
			.sequenceIndex = sequenceIndex,
			.speed = animSpeed,
			.value = value
		});
	}

	void AnimationBlender::AnimateSkeleton(Skeleton* skeleton) const
	{
		if (m_blendingFactor != 0.f)
			skeleton->Interpolate(m_animData[0].skeleton, m_animData[1].skeleton, m_blendingFactor);
		else
			skeleton->CopyPose(m_animData[0].skeleton); //< in case we only use one animation+sequence
	}

	void AnimationBlender::UpdateAnimation(Time elapsedTime)
	{
		if (m_points.empty())
			return;

		float deltaTime = elapsedTime.AsSeconds();
		RefreshPoints(deltaTime);

		const auto& pointA = m_points[m_animData[0].pointIndex];
		const auto& pointB = m_points[m_animData[1].pointIndex];
		float animationSpeed = Lerp(pointA.speed, pointB.speed, m_blendingFactor);

		m_animationProgress = std::fmod(m_animationProgress + animationSpeed * deltaTime, 1.f);

		auto AnimateSkeleton = [this](const Point& point, Nz::Skeleton* targetSkeleton)
		{
			const Animation::Sequence* sequence = point.animation->GetSequence(point.sequenceIndex);

			float frameIndex = m_animationProgress * sequence->frameCount;

			std::size_t currentFrame = static_cast<std::size_t>(frameIndex);
			std::size_t nextFrame = currentFrame + 1;
			if (nextFrame >= sequence->firstFrame + sequence->frameCount)
				nextFrame = sequence->firstFrame;
			float interp = frameIndex - std::floor(frameIndex);

			point.animation->AnimateSkeleton(targetSkeleton, currentFrame, nextFrame, interp);
		};

		if (pointA.animation != pointB.animation || pointA.sequenceIndex != pointB.sequenceIndex)
		{
			for (std::size_t i : { 0, 1 })
			{
				const auto& point = m_points[m_animData[i].pointIndex];
				AnimateSkeleton(point, &m_animData[i].skeleton);
			}
		}
		else
		{
			// If both points use the same animation and sequence, we can optimize a bit by computing animation only once
			AnimateSkeleton(pointA, &m_animData[0].skeleton);
			m_blendingFactor = 0.f;
		}
	}

	void AnimationBlender::RefreshPoints(float deltaTime)
	{
		m_currentValue = Approach(m_currentValue, m_targetValue, m_valueIncrease * deltaTime);
		m_currentValue = std::clamp(m_currentValue, m_points.front().value, m_points.back().value);

		std::size_t pointIndexA = m_points.size() - 1;
		std::size_t pointIndexB = pointIndexA;
		for (std::size_t i = 0; i < m_points.size(); ++i)
		{
			if (m_points[i].value > m_currentValue)
			{
				pointIndexA = (i > 0) ? i - 1 : 0;
				pointIndexB = i;
				break;
			}
		}

		m_animData[0].pointIndex = pointIndexA;
		m_animData[1].pointIndex = pointIndexB;

		if NAZARA_LIKELY(pointIndexA != pointIndexB)
		{
			const auto& pointA = m_points[m_animData[0].pointIndex];
			const auto& pointB = m_points[m_animData[1].pointIndex];

			m_blendingFactor = std::clamp(static_cast<float>(m_currentValue - pointA.value) / (pointB.value - pointA.value), 0.f, 1.f);
		}
		else
			m_blendingFactor = 0.f;
	}
}
