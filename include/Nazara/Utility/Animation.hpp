// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ANIMATION_HPP
#define NAZARA_ANIMATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Sequence.hpp>

struct NAZARA_API NzAnimationParams
{
	// La frame de fin à charger
	unsigned int endFrame = static_cast<unsigned int>(-1);
	// La frame de début à charger
	unsigned int startFrame = 0;

	bool IsValid() const;
};

class NzAnimation;
class NzSkeleton;

using NzAnimationConstRef = NzObjectRef<const NzAnimation>;
using NzAnimationLoader = NzResourceLoader<NzAnimation, NzAnimationParams>;
using NzAnimationRef = NzObjectRef<NzAnimation>;

struct NzAnimationImpl;

class NAZARA_API NzAnimation : public NzRefCounted, public NzResource
{
	friend NzAnimationLoader;

	public:
		NzAnimation() = default;
		~NzAnimation();

		bool AddSequence(const NzSequence& sequence);
		void AnimateSkeleton(NzSkeleton* targetSkeleton, unsigned int frameA, unsigned int frameB, float interpolation) const;

		bool CreateSkeletal(unsigned int frameCount, unsigned int jointCount);
		void Destroy();

		void EnableLoopPointInterpolation(bool loopPointInterpolation);

		unsigned int GetFrameCount() const;
		unsigned int GetJointCount() const;
		NzSequence* GetSequence(const NzString& sequenceName);
		NzSequence* GetSequence(unsigned int index);
		const NzSequence* GetSequence(const NzString& sequenceName) const;
		const NzSequence* GetSequence(unsigned int index) const;
		unsigned int GetSequenceCount() const;
		int GetSequenceIndex(const NzString& sequenceName) const;
		NzSequenceJoint* GetSequenceJoints(unsigned int frameIndex = 0);
		const NzSequenceJoint* GetSequenceJoints(unsigned int frameIndex = 0) const;
		nzAnimationType GetType() const;

		bool HasSequence(const NzString& sequenceName) const;
		bool HasSequence(unsigned int index = 0) const;

		bool IsLoopPointInterpolationEnabled() const;
		bool IsValid() const;

		bool LoadFromFile(const NzString& filePath, const NzAnimationParams& params = NzAnimationParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzAnimationParams& params = NzAnimationParams());
		bool LoadFromStream(NzInputStream& stream, const NzAnimationParams& params = NzAnimationParams());

		void RemoveSequence(const NzString& sequenceName);
		void RemoveSequence(unsigned int index);

	private:
		NzAnimationImpl* m_impl = nullptr;

		static NzAnimationLoader::LoaderList s_loaders;
};

#endif // NAZARA_ANIMATION_HPP
