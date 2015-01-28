// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ANIMATION_HPP
#define NAZARA_ANIMATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Sequence.hpp>
#include <limits>

struct NAZARA_API NzAnimationParams
{
	// La frame de fin à charger
	unsigned int endFrame = std::numeric_limits<unsigned int>::max();
	// La frame de début à charger
	unsigned int startFrame = 0;

	bool IsValid() const;
};

class NzAnimation;
class NzSkeleton;

using NzAnimationConstListener = NzObjectListenerWrapper<const NzAnimation>;
using NzAnimationConstRef = NzObjectRef<const NzAnimation>;
using NzAnimationLibrary = NzObjectLibrary<NzAnimation>;
using NzAnimationListener = NzObjectListenerWrapper<NzAnimation>;
using NzAnimationLoader = NzResourceLoader<NzAnimation, NzAnimationParams>;
using NzAnimationManager = NzResourceManager<NzAnimation, NzAnimationParams>;
using NzAnimationRef = NzObjectRef<NzAnimation>;

struct NzAnimationImpl;

class NAZARA_API NzAnimation : public NzRefCounted, public NzResource
{
	friend NzAnimationLibrary;
	friend NzAnimationLoader;
	friend NzAnimationManager;
	friend class NzUtility;

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

		template<typename... Args> static NzAnimationRef New(Args&&... args);

	private:
		static bool Initialize();
		static void Uninitialize();

		NzAnimationImpl* m_impl = nullptr;

		static NzAnimationLibrary::LibraryMap s_library;
		static NzAnimationLoader::LoaderList s_loaders;
		static NzAnimationManager::ManagerMap s_managerMap;
		static NzAnimationManager::ManagerParams s_managerParameters;
};

#include <Nazara/Utility/Animation.inl>

#endif // NAZARA_ANIMATION_HPP
