// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline AnimationBlender::AnimationBlender(const Nz::Skeleton& referenceSkeleton) :
	m_animationProgress(0.f),
	m_blendingFactor(0.f),
	m_currentValue(0.f),
	m_targetValue(0.f),
	m_valueIncrease(30.f)
	{
		for (AnimationData& animData : m_animData)
			animData.skeleton = referenceSkeleton;
	}

	inline void AnimationBlender::UpdateValue(float value)
	{
		m_targetValue = value;
	}

	inline void AnimationBlender::UpdateValueIncrease(float increasePerSecond)
	{
		m_valueIncrease = increasePerSecond;
	}
}
